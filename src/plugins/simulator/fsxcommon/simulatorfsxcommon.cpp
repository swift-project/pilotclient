/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfsxcommon.h"
#include "blackcore/application.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/fscommon/bcdconversions.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include <QTimer>
#include <QPointer>
#include <type_traits>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::Fsx;
using namespace BlackMisc::Weather;
using namespace BlackCore;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        CSimulatorFsxCommon::CSimulatorFsxCommon(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider *weatherGridProvider,
                IClientProvider *clientProvider,
                QObject *parent) :
            CSimulatorFsCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            Q_ASSERT_X(ownAircraftProvider, Q_FUNC_INFO, "Missing provider");
            Q_ASSERT_X(remoteAircraftProvider, Q_FUNC_INFO, "Missing provider");
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing global object");

            m_addPendingSimObjTimer.setInterval(AddPendingAircraftIntervalMs);
            m_useFsuipc = false;
            // default model will be set in derived class

            CSimulatorFsxCommon::registerHelp();
            connect(&m_addPendingSimObjTimer, &QTimer::timeout, this, &CSimulatorFsxCommon::addPendingAircraftByTimer);
        }

        CSimulatorFsxCommon::~CSimulatorFsxCommon()
        {
            this->disconnectFrom();
            // fsuipc is disconnected in CSimulatorFsCommon
        }

        bool CSimulatorFsxCommon::isConnected() const
        {
            return m_simConnected;
        }

        bool CSimulatorFsxCommon::isSimulating() const
        {
            return m_simSimulating;
        }

        bool CSimulatorFsxCommon::connectTo()
        {
            if (this->isConnected()) { return true; }
            this->reset();
            if (FAILED(SimConnect_Open(&m_hSimConnect, sApp->swiftVersionChar(), nullptr, 0, 0, 0)))
            {
                // reset state as expected for unconnected
                return false;
            }
            if (m_useFsuipc) { m_fsuipc->connect(); } // FSUIPC too

            // set structures and move on
            this->initEvents();
            this->initDataDefinitionsWhenConnected();
            m_timerId = startTimer(DispatchIntervalMs);
            // do not start m_addPendingAircraftTimer here, it will be started when object was added

            return true;
        }

        bool CSimulatorFsxCommon::disconnectFrom()
        {
            if (!m_simConnected) { return true; }
            this->safeKillTimer();
            m_simSimulating = false; // treat as stopped, just setting the flag here avoids overhead of on onSimStopped
            if (m_hSimConnect)
            {
                SimConnect_Close(m_hSimConnect);
                m_hSimConnect = nullptr;
            }

            this->reset(); // mark as disconnected and reset all values

            // emit status and disconnect FSUIPC
            CSimulatorFsCommon::disconnectFrom();
            return true;
        }

        bool CSimulatorFsxCommon::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            return this->physicallyAddRemoteAircraftImpl(newRemoteAircraft, ExternalCall);
        }

        bool CSimulatorFsxCommon::updateOwnSimulatorCockpit(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
        {
            if (originator == this->identifier()) { return false; }
            if (!this->isSimulating()) { return false; }

            // actually those data should be the same as ownAircraft
            const CComSystem newCom1 = ownAircraft.getCom1System();
            const CComSystem newCom2 = ownAircraft.getCom2System();
            const CTransponder newTransponder = ownAircraft.getTransponder();

            bool changed = false;
            if (newCom1.getFrequencyActive() != m_simCom1.getFrequencyActive())
            {
                const CFrequency newFreq = newCom1.getFrequencyActive();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom1Active,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;

            }
            if (newCom1.getFrequencyStandby() != m_simCom1.getFrequencyStandby())
            {
                const CFrequency newFreq = newCom1.getFrequencyStandby();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom1Standby,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }

            if (newCom2.getFrequencyActive() != m_simCom2.getFrequencyActive())
            {
                const CFrequency newFreq = newCom2.getFrequencyActive();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom2Active,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }
            if (newCom2.getFrequencyStandby() != m_simCom2.getFrequencyStandby())
            {
                const CFrequency newFreq = newCom2.getFrequencyStandby();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom2Standby,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }

            if (newTransponder.getTransponderCode() != m_simTransponder.getTransponderCode())
            {
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTransponderCode,
                                               CBcdConversions::transponderCodeToBcd(newTransponder), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }

            if (newTransponder.getTransponderMode() != m_simTransponder.getTransponderMode())
            {
                if (m_useSbOffsets)
                {
                    byte ident = newTransponder.isIdentifying() ? 1U : 0U; // 1 is ident
                    byte standby = newTransponder.isInStandby() ? 1U : 0U; // 1 is standby
                    HRESULT hr = S_OK;

                    hr += SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbIdent, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 1, &ident);
                    hr += SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbStandby, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 1, &standby);
                    if (hr != S_OK)
                    {
                        CLogMessage(this).warning("Setting transponder mode failed (SB offsets)");
                    }
                }
                changed = true;
            }

            // avoid changes of cockpit back to old values due to an outdated read back value
            if (changed) { m_skipCockpitUpdateCycles = SkipUpdateCyclesForCockpit; }

            // bye
            return changed;
        }

        bool CSimulatorFsxCommon::updateOwnSimulatorSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            if (originator == this->identifier()) { return false; }
            if (!this->isSimulating()) { return false; }

            //! \fixme KB 2017/8 use SELCAL
            Q_UNUSED(selcal);
            return false;
        }

        void CSimulatorFsxCommon::displayStatusMessage(const CStatusMessage &message) const
        {
            QByteArray m = message.getMessage().toLatin1().constData();
            m.append('\0');

            SIMCONNECT_TEXT_TYPE type = SIMCONNECT_TEXT_TYPE_PRINT_BLACK;
            switch (message.getSeverity())
            {
            case CStatusMessage::SeverityDebug: return;
            case CStatusMessage::SeverityInfo: type = SIMCONNECT_TEXT_TYPE_PRINT_GREEN; break;
            case CStatusMessage::SeverityWarning: type = SIMCONNECT_TEXT_TYPE_PRINT_YELLOW; break;
            case CStatusMessage::SeverityError: type = SIMCONNECT_TEXT_TYPE_PRINT_RED; break;
            }
            const HRESULT hr = SimConnect_Text(m_hSimConnect, type, 7.5, EventTextMessage, static_cast<DWORD>(m.size()), m.data());
            Q_UNUSED(hr);
        }

        void CSimulatorFsxCommon::displayTextMessage(const CTextMessage &message) const
        {
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        bool CSimulatorFsxCommon::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            return m_simConnectObjects.contains(callsign);
        }

        CCallsignSet CSimulatorFsxCommon::physicallyRenderedAircraft() const
        {
            CCallsignSet callsigns(m_simConnectObjects.keys());
            callsigns.push_back(m_addAgainAircraftWhenRemoved.getCallsigns()); // not really rendered right now, but very soon
            callsigns.push_back(m_addPendingAircraft.getCallsigns()); // not really rendered, but for the logic it should look like it is
            return CCallsignSet(m_simConnectObjects.keys());
        }

        CStatusMessageList CSimulatorFsxCommon::debugVerifyStateAfterAllAircraftRemoved() const
        {
            CStatusMessageList msgs;
            if (CBuildConfig::isLocalDeveloperDebugBuild()) { return msgs; }
            msgs = CSimulatorFsCommon::debugVerifyStateAfterAllAircraftRemoved();
            if (!m_simConnectObjects.isEmpty()) { msgs.push_back(CStatusMessage(this).error("m_simConnectObjects not empty: '%1'") << m_simConnectObjects.getAllCallsignStringsAsString(true)); }
            if (!m_simConnectObjectsPositionAndPartsTraces.isEmpty()) { msgs.push_back(CStatusMessage(this).error("m_simConnectObjectsPositionAndPartsTraces not empty: '%1'") << m_simConnectObjectsPositionAndPartsTraces.getAllCallsignStringsAsString(true)); }
            if (!m_addAgainAircraftWhenRemoved.isEmpty()) { msgs.push_back(CStatusMessage(this).error("m_addAgainAircraftWhenRemoved not empty: '%1'") << m_addAgainAircraftWhenRemoved.getCallsignStrings(true).join(", ")); }
            return msgs;
        }

        bool CSimulatorFsxCommon::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
        {
            Q_UNUSED(callsign);
            if (this->isShuttingDown()) { return false; }
            if (reference.isNull()) { return false; }

            static const CAltitude alt(50000, CLengthUnit::ft());
            CCoordinateGeodetic pos(reference);
            pos.setGeodeticHeight(alt);

            if (m_simConnectProbes.isEmpty()) { return this->physicallyAddAITerrainProbe(pos); }
            if (m_simConnectProbes.countConfirmedAdded() < 1) { return false; } // pending probes
            CSimConnectObject simObject = m_simConnectProbes.values().front();

            SIMCONNECT_DATA_INITPOSITION position = this->coordinateToFsxPosition(pos);
            const HRESULT hr = SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                               simObject.getObjectId(), 0, 0,
                               sizeof(SIMCONNECT_DATA_INITPOSITION), &position);
            if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO); }

            if (hr == S_OK)
            {
                this->requestTerrainProbeData(callsign);
            }
            else
            {
                const CStatusMessage msg = CStatusMessage(this).error("Can not request AI position: '%1'") << callsign.asString();
                CLogMessage::preformatted(msg);
            }
            return hr == S_OK;
        }

        bool CSimulatorFsxCommon::stillDisplayReceiveExceptions()
        {
            m_receiveExceptionCount++;
            return m_receiveExceptionCount < IgnoreReceiveExceptions;
        }

        void CSimulatorFsxCommon::setSimConnected()
        {
            m_simConnected = true;
            this->initSimulatorInternals();
            this->emitSimulatorCombinedStatus();

            // Internals depends on sim data which take a while to be read
            // this is a trick and I re-init again after a while (which is not really expensive)
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(1000, this, [myself]
            {
                if (myself.isNull()) { return; }
                myself->initSimulatorInternals();
            });
        }

        void CSimulatorFsxCommon::onSimRunning()
        {
            QTimer::singleShot(DeferSimulatingFlagMs, this, [ = ]
            {
                m_simulatingChangedTs = QDateTime::currentMSecsSinceEpoch();
                this->onSimRunningDefered(m_simulatingChangedTs);
            });
        }

        void CSimulatorFsxCommon::onSimRunningDefered(qint64 referenceTs)
        {
            if (m_simSimulating) { return; }
            if (referenceTs != m_simulatingChangedTs) { return; } // changed, so no longer valid
            m_simSimulating = true; // only place where this should be set to true
            m_simConnected = true;
            HRESULT hr = SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestOwnAircraft,
                         CSimConnectDefinitions::DataOwnAircraft,
                         SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);

            hr += SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestOwnAircraftTitle,
                                                    CSimConnectDefinitions::DataOwnAircraftTitle,
                                                    SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND,
                                                    SIMCONNECT_DATA_REQUEST_FLAG_CHANGED);

            hr += SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestSimEnvironment,
                                                    CSimConnectDefinitions::DataSimEnvironment,
                                                    SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND,
                                                    SIMCONNECT_DATA_REQUEST_FLAG_CHANGED);

            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin: SimConnect_RequestDataOnSimObject failed");
                return;
            }

            // Request the data from SB only when its changed and only ONCE so we don't have to run a 1sec event to get/set this info ;)
            hr += SimConnect_RequestClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::RequestSbData,
                                               CSimConnectDefinitions::DataClientAreaSb, SIMCONNECT_CLIENT_DATA_PERIOD_SECOND, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED);

            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin: SimConnect_RequestClientData failed");
                return;
            }

            this->emitSimulatorCombinedStatus(); // force sending status
        }

        void CSimulatorFsxCommon::onSimStopped()
        {
            // stopping events in FSX: Load menu, weather and season
            CLogMessage(this).info("Simulator stopped: %1") << this->m_simulatorDetails;
            const SimulatorStatus oldStatus = this->getSimulatorStatus();
            m_simSimulating = false;
            m_simulatingChangedTs = QDateTime::currentMSecsSinceEpoch();
            this->emitSimulatorCombinedStatus(oldStatus);
        }

        void CSimulatorFsxCommon::onSimFrame()
        {
            this->updateRemoteAircraft();
        }

        void CSimulatorFsxCommon::onSimExit()
        {
            CLogMessage(this).info("Simulator exit: %1") << this->m_simulatorDetails;

            // reset complete state, we are going down
            m_simulatingChangedTs = QDateTime::currentMSecsSinceEpoch();
            this->safeKillTimer();

            // if called from dispatch function, avoid that SimConnectProc disconnects itself while in SimConnectProc
            QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (myself.isNull()) { return; }
                myself->disconnectFrom();
            });
        }

        SIMCONNECT_DATA_REQUEST_ID CSimulatorFsxCommon::obtainRequestIdForSimData()
        {
            const SIMCONNECT_DATA_REQUEST_ID id = m_requestIdSimData++;
            if (id > RequestIdSimDataEnd) { m_requestIdSimData = RequestIdSimDataStart; }
            return id;
        }

        SIMCONNECT_DATA_REQUEST_ID CSimulatorFsxCommon::obtainRequestIdForProbe()
        {
            const SIMCONNECT_DATA_REQUEST_ID id = m_requestIdProbe++;
            if (id > RequestIdTerrainProbeEnd) { m_requestIdProbe = RequestIdTerrainProbeStart; }
            return id;
        }

        void CSimulatorFsxCommon::updateOwnAircraftFromSimulator(const DataDefinitionOwnAircraft &simulatorOwnAircraft)
        {
            CSimulatedAircraft myAircraft(getOwnAircraft());
            CCoordinateGeodetic position;
            position.setLatitude(CLatitude(simulatorOwnAircraft.latitude, CAngleUnit::deg()));
            position.setLongitude(CLongitude(simulatorOwnAircraft.longitude, CAngleUnit::deg()));

            if (simulatorOwnAircraft.pitch < -90.0 || simulatorOwnAircraft.pitch >= 90.0)
            {
                CLogMessage(this).warning("FSX: Pitch value (own aircraft) out of limits: %1") << simulatorOwnAircraft.pitch;
            }
            CAircraftSituation aircraftSituation;
            aircraftSituation.setPosition(position);
            // MSFS has inverted pitch and bank angles
            aircraftSituation.setPitch(CAngle(-simulatorOwnAircraft.pitch, CAngleUnit::deg()));
            aircraftSituation.setBank(CAngle(-simulatorOwnAircraft.bank, CAngleUnit::deg()));
            aircraftSituation.setHeading(CHeading(simulatorOwnAircraft.trueHeading, CHeading::True, CAngleUnit::deg()));
            aircraftSituation.setGroundSpeed(CSpeed(simulatorOwnAircraft.velocity, CSpeedUnit::kts()));
            aircraftSituation.setGroundElevation(CAltitude(simulatorOwnAircraft.elevation, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
            aircraftSituation.setAltitude(CAltitude(simulatorOwnAircraft.altitude, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
            aircraftSituation.setPressureAltitude(CAltitude(simulatorOwnAircraft.pressureAltitude, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
            // set on ground also in situation for consistency and future usage
            // it is duplicated in parts
            aircraftSituation.setOnGround(simulatorOwnAircraft.simOnGround ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround, CAircraftSituation::OutOnGroundOwnAircraft);

            const CAircraftLights lights(simulatorOwnAircraft.lightStrobe,
                                         simulatorOwnAircraft.lightLanding,
                                         simulatorOwnAircraft.lightTaxi,
                                         simulatorOwnAircraft.lightBeacon,
                                         simulatorOwnAircraft.lightNav,
                                         simulatorOwnAircraft.lightLogo);

            CAircraftEngineList engines;
            const QList<bool> helperList
            {
                simulatorOwnAircraft.engine1Combustion != 0, simulatorOwnAircraft.engine2Combustion != 0,
                simulatorOwnAircraft.engine3Combustion != 0, simulatorOwnAircraft.engine4Combustion != 0
            };

            for (int index = 0; index < simulatorOwnAircraft.numberOfEngines; ++index)
            {
                engines.push_back(CAircraftEngine(index + 1, helperList.at(index)));
            }

            const CAircraftParts parts(lights, simulatorOwnAircraft.gearHandlePosition,
                                       simulatorOwnAircraft.flapsHandlePosition * 100,
                                       simulatorOwnAircraft.spoilersHandlePosition,
                                       engines,
                                       simulatorOwnAircraft.simOnGround);

            // set values
            this->updateOwnSituation(aircraftSituation);
            this->updateOwnParts(parts);

            // When I change cockpit values in the sim (from GUI to simulator, not originating from simulator)
            // it takes a little while before these values are set in the simulator.
            // To avoid jitters, I wait some update cylces to stabilize the values
            if (m_skipCockpitUpdateCycles < 1)
            {
                // defaults
                CComSystem com1(myAircraft.getCom1System()); // set defaults
                CComSystem com2(myAircraft.getCom2System());
                CTransponder transponder(myAircraft.getTransponder());

                // updates
                com1.setFrequencyActive(CFrequency(simulatorOwnAircraft.com1ActiveMHz, CFrequencyUnit::MHz()));
                com1.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com1StandbyMHz, CFrequencyUnit::MHz()));
                const bool changedCom1 = myAircraft.getCom1System() != com1;
                m_simCom1 = com1;

                com2.setFrequencyActive(CFrequency(simulatorOwnAircraft.com2ActiveMHz, CFrequencyUnit::MHz()));
                com2.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com2StandbyMHz, CFrequencyUnit::MHz()));
                const bool changedCom2 = myAircraft.getCom2System() != com2;
                m_simCom2 = com2;

                transponder.setTransponderCode(simulatorOwnAircraft.transponderCode);
                const bool changedXpr = (myAircraft.getTransponderCode() != transponder.getTransponderCode());

                if (changedCom1 || changedCom2 || changedXpr)
                {
                    this->updateCockpit(com1, com2, transponder, identifier());
                }
            }
            else
            {
                --m_skipCockpitUpdateCycles;
            }

            if (m_isWeatherActivated)
            {
                const auto currentPosition = CCoordinateGeodetic { aircraftSituation.latitude(), aircraftSituation.longitude(), {0} };
                if (CWeatherScenario::isRealWeatherScenario(m_weatherScenarioSettings.get()) &&
                        calculateGreatCircleDistance(m_lastWeatherPosition, currentPosition).value(CLengthUnit::mi()) > 20)
                {
                    m_lastWeatherPosition = currentPosition;
                    const auto weatherGrid = CWeatherGrid { { "GLOB", currentPosition } };
                    requestWeatherGrid(weatherGrid, { this, &CSimulatorFsxCommon::injectWeatherGrid });
                }
            }
        }

        void CSimulatorFsxCommon::updateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionRemoteAircraftSimData &remoteAircraftData)
        {
            // Near ground we use faster updates
            if (remoteAircraftData.aboveGround() <= 100.0)
            {
                // switch to fast updates
                if (simObject.getSimDataPeriod() != SIMCONNECT_PERIOD_VISUAL_FRAME)
                {
                    this->requestPositionDataForSimObject(simObject, SIMCONNECT_PERIOD_VISUAL_FRAME);
                }
            }
            else
            {
                // switch to slow updates
                if (simObject.getSimDataPeriod() != SIMCONNECT_PERIOD_SECOND)
                {
                    this->requestPositionDataForSimObject(simObject, SIMCONNECT_PERIOD_SECOND);
                }
            }

            // CElevationPlane: deg, deg, feet
            // we only remember near ground
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(simObject.getCallsign());
            if (simObject.getLastInterpolatedSituation(setup.getInterpolatorMode()).canLikelySkipNearGroundInterpolation()) { return; }

            CElevationPlane elevation(remoteAircraftData.latitudeDeg, remoteAircraftData.longitudeDeg, remoteAircraftData.elevationFt);
            elevation.setSinglePointRadius();
            this->rememberElevationAndCG(simObject.getCallsign(), elevation, CLength(remoteAircraftData.cgToGroundFt, CLengthUnit::ft()));
        }

        void CSimulatorFsxCommon::updatProbeFromSimulator(const CCallsign &callsign, const DataDefinitionRemoteAircraftSimData &remoteAircraftData)
        {
            CElevationPlane elevation(remoteAircraftData.latitudeDeg, remoteAircraftData.longitudeDeg, remoteAircraftData.elevationFt);
            elevation.setSinglePointRadius();
            this->callbackReceivedRequestedElevation(elevation, callsign);
        }

        void CSimulatorFsxCommon::updateOwnAircraftFromSimulator(const DataDefinitionClientAreaSb &sbDataArea)
        {
            CTransponder::TransponderMode newMode;
            if (sbDataArea.isIdent())
            {
                newMode = CTransponder::StateIdent;
            }
            else
            {
                newMode = sbDataArea.isStandby() ? CTransponder::StateStandby : CTransponder::ModeC;
            }
            const CSimulatedAircraft myAircraft(this->getOwnAircraft());
            const bool changed = (myAircraft.getTransponderMode() != newMode);
            if (!changed) { return; }
            CTransponder xpdr = myAircraft.getTransponder();
            xpdr.setTransponderMode(newMode);
            this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), xpdr, this->identifier());
        }

        bool CSimulatorFsxCommon::simulatorReportedObjectAdded(DWORD objectId)
        {
            if (this->isShuttingDown()) { return true; } // pretend everything is fine
            const CSimConnectObject simObject = m_simConnectObjects.getSimObjectForObjectId(objectId);
            const CCallsign callsign(simObject.getCallsign());
            if (!simObject.hasValidRequestAndObjectId() || callsign.isEmpty()) { return false; }

            // we know the object has been created. But it can happen it is directly removed afterwards
            const CSimulatedAircraft verifyAircraft(simObject.getAircraft());
            QTimer::singleShot(1000, this, [ = ]
            {
                // also triggers new add if required
                this->verifyAddedRemoteAircraft(verifyAircraft);
            });
            return true;
        }

        bool CSimulatorFsxCommon::simulatorReportedProbeAdded(DWORD objectId)
        {
            if (this->isShuttingDown()) { return true; } // pretend everything is fine
            const CSimConnectObject simObject = m_simConnectProbes.markObjectAsAdded(objectId);
            const bool valid(simObject.hasValidRequestAndObjectId() && simObject.isConfirmedAdded());
            return valid;
        }

        void CSimulatorFsxCommon::verifyAddedRemoteAircraft(const CSimulatedAircraft &remoteAircraftIn)
        {
            if (this->isShuttingDown()) { return; }

            CStatusMessage msg;
            CSimulatedAircraft remoteAircraft = remoteAircraftIn;
            const CCallsign callsign(remoteAircraft.getCallsign());

            do
            {
                // no callsign
                if (callsign.isEmpty())
                {
                    msg = CLogMessage(this).error("Cannot confirm AI object, empty callsign");
                    break;
                }

                // removed in meantime
                const bool aircraftStillInRange = this->isAircraftInRange(callsign);
                if (!m_simConnectObjects.contains(callsign))
                {
                    if (aircraftStillInRange)
                    {
                        msg = CLogMessage(this).warning("Callsign '%1' removed in meantime, but still in range") << callsign.toQString();
                    }
                    else
                    {
                        this->removeFromAddPendingAndAddAgainAircraft(callsign);
                        msg = CLogMessage(this).info("Callsign '%1' removed in meantime and no longer in range") << callsign.toQString();
                    }
                    break;
                }

                CSimConnectObject &simObject = m_simConnectObjects[callsign];
                remoteAircraft = simObject.getAircraft(); // update, if something has changed

                if (!simObject.hasValidRequestAndObjectId() || simObject.isPendingRemoved())
                {
                    msg = CStatusMessage(this).warning("Object for callsign '%1'/id: %2 removed in meantime/invalid") << callsign.toQString() << simObject.getObjectId();
                    break;
                }

                Q_ASSERT_X(simObject.isPendingAdded(), Q_FUNC_INFO, "already confirmed, this should be the only place");
                simObject.setConfirmedAdded(true);

                // P3D also has SimConnect_AIReleaseControlEx which also allows to destroy the aircraft
                const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimData();
                const DWORD objectId = simObject.getObjectId();
                HRESULT hr = SimConnect_AIReleaseControl(m_hSimConnect, objectId, requestId);
                hr += SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeLat, 1,
                                                     SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                hr += SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAlt, 1,
                                                     SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                hr += SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAtt, 1,
                                                     SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

                if (hr != S_OK)
                {
                    msg = CStatusMessage(this).error("Cannot confirm object %1, cs: '%2' model: '%3'") << objectId << remoteAircraft.getCallsignAsString() << remoteAircraft.getModelString();
                    break;
                }

                // request data on object
                this->requestPositionDataForSimObject(simObject);
                this->requestLightsForSimObject(simObject);

                this->removeFromAddPendingAndAddAgainAircraft(callsign); // no longer try to add
                const bool updated = this->updateAircraftRendered(callsign, true);
                if (updated)
                {
                    emit aircraftRenderingChanged(simObject.getAircraft());
                    if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' model: '%2' verified, request/object id: %3 %4").arg(callsign.toQString(), remoteAircraft.getModelString()).arg(requestId).arg(objectId)); }
                }
            }
            while (false);

            // log errors and emit signal
            if (!msg.isEmpty())
            {
                CLogMessage::preformatted(msg);
                emit this->physicallyAddingRemoteModelFailed(CSimulatedAircraft(), msg);
            }

            // trigger new adding
            if (!m_addPendingAircraft.isEmpty())
            {
                this->addPendingAircraftAfterAdded();
            }
        }

        void CSimulatorFsxCommon::addPendingAircraftByTimer()
        {
            this->addPendingAircraft(AddByTimer);
        }

        void CSimulatorFsxCommon::addPendingAircraftAfterAdded()
        {
            this->addPendingAircraft(AddAfterAdded);
        }

        void CSimulatorFsxCommon::addPendingAircraft(AircraftAddMode mode)
        {
            if (m_addPendingAircraft.isEmpty()) { return; }
            const CCallsignSet aircraftCallsignsInRange(getAircraftInRangeCallsigns());
            CSimulatedAircraftList toBeAddedAircraft;
            CCallsignSet toBeRemovedCallsigns;
            for (const CSimulatedAircraft &aircraft : as_const(m_addPendingAircraft))
            {
                Q_ASSERT_X(!aircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "missing callsign");
                if (aircraftCallsignsInRange.contains(aircraft.getCallsign()))
                {
                    toBeAddedAircraft.push_back(aircraft);
                }
                else
                {
                    toBeRemovedCallsigns.push_back(aircraft.getCallsign());
                }
            }

            // no longer required to be added
            m_addPendingAircraft.removeByCallsigns(toBeRemovedCallsigns);
            m_addAgainAircraftWhenRemoved.removeByCallsigns(toBeRemovedCallsigns);

            // add aircraft, but "non blocking"
            if (!toBeAddedAircraft.isEmpty())
            {
                const CSimulatedAircraft nextPendingAircraft(m_addPendingAircraft.front());
                const QPointer <CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(100, this, [ = ]
                {
                    if (myself.isNull()) { return; }
                    this->physicallyAddRemoteAircraftImpl(nextPendingAircraft, mode);
                });
            }
        }

        void CSimulatorFsxCommon::removeFromAddPendingAndAddAgainAircraft(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return; }
            m_addPendingAircraft.removeByCallsign(callsign);
            m_addAgainAircraftWhenRemoved.removeByCallsign(callsign);
        }

        bool CSimulatorFsxCommon::simulatorReportedObjectRemoved(DWORD objectID)
        {
            const CSimConnectObject simObject = m_simConnectObjects.getSimObjectForObjectId(objectID);
            if (!simObject.hasValidRequestAndObjectId()) { return false; } // object id from somewhere else
            const CCallsign callsign(simObject.getCallsign());
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign for removed object");

            if (simObject.isPendingRemoved())
            {
                // good case, object has been removed
                // we can remove the sim object
            }
            else if (!this->isShuttingDown())
            {
                // object was removed, but removal was not requested by us
                // this means we are out of the reality bubble or something else went wrong
                // Possible reasons:
                // 1) out of reality bubble
                // 2) wrong position (in ground etc.)
                // 3) Simulator not running (ie in stopped mode)
                CStatusMessage msg;
                if (!simObject.getAircraftModelString().isEmpty())
                {
                    const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
                    m_addPendingAircraft.replaceOrAddByCallsign(simObject.getAircraft());
                    msg = CLogMessage(this).warning("Aircraft removed, '%1' '%2' object id '%3' out of reality bubble or other reason. Interpolator: '%4'")
                          << callsign.toQString() << simObject.getAircraftModelString()
                          << objectID << simObject.getInterpolatorInfo(setup.getInterpolatorMode());
                }
                else
                {
                    msg = CLogMessage(this).warning("Removed '%1' from simulator, but was not initiated by us: %1 '%2' object id %3") << callsign.toQString() << simObject.getAircraftModelString() << objectID;
                }
                emit this->driverMessages(msg);
            }

            // in all cases we remove the object
            const int c = m_simConnectObjects.remove(callsign);
            const bool removedAny = (c > 0);
            const bool updated = this->updateAircraftRendered(simObject.getCallsign(), false);
            if (updated)
            {
                emit this->aircraftRenderingChanged(simObject.getAircraft());
            }

            // models we have to add again after removing
            if (m_addAgainAircraftWhenRemoved.containsCallsign(callsign))
            {
                const CSimulatedAircraft aircraftAddAgain = m_addAgainAircraftWhenRemoved.findFirstByCallsign(callsign);
                m_addAgainAircraftWhenRemoved.removeByCallsign(callsign);
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2500, this, [ = ]
                {
                    if (myself.isNull()) { return; }
                    myself->physicallyAddRemoteAircraftImpl(aircraftAddAgain, AddedAfterRemoved);
                });
            }
            return removedAny;
        }

        bool CSimulatorFsxCommon::setSimConnectObjectId(DWORD requestId, DWORD objectId)
        {
            return m_simConnectObjects.setSimConnectObjectIdForRequestId(requestId, objectId, true);
        }

        bool CSimulatorFsxCommon::setSimConnectProbeId(DWORD requestId, DWORD objectId)
        {
            return m_simConnectProbes.setSimConnectObjectIdForRequestId(requestId, objectId, true);
        }

        bool CSimulatorFsxCommon::setCurrentLights(const CCallsign &callsign, const CAircraftLights &lights)
        {
            if (!m_simConnectObjects.contains(callsign)) { return false; }
            m_simConnectObjects[callsign].setCurrentLightsInSimulator(lights);
            return true;
        }

        bool CSimulatorFsxCommon::setLightsAsSent(const CCallsign &callsign, const CAircraftLights &lights)
        {
            if (!m_simConnectObjects.contains(callsign)) { return false; }
            m_simConnectObjects[callsign].setLightsAsSent(lights);
            return true;
        }

        void CSimulatorFsxCommon::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);
            if (this->isShuttingDown()) { return; }
            this->dispatch();
        }

        bool CSimulatorFsxCommon::parseDetails(const CSimpleCommandParser &parser)
        {
            // .driver sendid on|off
            if (parser.matchesPart(1, "sendid") && parser.hasPart(2))
            {
                const bool trace = parser.toBool(2);
                this->setTraceSendId(trace);
                CLogMessage(this, CLogCategory::cmdLine()).info("Tracing %1 driver sendIds is '%2'") << this->getSimulatorPluginInfo().getIdentifier() << boolToOnOff(trace);
                return true;
            }
            return CSimulatorFsCommon::parseDetails(parser);
        }

        void CSimulatorFsxCommon::registerHelp()
        {
            if (CSimpleCommandParser::registered("BlackSimPlugin::CSimulatorFsxCommon::CSimulatorFsxCommon")) { return; }
            CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
            CSimpleCommandParser::registerCommand({".drv sendid on|off", "Trace simConnect sendId on|off"});
        }

        CCallsign CSimulatorFsxCommon::getCallsignForPendingProbeRequests(DWORD requestId, bool remove)
        {
            const CCallsign cs = m_pendingProbeRequests.value(requestId);
            if (remove) { m_pendingProbeRequests.remove(requestId); }
            return cs;
        }

        const QString &CSimulatorFsxCommon::modeToString(CSimulatorFsxCommon::AircraftAddMode mode)
        {
            static const QString e("external call");
            static const QString pt("add pending by timer");
            static const QString oa("add pending after object added");
            static const QString ar("add again after removed");
            static const QString dontKnow("???");

            switch (mode)
            {
            case ExternalCall: return e;
            case AddByTimer: return pt;
            case AddAfterAdded: return oa;
            case AddedAfterRemoved: return ar;
            default: break;
            }
            return dontKnow;
        }

        void CSimulatorFsxCommon::dispatch()
        {
            // call CSimulatorFsxCommon::SimConnectProc or specialized P3D version
            Q_ASSERT_X(m_dispatchProc, Q_FUNC_INFO, "Missing DispatchProc");
            const HRESULT hr = SimConnect_CallDispatch(m_hSimConnect, m_dispatchProc, this);
            if (hr != S_OK)
            {
                m_dispatchErrors++;
                if (m_dispatchErrors == 2)
                {
                    // 2nd time, an error / avoid multiple messages
                    // idea: if it happens once ignore
                    CLogMessage(this).error("%1: Dispatch error") << this->getSimulatorPluginInfo().getIdentifier();
                }
                else if (m_dispatchErrors > 5)
                {
                    // this normally happens during a FSX crash or shutdown with simconnect
                    CLogMessage(this).error("%1: Multiple dispatch errors, disconnecting") << this->getSimulatorPluginInfo().getIdentifier();
                    this->disconnectFrom();
                }
                return;
            }
            m_dispatchErrors = 0;
            if (m_useFsuipc && m_fsuipc)
            {
                CSimulatedAircraft fsuipcAircraft(getOwnAircraft());
                //! \fixme split in high / low frequency reads
                bool ok = m_fsuipc->read(fsuipcAircraft, true, true, true);
                if (ok)
                {
                    // do whatever is required
                    Q_UNUSED(fsuipcAircraft);
                }
            }
        }

        bool CSimulatorFsxCommon::physicallyAddRemoteAircraftImpl(const CSimulatedAircraft &newRemoteAircraft, CSimulatorFsxCommon::AircraftAddMode addMode)
        {
            const CCallsign callsign(newRemoteAircraft.getCallsign());

            // entry checks
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
            Q_ASSERT_X(newRemoteAircraft.hasModelString(), Q_FUNC_INFO, "missing model string");

            // reset timer
            m_addPendingSimObjTimer.start(AddPendingAircraftIntervalMs); // restart

            const bool hasPendingAdded = m_simConnectObjects.containsPendingAdded();
            bool canAdd = m_simSimulating && m_simConnected && !hasPendingAdded;

            Q_ASSERT_X(!hasPendingAdded || m_simConnectObjects.countPendingAdded() < 2, Q_FUNC_INFO, "There must be only 0..1 pending objects");
            if (this->showDebugLogMessage())
            {
                this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' mode: '%2' model: '%3'").arg(newRemoteAircraft.getCallsignAsString(), modeToString(addMode), newRemoteAircraft.getModelString()));
                this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' pending callsigns: '%2', pending objects: '%3'").arg(newRemoteAircraft.getCallsignAsString(), m_addPendingAircraft.getCallsignStrings().join(", "), m_simConnectObjects.getPendingAddedCallsigns().getCallsignStrings().join(", ")));
            }

            // do we need to remove/add again because something has changed
            if (m_simConnectObjects.contains(callsign))
            {
                const CSimConnectObject simObject = m_simConnectObjects[callsign];
                const QString newModelString(newRemoteAircraft.getModelString());
                const QString simObjModelString(simObject.getAircraftModelString());
                const bool sameModel = (simObjModelString == newModelString); // compare on string only (other attributes might change such as mode)

                // same model, nothing will change, otherwise add again when removed
                if (sameModel)
                {
                    if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' re-added same model '%2'").arg(newRemoteAircraft.getCallsignAsString(), newModelString)); }
                    return true;
                }

                this->physicallyRemoveRemoteAircraft(newRemoteAircraft.getCallsign());
                m_addAgainAircraftWhenRemoved.replaceOrAddByCallsign(newRemoteAircraft);
                if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' re-added changed model '%2', will be added again").arg(newRemoteAircraft.getCallsignAsString(), newModelString)); }
                return false;
            }

            // situation check
            CAircraftSituation situation(newRemoteAircraft.getSituation());
            if (canAdd && situation.isPositionOrAltitudeNull())
            {
                // invalid position
                const CAircraftSituationList situations(this->remoteAircraftSituations(callsign));
                if (situations.isEmpty())
                {
                    CLogMessage(this).warning("No valid situations for '%1', will be added as pending") << callsign.asString();
                    canAdd = false;
                }
                else
                {
                    CLogMessage(this).warning("Invalid aircraft situation for new aircraft '%1', use situation") << callsign.asString();
                    situation = situations.findClosestTimeDistanceAdjusted(QDateTime::currentMSecsSinceEpoch());
                    Q_ASSERT_X(!situation.isPositionOrAltitudeNull(), Q_FUNC_INFO, "Invalid situation for new aircraft");
                }
            }

            // check if we can add, do not add if simulator is stopped or other objects pending
            if (!canAdd)
            {
                m_addPendingAircraft.replaceOrAddByCallsign(newRemoteAircraft);
                return false;
            }

            this->removeFromAddPendingAndAddAgainAircraft(callsign);

            // create AI
            if (!this->isAircraftInRange(callsign))
            {
                CLogMessage(this).info("Skipping adding of '%1' since it is no longer in range") << callsign.asString();
                return false;
            }

            // setup
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign);
            const bool sendGround = setup.sendGndFlagToSimulator();

            // FSX/P3D adding
            bool adding = false; // will be added flag
            const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimData();
            const SIMCONNECT_DATA_INITPOSITION initialPosition = CSimulatorFsxCommon::aircraftSituationToFsxPosition(newRemoteAircraft.getSituation(), sendGround);
            const QString modelString(newRemoteAircraft.getModelString());
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' model: '%2' request: %3, init pos: %4").arg(callsign.toQString(), modelString).arg(requestId).arg(fsxPositionToString(initialPosition))); }

            const HRESULT hr = SimConnect_AICreateNonATCAircraft(m_hSimConnect, qPrintable(modelString), qPrintable(callsign.toQString().left(12)), initialPosition, requestId);
            if (hr != S_OK)
            {
                const CStatusMessage msg = CStatusMessage(this).error("SimConnect, can not create AI traffic: '%1' '%2'") << callsign.toQString() << modelString;
                CLogMessage::preformatted(msg);
                emit this->physicallyAddingRemoteModelFailed(newRemoteAircraft, msg);
            }
            else
            {
                // we will request a new aircraft by request ID, later we will receive its object id
                // so far this object id is -1
                const CSimConnectObject simObject = this->insertNewSimConnectObject(newRemoteAircraft, requestId);
                if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO);}
                adding = true;
            }
            return adding;
        }

        bool CSimulatorFsxCommon::physicallyAddAITerrainProbe(const ICoordinateGeodetic &coordinate)
        {
            if (coordinate.isNull()) { return false; }
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");

            // static const QString modelString = this->getDefaultModel().getModelString();
            static const QString modelString("OrcaWhale");
            static const QString pseudoCallsign("swift pr: %1"); // max 12 chars
            const int index = m_simConnectProbes.size() + 1;
            const CCallsign cs(pseudoCallsign.arg(index));
            const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForProbe();
            const SIMCONNECT_DATA_INITPOSITION initialPosition = CSimulatorFsxCommon::coordinateToFsxPosition(coordinate);
            // const HRESULT hr = SimConnect_AICreateNonATCAircraft(m_hSimConnect, qPrintable(modelString), qPrintable(cs.asString().right(12)), initialPosition, requestId);
            const HRESULT hr = SimConnect_AICreateSimulatedObject(m_hSimConnect, qPrintable(modelString), initialPosition, requestId);
            if (m_traceSendId) { this->traceSendId(0, Q_FUNC_INFO, QString("Adding probe, req.id: %1").arg(requestId));}

            bool ok = false;
            if (hr == S_OK)
            {
                ok = true;
                const CAircraftModel model(modelString, CAircraftModel::TypeTerrainProbe, QStringLiteral("swift terrain probe"), CAircraftIcaoCode::unassignedIcao());
                const CAircraftSituation situation(cs, coordinate);
                const CSimulatedAircraft pseudoAircraft(cs, model, CUser("123456", "swift", cs), situation);
                CSimConnectObject simObj(CSimConnectObject::Probe);
                simObj.setRequestId(requestId);
                simObj.setAircraft(pseudoAircraft);
                m_simConnectProbes.insert(cs, simObj);
            }
            else
            {
                const CStatusMessage msg = CStatusMessage(this).error("SimConnect, can not create terrain AI: '%1'") << requestId;
                CLogMessage::preformatted(msg);
            }
            return ok;
        }

        bool CSimulatorFsxCommon::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            // only remove from sim
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "wrong thread");
            if (callsign.isEmpty()) { return false; } // can happen if an object is not an aircraft

            // clean up anyway
            this->removeFromAddPendingAndAddAgainAircraft(callsign);

            // really remove from simulator
            if (!m_simConnectObjects.contains(callsign)) { return false; } // already fully removed or not yet added
            CSimConnectObject &simObject = m_simConnectObjects[callsign];
            if (simObject.isPendingRemoved()) { return true; }

            const bool pendingAdded = simObject.isPendingAdded();
            const bool stillWaitingForLights = !simObject.hasCurrentLightsInSimulator();
            if (pendingAdded || stillWaitingForLights)
            {
                // problem: we try to delete an aircraft just requested to be added
                // best solution so far, call remove again with a delay
                simObject.fakeCurrentLightsInSimulator(); // next time looks like we have lights
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2000, this, [ = ]
                {
                    if (myself.isNull()) { return; }
                    myself->physicallyRemoveRemoteAircraft(callsign);
                });
                return false; // not yet deleted
            }

            // avoid further data from simulator
            this->stopRequestingDataForSimObject(simObject);
            simObject.setPendingRemoved(true);
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1' request/object id: %2/%3").arg(callsign.toQString()).arg(simObject.getRequestId()).arg(simObject.getObjectId())); }

            // call in SIM
            const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimData();
            const HRESULT result = SimConnect_AIRemoveObject(m_hSimConnect, static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), requestId);
            if (result == S_OK)
            {
                if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO);}
            }
            else
            {
                CLogMessage(this).warning("Removing aircraft '%1' from simulator failed") << callsign.asString();
            }

            // mark in provider
            const bool updated = this->updateAircraftRendered(callsign, false);
            if (updated)
            {
                CSimulatedAircraft aircraft(simObject.getAircraft());
                aircraft.setRendered(false);
                emit this->aircraftRenderingChanged(aircraft);
            }

            // cleanup function, actually this should not be needed
            const QPointer<CSimulatorFsxCommon> guard(this);
            QTimer::singleShot(100, this, [ = ]
            {
                if (guard.isNull()) { return; }
                CSimulatorFsxCommon::physicallyRemoveAircraftNotInProvider();
            });

            // bye
            return true;
        }

        int CSimulatorFsxCommon::physicallyRemoveAllRemoteAircraft()
        {
            // make sure they are not added again
            // cleaning here is somewhat redundant, but double checks
            this->resetHighlighting();
            m_addPendingAircraft.clear();
            m_addAgainAircraftWhenRemoved.clear();

            // remove one by one
            int r = 0;
            const CCallsignSet callsigns = m_simConnectObjects.getAllCallsigns();
            for (const CCallsign &cs : callsigns)
            {
                if (this->physicallyRemoveRemoteAircraft(cs)) { r++; }
            }
            return r;
        }

        HRESULT CSimulatorFsxCommon::initEvents()
        {
            HRESULT hr = S_OK;
            // System events, see http://msdn.microsoft.com/en-us/library/cc526983.aspx#SimConnect_SubscribeToSystemEvent
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventSimStatus, "Sim");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventObjectAdded, "ObjectAdded");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventObjectRemoved, "ObjectRemoved");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventFrame, "Frame");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventPause, "Pause");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventFlightLoaded, "FlightLoaded");
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin error: %1") << "SimConnect_SubscribeToSystemEvent failed";
                return hr;
            }

            // Mapped events, see event ids here:
            // http://msdn.microsoft.com/en-us/library/cc526980.aspx
            // http://www.prepar3d.com/SDKv2/LearningCenter/utilities/variables/event_ids.html
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventPauseToggle, "PAUSE_TOGGLE");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, SystemEventSlewToggle, "SLEW_TOGGLE");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeLat, "FREEZE_LATITUDE_LONGITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeAlt, "FREEZE_ALTITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeAtt, "FREEZE_ATTITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom1Active, "COM_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom1Standby, "COM_STBY_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom2Active, "COM2_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom2Standby, "COM2_STBY_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetTransponderCode, "XPNDR_SET");

            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetTimeZuluYear, "ZULU_YEAR_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetTimeZuluDay, "ZULU_DAY_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetTimeZuluHours, "ZULU_HOURS_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetTimeZuluMinutes, "ZULU_MINUTES_SET");

            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventLandingLightsOff, "LANDING_LIGHTS_OFF");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventLandinglightsOn, "LANDING_LIGHTS_ON");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventLandingLightsSet, "LANDING_LIGHTS_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventLandingLightsToggle, "LANDING_LIGHTS_TOGGLE");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventPanelLightsOff, "PANEL_LIGHTS_OFF");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventPanelLightsOn, "PANEL_LIGHTS_ON");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventPanelLightsSet, "PANEL_LIGHTS_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventStrobesOff, "STROBES_OFF");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventStrobesOn, "STROBES_ON");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventStrobesSet, "STROBES_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventStrobesToggle, "STROBES_TOGGLE");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleBeaconLights, "TOGGLE_BEACON_LIGHTS");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleCabinLights, "TOGGLE_CABIN_LIGHTS");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleLogoLights, "TOGGLE_LOGO_LIGHTS");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleNavLights, "TOGGLE_NAV_LIGHTS");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleRecognitionLights, "TOGGLE_RECOGNITION_LIGHTS");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleTaxiLights, "TOGGLE_TAXI_LIGHTS");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleWingLights, "TOGGLE_WING_LIGHTS");

            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin error: %1") << "SimConnect_MapClientEventToSimEvent failed";
                return hr;
            }

            // facility
            SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimData();
            hr += SimConnect_SubscribeToFacilities(m_hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT, requestId);
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin error: %1") << "SimConnect_SubscribeToFacilities failed";
                return hr;
            }
            return hr;
        }

        HRESULT CSimulatorFsxCommon::initDataDefinitionsWhenConnected()
        {
            return CSimConnectDefinitions::initDataDefinitionsWhenConnected(m_hSimConnect);
        }

        HRESULT CSimulatorFsxCommon::initWhenConnected()
        {
            // called when connected

            HRESULT hr = this->initEvents();
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin: initEvents failed");
                return hr;
            }

            // init data definitions and SB data area
            hr += this->initDataDefinitionsWhenConnected();
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin: initDataDefinitionsWhenConnected failed");
                return hr;
            }

            return hr;
        }

        void CSimulatorFsxCommon::updateRemoteAircraft()
        {
            static_assert(sizeof(DataDefinitionRemoteAircraftPartsWithoutLights) == sizeof(double) * 10, "DataDefinitionRemoteAircraftPartsWithoutLights has an incorrect size.");
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "thread");

            // Freeze interpolation while paused
            if (this->isPaused() && m_pausedSimFreezesInterpolation) { return; }

            // nothing to do, reset request id and exit
            const int remoteAircraftNo = this->getAircraftInRangeCount();
            if (remoteAircraftNo < 1) { m_interpolationRequest = 0;  return; }

            // interpolate and send to simulator
            m_interpolationRequest++;

            // values used for position and parts
            const qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

            // interpolation for all remote aircraft
            const QList<CSimConnectObject> simObjects(m_simConnectObjects.values());

            for (const CSimConnectObject &simObject : simObjects)
            {
                // happening if aircraft is not yet added to simulator or to be deleted
                if (simObject.isPendingAdded())   { continue; }
                if (simObject.isPendingRemoved()) { continue; }
                if (!simObject.hasCurrentLightsInSimulator()) { continue; } // wait until we have light state

                const CCallsign callsign(simObject.getCallsign());
                Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");
                Q_ASSERT_X(simObject.hasValidRequestAndObjectId(), Q_FUNC_INFO, "Missing ids");

                // setup
                const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign);
                const bool sendGround = setup.sendGndFlagToSimulator();

                // Interpolated situation
                const CInterpolationResult result = simObject.getInterpolation(currentTimestamp, setup);
                if (result.getInterpolationStatus().hasValidSituation())
                {
                    // update situation
                    SIMCONNECT_DATA_INITPOSITION position = this->aircraftSituationToFsxPosition(result, sendGround);
                    if (!simObject.isSameAsSent(position))
                    {
                        m_simConnectObjects[simObject.getCallsign()].setPositionAsSent(position);
                        const HRESULT hr = SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                                           static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), 0, 0,
                                           sizeof(SIMCONNECT_DATA_INITPOSITION), &position);
                        if (hr == S_OK)
                        {
                            if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO); }
                        }
                        else
                        {
                            CLogMessage(this).warning("Failed so set position on SimObject '%1' callsign: '%2'") << simObject.getObjectId() << callsign;
                        }
                    }
                }
                else
                {
                    static const QString so("SimObject id: %1");
                    CLogMessage(this).warning(this->getInvalidSituationLogMessage(callsign, result.getInterpolationStatus(), so.arg(simObject.getObjectId())));
                }

                // Interpolated parts
                this->updateRemoteAircraftParts(simObject, result);

            } // all callsigns

            const qint64 dt = QDateTime::currentMSecsSinceEpoch() - currentTimestamp;
            m_statsUpdateAircraftTimeTotalMs += dt;
            m_statsUpdateAircraftCountMs++;
            m_statsUpdateAircraftTimeAvgMs = m_statsUpdateAircraftTimeTotalMs / m_statsUpdateAircraftCountMs;
        }

        bool CSimulatorFsxCommon::updateRemoteAircraftParts(const CSimConnectObject &simObject, const CInterpolationResult &result)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }

            const CAircraftParts parts = result;
            if (parts.getPartsDetails() != CAircraftParts::GuessedParts && !result.getPartsStatus().isSupportingParts()) { return false; }

            DataDefinitionRemoteAircraftPartsWithoutLights ddRemoteAircraftPartsWithoutLights(parts); // no init, all values will be set
            return this->sendRemoteAircraftPartsToSimulator(simObject, ddRemoteAircraftPartsWithoutLights, parts.getAdjustedLights());
        }

        bool CSimulatorFsxCommon::sendRemoteAircraftPartsToSimulator(const CSimConnectObject &simObject, DataDefinitionRemoteAircraftPartsWithoutLights &ddRemoteAircraftPartsWithoutLights, const CAircraftLights &lights)
        {
            Q_ASSERT(m_hSimConnect);
            const DWORD objectId = simObject.getObjectId();

            // same as in simulator or same as already send to simulator?
            const CAircraftLights sentLights(simObject.getLightsAsSent());
            if (simObject.getPartsAsSent() == ddRemoteAircraftPartsWithoutLights && sentLights == lights)
            {
                return true;
            }

            // in case we sent, we sent everything
            const HRESULT hr = SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftParts,
                               objectId, SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0,
                               sizeof(DataDefinitionRemoteAircraftPartsWithoutLights), &ddRemoteAircraftPartsWithoutLights);

            if (hr == S_OK && m_simConnectObjects.contains(simObject.getCallsign()))
            {
                if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO);}

                // Update data
                CSimConnectObject &objUdpate = m_simConnectObjects[simObject.getCallsign()];
                objUdpate.setPartsAsSent(ddRemoteAircraftPartsWithoutLights);
            }
            else
            {
                CLogMessage(this).warning("Failed so set parts on SimObject '%1' callsign: '%2'") << simObject.getObjectId() << simObject.getCallsign();
            }

            // lights we can set directly
            SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventLandingLightsSet, lights.isLandingOn() ? 1.0 : 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventStrobesSet, lights.isStrobeOn() ? 1.0 : 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

            // lights we need to toggle
            // (potential risk with quickly changing values that we accidentally toggle back, also we need the light state before we can toggle)
            this->sendToggledLightsToSimulator(simObject, lights);

            // done
            return hr == S_OK;
        }

        void CSimulatorFsxCommon::sendToggledLightsToSimulator(const CSimConnectObject &simObj, const CAircraftLights &lightsWanted, bool force)
        {
            if (!simObj.hasValidRequestAndObjectId()) { return; } // stale
            const CAircraftLights lightsIsState = simObj.getCurrentLightsInSimulator();
            if (lightsWanted == lightsIsState) { return; }
            if (!force && lightsWanted == simObj.getLightsAsSent()) { return; }
            const CCallsign callsign(simObj.getCallsign());

            // Update data
            if (m_simConnectObjects.contains(callsign))
            {
                CSimConnectObject &simObjToUpdate = m_simConnectObjects[callsign];
                simObjToUpdate.setLightsAsSent(lightsWanted);
            }

            // state available, then I can toggle
            if (!lightsIsState.isNull())
            {
                const DWORD objectId = simObj.getObjectId();
                if (lightsWanted.isTaxiOn() != lightsIsState.isTaxiOn())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleTaxiLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                if (lightsWanted.isNavOn() != lightsIsState.isNavOn())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleNavLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                if (lightsWanted.isBeaconOn() != lightsIsState.isBeaconOn())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleBeaconLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                if (lightsWanted.isLogoOn() != lightsIsState.isLogoOn())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleLogoLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                if (lightsWanted.isRecognitionOn() != lightsIsState.isRecognitionOn())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleRecognitionLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                if (lightsWanted.isCabinOn() != lightsIsState.isCabinOn())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleCabinLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                return;
            }

            // missing lights info from simulator so far
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Missing light state in simulator for '%1', model '%2'").arg(callsign.asString(), simObj.getAircraftModelString())); }

            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(DeferResendingLights, this, [ = ]
            {
                if (myself.isNull()) { return; }
                if (!m_simConnectObjects.contains(callsign)) { return; }
                const CSimConnectObject currentSimObject = m_simConnectObjects[callsign];
                if (!currentSimObject.hasValidRequestAndObjectId()) { return; } // stale
                if (lightsWanted != currentSimObject.getLightsAsSent())  { return; } // changed in between, so another call sendToggledLightsToSimulator is pending
                if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Resending light state for '%1', model '%2'").arg(callsign.asString(), simObj.getAircraftModelString())); }
                this->sendToggledLightsToSimulator(currentSimObject, lightsWanted, true);
            });
        }

        SIMCONNECT_DATA_INITPOSITION CSimulatorFsxCommon::aircraftSituationToFsxPosition(const CAircraftSituation &situation, bool sendGnd)
        {
            Q_ASSERT_X(!situation.isGeodeticHeightNull(), Q_FUNC_INFO, "Missing height (altitude)");
            Q_ASSERT_X(!situation.isPositionNull(), Q_FUNC_INFO,  "Missing position");

            SIMCONNECT_DATA_INITPOSITION position = CSimulatorFsxCommon::coordinateToFsxPosition(situation);
            position.Heading = situation.getHeading().value(CAngleUnit::deg());
            position.Airspeed = situation.getGroundSpeed().value(CSpeedUnit::kts());

            // MSFS has inverted pitch and bank angles
            position.Pitch = -situation.getPitch().value(CAngleUnit::deg());
            position.Bank  = -situation.getBank().value(CAngleUnit::deg());
            position.OnGround = 0U;

            if (sendGnd && situation.isOnGroundInfoAvailable())
            {
                const bool onGround = (situation.getOnGround() == CAircraftSituation::OnGround);
                position.OnGround = onGround ? 1U : 0U;
            }
            return position;
        }

        SIMCONNECT_DATA_INITPOSITION CSimulatorFsxCommon::coordinateToFsxPosition(const ICoordinateGeodetic &coordinate)
        {
            SIMCONNECT_DATA_INITPOSITION position;
            position.Latitude = coordinate.latitude().value(CAngleUnit::deg());
            position.Longitude = coordinate.longitude().value(CAngleUnit::deg());
            position.Altitude = coordinate.geodeticHeight().value(CLengthUnit::ft()); // already corrected in interpolator if there is an underflow
            position.Heading = 0;
            position.Airspeed = 0;
            position.Pitch = 0;
            position.Bank  = 0;
            position.OnGround = 0;
            return position;
        }

        void CSimulatorFsxCommon::synchronizeTime(const CTime &zuluTimeSim, const CTime &localTimeSim)
        {
            if (!m_simTimeSynced) { return; }
            if (!this->isConnected())   { return; }
            if (m_syncDeferredCounter > 0)
            {
                --m_syncDeferredCounter;
            }
            Q_UNUSED(localTimeSim);

            QDateTime myDateTime = QDateTime::currentDateTimeUtc();
            if (!m_syncTimeOffset.isZeroEpsilonConsidered())
            {
                int offsetSeconds = m_syncTimeOffset.valueRounded(CTimeUnit::s(), 0);
                myDateTime = myDateTime.addSecs(offsetSeconds);
            }
            const QTime myTime = myDateTime.time();
            const DWORD h = static_cast<DWORD>(myTime.hour());
            const DWORD m = static_cast<DWORD>(myTime.minute());
            const int targetMins = myTime.hour() * 60 + myTime.minute();
            const int simMins = zuluTimeSim.valueRounded(CTimeUnit::min());
            const int diffMins = qAbs(targetMins - simMins);
            if (diffMins < 2) { return; }
            HRESULT hr = S_OK;
            hr += SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTimeZuluHours, h, SIMCONNECT_GROUP_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            hr += SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTimeZuluMinutes, m, SIMCONNECT_GROUP_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

            if (hr != S_OK)
            {
                CLogMessage(this).warning("Sending time sync failed!");
            }
            else
            {
                m_syncDeferredCounter = 5; // allow some time to sync
                CLogMessage(this).info("Synchronized time to UTC: '%1'") << myTime.toString();
            }
        }

        void CSimulatorFsxCommon::injectWeatherGrid(const Weather::CWeatherGrid &weatherGrid)
        {
            // So far, there is only global weather
            auto glob = weatherGrid.frontOrDefault();
            glob.setIdentifier("GLOB");
            const QString metar = CSimConnectUtilities::convertToSimConnectMetar(glob);
            SimConnect_WeatherSetModeCustom(m_hSimConnect);
            SimConnect_WeatherSetModeGlobal(m_hSimConnect);
            SimConnect_WeatherSetObservation(m_hSimConnect, 0, qPrintable(metar));
        }

        bool CSimulatorFsxCommon::requestPositionDataForSimObject(const CSimConnectObject &simObject, SIMCONNECT_PERIOD period)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (simObject.isPendingRemoved()) { return false; }
            if (!m_hSimConnect) { return false; }
            if (simObject.getSimDataPeriod() == period) { return true; } // already queried like this

            // always request, not only when something has changed
            const HRESULT result = SimConnect_RequestDataOnSimObject(
                                       m_hSimConnect, simObject.getRequestId() + RequestSimDataOffset,
                                       CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                       simObject.getObjectId(), period);

            if (result == S_OK && m_simConnectObjects.contains(simObject.getCallsign()))
            {
                if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO);}
                m_simConnectObjects[simObject.getCallsign()].setSimDataPeriod(period);
                return true;
            }
            CLogMessage(this).error("Cannot request simulator data on object '%1'") << simObject.getObjectId();
            return false;
        }

        bool CSimulatorFsxCommon::requestTerrainProbeData(const CCallsign &callsign)
        {
            if (m_simConnectProbes.countConfirmedAdded() < 1) { return false; }
            const DWORD id = this->obtainRequestIdForProbe();
            const DWORD objectId = m_simConnectProbes.values().front().getObjectId();
            const HRESULT result = SimConnect_RequestDataOnSimObject(
                                       m_hSimConnect, id,
                                       CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                       objectId, SIMCONNECT_PERIOD_ONCE);
            if (m_traceSendId) { this->traceSendId(id, Q_FUNC_INFO); }

            if (result == S_OK)
            {
                m_pendingProbeRequests.insert(id, callsign);
                return true;
            }
            CLogMessage(this).error("Cannot request terrain probe data for id '%1' ''%2") << id << callsign.asString();
            return false;
        }

        bool CSimulatorFsxCommon::requestLightsForSimObject(const CSimConnectObject &simObject)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (simObject.isPendingRemoved()) { return false; }
            if (!m_hSimConnect) { return false; }

            // always request, not only when something has changed
            const HRESULT result = SimConnect_RequestDataOnSimObject(
                                       m_hSimConnect, simObject.getRequestId() + RequestLightsOffset,
                                       CSimConnectDefinitions::DataRemoteAircraftLights, simObject.getObjectId(),
                                       SIMCONNECT_PERIOD_SECOND);
            if (result == S_OK)
            {
                if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO);}
                return true;
            }
            CLogMessage(this).error("Cannot request lights data on object '%1'") << simObject.getObjectId();
            return false;
        }

        bool CSimulatorFsxCommon::stopRequestingDataForSimObject(const CSimConnectObject &simObject)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (!m_hSimConnect) { return false; }

            // always request, not only when something has changed
            HRESULT result = SimConnect_RequestDataOnSimObject(
                                 m_hSimConnect, simObject.getRequestId() + RequestSimDataOffset,
                                 CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                 simObject.getObjectId(), SIMCONNECT_PERIOD_NEVER);
            if (result == S_OK)  { if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO, "Position");} }

            result = SimConnect_RequestDataOnSimObject(
                         m_hSimConnect, simObject.getRequestId() + RequestLightsOffset,
                         CSimConnectDefinitions::DataRemoteAircraftLights, simObject.getObjectId(),
                         SIMCONNECT_PERIOD_NEVER);
            if (result == S_OK)  { if (m_traceSendId) { this->traceSendId(simObject.getObjectId(), Q_FUNC_INFO, "Lights");} }
            Q_UNUSED(result);
            return true;
        }

        void CSimulatorFsxCommon::initSimulatorInternals()
        {
            CSimulatorFsCommon::initSimulatorInternals();
            CSimulatorInternals s = m_simulatorInternals;
            const QString fsxPath = CFsCommonUtil::fsxDirFromRegistry(); // can be empty for remote FSX
            if (!fsxPath.isEmpty()) { s.setSimulatorInstallationDirectory(fsxPath); }

            s.setValue("fsx/simConnectCfgFilename", CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename());
            s.setValue("fsx/simConnectVersion", m_simConnectVersion);
            m_simulatorInternals = s;
        }

        void CSimulatorFsxCommon::reset()
        {
            this->safeKillTimer();
            m_simulatingChangedTs = -1;
            m_simConnected = false;
            m_simSimulating = false;
            m_syncDeferredCounter =  0;
            m_skipCockpitUpdateCycles = 0;
            m_interpolationRequest  = 0;
            m_requestIdSimData = static_cast<SIMCONNECT_DATA_REQUEST_ID>(RequestIdSimDataStart);
            m_dispatchErrors = 0;
            m_receiveExceptionCount = 0;
            m_sendIdTraces.clear();
            this->removeAllProbes();
            // cleared below:
            // m_simConnectObjects
            // m_simConnectObjectsPositionAndPartsTraces
            // m_addPendingAircraft
            CSimulatorFsCommon::reset(); // clears all pending aircraft etc
        }

        void CSimulatorFsxCommon::clearAllRemoteAircraftData()
        {
            m_simConnectObjects.clear();
            m_addPendingAircraft.clear();
            m_simConnectObjectsPositionAndPartsTraces.clear();
            this->removeAllProbes();
            // m_addAgainAircraftWhenRemoved cleared below
            CSimulatorFsCommon::clearAllRemoteAircraftData();
        }

        QString CSimulatorFsxCommon::fsxPositionToString(const SIMCONNECT_DATA_INITPOSITION &position)
        {
            static const QString positionStr("Lat: %1deg lng: %2deg alt: %3ft pitch: %4deg bank: %5deg hdg: %6deg airspeed: %7kts onGround: %8");
            return positionStr.
                   arg(position.Latitude).arg(position.Longitude).arg(position.Altitude).
                   arg(position.Pitch).arg(position.Bank).arg(position.Heading).arg(position.Airspeed).arg(position.OnGround);
        }

        CCallsignSet CSimulatorFsxCommon::getCallsignsMissingInProvider() const
        {
            const CCallsignSet simObjectCallsigns(m_simConnectObjects.keys());
            const CCallsignSet providerCallsigns(this->getAircraftInRangeCallsigns());
            return simObjectCallsigns.difference(providerCallsigns);
        }

        void CSimulatorFsxCommon::traceSendId(DWORD simObjectId, const QString &function, const QString &details)
        {
            if (!m_traceSendId) { return; }
            if (MaxSendIdTraces < 1) { return; }
            DWORD dwLastId = 0;
            const HRESULT hr = SimConnect_GetLastSentPacketID(m_hSimConnect, &dwLastId);
            if (hr != S_OK) { return; }
            if (m_sendIdTraces.size() > MaxSendIdTraces) { m_sendIdTraces.removeFirst(); }
            const TraceFsxSendId trace(dwLastId, simObjectId,
                                       details.isEmpty() ? function : details + ", " + function);
            m_sendIdTraces.push_back(trace);
        }

        QString CSimulatorFsxCommon::getSendIdTraceDetails(DWORD sendId) const
        {
            for (const TraceFsxSendId &trace : m_sendIdTraces)
            {
                if (trace.sendId == sendId)
                {
                    static const QString d("Send id: %1 obj.id.: %2 cs.: %4 '%3'");
                    const CCallsign cs = m_simConnectObjects.getCallsignForObjectId(trace.simObjectId);
                    return d.arg(sendId).arg(trace.simObjectId).arg(cs.asString(), trace.comment);
                }
            }
            return "";
        }

        int CSimulatorFsxCommon::removeAllProbes()
        {
            if (m_simConnectProbes.isEmpty()) { return 0; }
            int c = 0;
            for (const CSimConnectObject &simObject : m_simConnectProbes.values())
            {
                if (!simObject.isConfirmedAdded()) { continue; }
                const SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForProbe();
                const HRESULT result = SimConnect_AIRemoveObject(m_hSimConnect, static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), requestId);
                if (result == S_OK)
                {
                    c++;
                }
                else
                {
                    CLogMessage(this).warning("Removing probe '%1' from simulator failed") << simObject.getObjectId();
                }
            }
            m_simConnectProbes.clear();
            m_pendingProbeRequests.clear();
            return c;
        }

        CSimConnectObject CSimulatorFsxCommon::insertNewSimConnectObject(const CSimulatedAircraft &aircraft, DWORD requestId)
        {
            if (m_simConnectObjects.contains(aircraft.getCallsign()))
            {
                // error, ...?
                return m_simConnectObjects[aircraft.getCallsign()];
            }

            CSimConnectObject simObject;
            if (m_simConnectObjectsPositionAndPartsTraces.contains(aircraft.getCallsign()))
            {
                simObject = m_simConnectObjectsPositionAndPartsTraces[aircraft.getCallsign()];
                m_simConnectObjectsPositionAndPartsTraces.remove(aircraft.getCallsign());
                simObject.resetState();
                simObject.setRequestId(requestId);
                simObject.setAircraft(aircraft);
                simObject.attachInterpolatorLogger(&m_interpolationLogger); // setting a logger does not start logging
            }
            else
            {
                simObject = CSimConnectObject(aircraft, requestId, this, this, this->getRemoteAircraftProvider(), &m_interpolationLogger);
            }
            m_simConnectObjects.insert(aircraft.getCallsign(), simObject);
            return simObject;
        }

        QString CSimulatorFsxCommon::fsxCharToQString(const char *fsxChar, int size)
        {
            return QString::fromLatin1(fsxChar, size);
        }

        CCallsignSet CSimulatorFsxCommon::physicallyRemoveAircraftNotInProvider()
        {
            const CCallsignSet toBeRemoved(getCallsignsMissingInProvider());
            if (toBeRemoved.isEmpty()) { return toBeRemoved; }
            for (const CCallsign &callsign : toBeRemoved)
            {
                this->physicallyRemoveRemoteAircraft(callsign);
            }

            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Cs: '%1'").arg(toBeRemoved.toStringList().join(", "))); }
            return toBeRemoved;
        }

        CSimulatorFsxCommonListener::CSimulatorFsxCommonListener(const CSimulatorPluginInfo &info) :
            ISimulatorListener(info)
        {
            constexpr int QueryInterval = 5 * 1000; // 5 seconds
            m_timer.setInterval(QueryInterval);
            m_timer.setObjectName(this->objectName().append(":m_timer"));
            connect(&m_timer, &QTimer::timeout, this, &CSimulatorFsxCommonListener::checkConnection);
        }

        void CSimulatorFsxCommonListener::startImpl()
        {
            m_simulatorVersion.clear();
            m_simConnectVersion.clear();
            m_simulatorName.clear();
            m_simulatorDetails.clear();
            m_timer.start();
        }

        void CSimulatorFsxCommonListener::stopImpl()
        {
            m_timer.stop();
        }

        QString CSimulatorFsxCommonListener::backendInfo() const
        {
            if (m_simulatorName.isEmpty()) { return ISimulatorListener::backendInfo(); }
            return m_simulatorDetails;
        }

        void CSimulatorFsxCommonListener::checkConnection()
        {
            if (this->isShuttingDown()) { return; }
            Q_ASSERT_X(!CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "Expect to run in background");
            HANDLE hSimConnect;
            HRESULT result = SimConnect_Open(&hSimConnect, sApp->swiftVersionChar(), nullptr, 0, 0, 0);
            bool check = false;
            if (result == S_OK)
            {
                for (int i = 0; !check && i < 3 && !this->isShuttingDown(); i++)
                {
                    // result not always in first dispatch as we first have to obtain simulator name
                    result = SimConnect_CallDispatch(hSimConnect, CSimulatorFsxCommonListener::SimConnectProc, this);
                    if (result != S_OK) { break; } // means serious failure
                    check = this->checkVersionAndSimulator();
                    if (!check) { sApp->processEventsFor(500); }
                }
            }
            SimConnect_Close(hSimConnect);

            if (check)
            {
                emit this->simulatorStarted(this->getPluginInfo());
            }
        }

        bool CSimulatorFsxCommonListener::checkVersionAndSimulator() const
        {
            const CSimulatorInfo pluginSim(getPluginInfo().getIdentifier());
            const QString connectedSimName = m_simulatorName.toLower().trimmed();

            if (connectedSimName.isEmpty()) { return false; }
            if (pluginSim.p3d())
            {
                // P3D drivers only works with P3D
                return connectedSimName.contains("lockheed") || connectedSimName.contains("martin") || connectedSimName.contains("p3d") || connectedSimName.contains("prepar");
            }
            else if (pluginSim.fsx())
            {
                // FSX drivers only works with FSX
                return connectedSimName.contains("fsx") || connectedSimName.contains("microsoft") || connectedSimName.contains("simulator x");
            }
            return false;
        }

        bool CSimulatorFsxCommonListener::checkSimConnectDll() const
        {
            static const CWinDllUtils::DLLInfo simConnectInfo = CSimConnectUtilities::simConnectDllInfo();
            if (!simConnectInfo.errorMsg.isEmpty()) { return false; }
            return true;
        }

        void CSimulatorFsxCommonListener::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
        {
            Q_UNUSED(cbData);
            CSimulatorFsxCommonListener *simListener = static_cast<CSimulatorFsxCommonListener *>(pContext);
            switch (pData->dwID)
            {
            case SIMCONNECT_RECV_ID_OPEN:
                {
                    SIMCONNECT_RECV_OPEN *event = (SIMCONNECT_RECV_OPEN *)pData;
                    simListener->m_simulatorVersion = QString("%1.%2.%3.%4").arg(event->dwApplicationVersionMajor).arg(event->dwApplicationVersionMinor).arg(event->dwApplicationBuildMajor).arg(event->dwApplicationBuildMinor);
                    simListener->m_simConnectVersion = QString("%1.%2.%3.%4").arg(event->dwSimConnectVersionMajor).arg(event->dwSimConnectVersionMinor).arg(event->dwSimConnectBuildMajor).arg(event->dwSimConnectBuildMinor);
                    simListener->m_simulatorName = QString(event->szApplicationName);
                    simListener->m_simulatorDetails = QString("Name: '%1' Version: %2 SimConnect: %3").arg(simListener->m_simulatorName, simListener->m_simulatorVersion, simListener->m_simConnectVersion);
                    CLogMessage(static_cast<CSimulatorFsxCommonListener *>(nullptr)).info("Connect to %1: '%2'") << simListener->getPluginInfo().getIdentifier() << simListener->backendInfo();
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION:
                break;
            default:
                break;
            }
        }
    } // namespace
} // namespace
