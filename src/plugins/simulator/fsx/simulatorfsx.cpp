/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfsx.h"
#include "blackcore/application.h"
#include "blackmisc/interpolatorlinear.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/simulation/fscommon/bcdconversions.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/fsx/fsxsimulatorsetup.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"

#include <QTimer>
#include <type_traits>

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
    namespace Fsx
    {
        CSimulatorFsx::CSimulatorFsx(const CSimulatorPluginInfo &info,
                                     IOwnAircraftProvider *ownAircraftProvider,
                                     IRemoteAircraftProvider *remoteAircraftProvider,
                                     IWeatherGridProvider *weatherGridProvider,
                                     QObject *parent) :
            CSimulatorFsCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, parent)
        {
            Q_ASSERT_X(ownAircraftProvider, Q_FUNC_INFO, "Missing provider");
            Q_ASSERT_X(remoteAircraftProvider, Q_FUNC_INFO, "Missing provider");
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing global object");
            this->m_simulatorSetup = CFsxSimulatorSetup::getInitialSetup();

            m_useFsuipc = true; // Temporarily enabled until Simconnect Weather is implemented.
            this->m_interpolator = new CInterpolatorLinear(remoteAircraftProvider, this);
            m_modelMatcher.setDefaultModel(CAircraftModel(
                                               "Boeing 737-800 Paint1",
                                               CAircraftModel::TypeModelMatchingDefaultModel,
                                               "B737-800 default model",
                                               CAircraftIcaoCode("B738", "L2J")
                                           ));
        }

        CSimulatorFsx::~CSimulatorFsx()
        {
            disconnectFrom();
            // fsuipc is disconnected in CSimulatorFsCommon
        }

        bool CSimulatorFsx::isConnected() const
        {
            return m_simConnected;
        }

        bool CSimulatorFsx::isSimulating() const
        {
            return m_simSimulating;
        }

        bool CSimulatorFsx::connectTo()
        {
            if (this->isConnected()) { return true; }
            if (FAILED(SimConnect_Open(&m_hSimConnect, sApp->swiftVersionChar(), nullptr, 0, 0, 0)))
            {
                // reset state as expected for unconnected
                if (m_simconnectTimerId >= 0) { killTimer(m_simconnectTimerId); }
                m_simConnected = false;
                m_simSimulating = false;
                return false;
            }
            if (m_useFsuipc) { this->m_fsuipc->connect(); } // FSUIPC too

            // set structures and move on
            initEvents();
            initDataDefinitionsWhenConnected();
            m_simconnectTimerId = startTimer(10);
            reloadWeatherSettings();
            return true;
        }

        bool CSimulatorFsx::disconnectFrom()
        {
            if (!m_simConnected) { return true; }

            // stop mapper init
            //! \todo mapper shutdown in FSX, review keep it?
            // mapperInstance()->gracefulShutdown();

            if (m_simconnectTimerId)
            {
                killTimer(m_simconnectTimerId);
            }

            if (m_hSimConnect)
            {
                SimConnect_Close(m_hSimConnect);
                m_hSimConnect = nullptr;
            }

            m_simConnected = false;
            m_simSimulating = false;
            m_simconnectTimerId = -1;

            // emit status and disconnect FSUIPC
            CSimulatorFsCommon::disconnectFrom();
            return true;
        }

        bool CSimulatorFsx::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            CCallsign callsign(newRemoteAircraft.getCallsign());
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
            if (callsign.isEmpty()) { return false; }

            bool aircraftAlreadyExistsInSim = this->m_simConnectObjects.contains(callsign);
            if (aircraftAlreadyExistsInSim)
            {
                // remove first
                this->physicallyRemoveRemoteAircraft(callsign);
                CLogMessage(this).warning("Have to remove aircraft %1 before I can add it") << callsign;
            }

            CSimConnectObject simObj(callsign, m_nextObjID, 0, newRemoteAircraft.isVtol());
            ++m_nextObjID;

            // matched models
            CAircraftModel aircraftModel = getClosestMatch(newRemoteAircraft);
            Q_ASSERT_X(newRemoteAircraft.getCallsign() == aircraftModel.getCallsign(), Q_FUNC_INFO, "mismatching callsigns");

            this->updateAircraftModel(callsign, aircraftModel);
            CSimulatedAircraft aircraftAfterModelApplied(getAircraftInRangeForCallsign(newRemoteAircraft.getCallsign()));

            // create AI
            bool rendered = false;
            if (isConnected())
            {
                // initial position
                this->setInitialAircraftSituation(aircraftAfterModelApplied); // set interpolated data/parts if available

                SIMCONNECT_DATA_INITPOSITION initialPosition = aircraftSituationToFsxInitPosition(aircraftAfterModelApplied.getSituation());
                QByteArray m = aircraftModel.getModelString().toLocal8Bit();
                HRESULT hr = SimConnect_AICreateNonATCAircraft(m_hSimConnect, m.constData(), qPrintable(callsign.toQString().left(12)), initialPosition, static_cast<SIMCONNECT_DATA_REQUEST_ID>(simObj.getRequestId()));
                if (hr != S_OK) { CLogMessage(this).error("SimConnect, can not create AI traffic"); }
                m_simConnectObjects.insert(callsign, simObj);
                CLogMessage(this).info("FSX: Added aircraft %1") << callsign.toQString();
                rendered = true;
            }
            else
            {
                CLogMessage(this).warning("FSX: Not connected, not added aircraft %1") << callsign.toQString();
            }

            aircraftAfterModelApplied.setRendered(rendered);
            this->updateAircraftRendered(callsign, rendered);
            emit modelMatchingCompleted(aircraftAfterModelApplied);

            return rendered;
        }

        bool CSimulatorFsx::updateOwnSimulatorCockpit(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
        {
            if (originator == this->identifier()) { return false; }
            if (!this->isSimulating()) { return false; }

            // actually those data should be the same as ownAircraft
            CComSystem newCom1 = ownAircraft.getCom1System();
            CComSystem newCom2 = ownAircraft.getCom2System();
            CTransponder newTransponder = ownAircraft.getTransponder();

            bool changed = false;
            if (newCom1.getFrequencyActive() != this->m_simCom1.getFrequencyActive())
            {
                CFrequency newFreq = newCom1.getFrequencyActive();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom1Active,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;

            }
            if (newCom1.getFrequencyStandby() != this->m_simCom1.getFrequencyStandby())
            {
                CFrequency newFreq = newCom1.getFrequencyStandby();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom1Standby,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }

            if (newCom2.getFrequencyActive() != this->m_simCom2.getFrequencyActive())
            {
                CFrequency newFreq = newCom2.getFrequencyActive();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom2Active,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }
            if (newCom2.getFrequencyStandby() != this->m_simCom2.getFrequencyStandby())
            {
                CFrequency newFreq = newCom2.getFrequencyStandby();
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom2Standby,
                                               CBcdConversions::comFrequencyToBcdHz(newFreq), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }

            if (newTransponder.getTransponderCode() != this->m_simTransponder.getTransponderCode())
            {
                SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTransponderCode,
                                               CBcdConversions::transponderCodeToBcd(newTransponder), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                changed = true;
            }

            if (newTransponder.getTransponderMode() != this->m_simTransponder.getTransponderMode())
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

        void CSimulatorFsx::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            QByteArray m = message.getMessage().toLocal8Bit().constData();
            m.append('\0');

            SIMCONNECT_TEXT_TYPE type = SIMCONNECT_TEXT_TYPE_PRINT_BLACK;
            switch (message.getSeverity())
            {
            case CStatusMessage::SeverityDebug:
                return;
            case CStatusMessage::SeverityInfo:
                type = SIMCONNECT_TEXT_TYPE_PRINT_GREEN;
                break;
            case CStatusMessage::SeverityWarning:
                type = SIMCONNECT_TEXT_TYPE_PRINT_YELLOW;
                break;
            case CStatusMessage::SeverityError:
                type = SIMCONNECT_TEXT_TYPE_PRINT_RED;
                break;
            }
            HRESULT hr = SimConnect_Text(
                             m_hSimConnect, type, 7.5, EventTextMessage, static_cast<DWORD>(m.size()),
                             m.data()
                         );
            Q_UNUSED(hr);
        }

        void CSimulatorFsx::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        bool CSimulatorFsx::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            return this->m_simConnectObjects.contains(callsign);
        }

        CCallsignSet CSimulatorFsx::physicallyRenderedAircraft() const
        {
            return CCollection<CCallsign>(this->m_simConnectObjects.keys());
        }

        void CSimulatorFsx::setSimConnected()
        {
            m_simConnected = true;
            emitSimulatorCombinedStatus();
        }

        void CSimulatorFsx::onSimRunning()
        {
            if (m_simSimulating) { return; }
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

            emitSimulatorCombinedStatus();
        }

        void CSimulatorFsx::onSimStopped()
        {
            int oldStatus = getSimulatorStatus();
            m_simSimulating = false;
            emitSimulatorCombinedStatus(oldStatus);
        }

        void CSimulatorFsx::onSimFrame()
        {
            updateRemoteAircraft();
        }

        void CSimulatorFsx::onSimExit()
        {
            // reset complete state, we are going down
            disconnectFrom();
        }

        void CSimulatorFsx::updateOwnAircraftFromSimulator(DataDefinitionOwnAircraft simulatorOwnAircraft)
        {
            CSimulatedAircraft myAircraft(getOwnAircraft());
            BlackMisc::Geo::CCoordinateGeodetic position;
            position.setLatitude(CLatitude(simulatorOwnAircraft.latitude, CAngleUnit::deg()));
            position.setLongitude(CLongitude(simulatorOwnAircraft.longitude, CAngleUnit::deg()));

            BlackMisc::Aviation::CAircraftSituation aircraftSituation;
            aircraftSituation.setPosition(position);
            aircraftSituation.setPitch(CAngle(simulatorOwnAircraft.pitch, CAngleUnit::deg()));
            aircraftSituation.setBank(CAngle(simulatorOwnAircraft.bank, CAngleUnit::deg()));
            aircraftSituation.setHeading(CHeading(simulatorOwnAircraft.trueHeading, CHeading::True, CAngleUnit::deg()));
            aircraftSituation.setGroundSpeed(CSpeed(simulatorOwnAircraft.velocity, CSpeedUnit::kts()));
            aircraftSituation.setAltitude(CAltitude(simulatorOwnAircraft.altitude, CAltitude::MeanSeaLevel, CLengthUnit::ft()));

            CAircraftLights lights(simulatorOwnAircraft.lightStrobe,
                                   simulatorOwnAircraft.lightLanding,
                                   simulatorOwnAircraft.lightTaxi,
                                   simulatorOwnAircraft.lightBeacon,
                                   simulatorOwnAircraft.lightNav,
                                   simulatorOwnAircraft.lightLogo);

            QList<bool> helperList {simulatorOwnAircraft.engine1Combustion != 0, simulatorOwnAircraft.engine2Combustion != 0,
                                    simulatorOwnAircraft.engine3Combustion != 0, simulatorOwnAircraft.engine4Combustion != 0
                                   };

            CAircraftEngineList engines;
            for (int index = 0; index < simulatorOwnAircraft.numberOfEngines; ++index)
            {
                engines.push_back(CAircraftEngine(index + 1, helperList.at(index)));
            }

            CAircraftParts parts(lights, simulatorOwnAircraft.gearHandlePosition,
                                 simulatorOwnAircraft.flapsHandlePosition * 100,
                                 simulatorOwnAircraft.spoilersHandlePosition,
                                 engines,
                                 simulatorOwnAircraft.simOnGround);

            // set values
            updateOwnSituation(aircraftSituation);
            updateOwnParts(parts);

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
                bool changedCom1 = myAircraft.getCom1System() != com1;
                this->m_simCom1 = com1;

                com2.setFrequencyActive(CFrequency(simulatorOwnAircraft.com2ActiveMHz, CFrequencyUnit::MHz()));
                com2.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com2StandbyMHz, CFrequencyUnit::MHz()));
                bool changedCom2 = myAircraft.getCom2System() != com2;
                this->m_simCom2 = com2;

                transponder.setTransponderCode(simulatorOwnAircraft.transponderCode);
                bool changedXpr = (myAircraft.getTransponderCode() != transponder.getTransponderCode());

                if (changedCom1 || changedCom2 || changedXpr)
                {
                    this->updateCockpit(com1, com2, transponder, identifier());
                }
            }
            else
            {
                --m_skipCockpitUpdateCycles;
            }

            const auto currentPosition = CCoordinateGeodetic { aircraftSituation.latitude(), aircraftSituation.longitude(), {0} };
            if (CWeatherScenario::isRealWeatherScenario(m_weatherScenarioSettings.get()) &&
                    calculateGreatCircleDistance(m_lastWeatherPosition, currentPosition).value(CLengthUnit::mi()) > 20)
            {
                m_lastWeatherPosition = currentPosition;
                const auto weatherGrid = CWeatherGrid { { "GLOB", currentPosition } };
                requestWeatherGrid(weatherGrid, { this, &CSimulatorFsx::injectWeatherGrid });
            }
        }

        void CSimulatorFsx::updateOwnAircraftFromSimulator(DataDefinitionClientAreaSb sbDataArea)
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
            CSimulatedAircraft myAircraft(this->getOwnAircraft());
            bool changed = (myAircraft.getTransponderMode() != newMode);
            if (!changed) { return; }
            CTransponder xpdr = myAircraft.getTransponder();
            xpdr.setTransponderMode(newMode);
            this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), xpdr, this->identifier());
        }

        void CSimulatorFsx::setSimConnectObjectID(DWORD requestID, DWORD objectID)
        {
            // First check, if this request id belongs to us
            auto it = std::find_if(m_simConnectObjects.begin(), m_simConnectObjects.end(),
            [requestID](const CSimConnectObject & obj) { return obj.getRequestId() == static_cast<int>(requestID); });
            if (it == m_simConnectObjects.end()) { return; }

            // belongs to us
            it->setObjectId(static_cast<int>(objectID));
            SimConnect_AIReleaseControl(m_hSimConnect, objectID, requestID);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EventFreezeLat, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EventFreezeAlt, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EventFreezeAtt, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
        }

        void CSimulatorFsx::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);
            ps_dispatch();
        }

        void CSimulatorFsx::ps_dispatch()
        {
            HRESULT hr = SimConnect_CallDispatch(m_hSimConnect, SimConnectProc, this);
            if (hr != S_OK)
            {
                m_dispatchErrors++;
                if (m_dispatchErrors == 2)
                {
                    // 2nd time, an error / avoid multiple messages
                    // idea: if it happens once ignore
                    CLogMessage(this).error("FSX: Dispatch error");
                }
                else if (m_dispatchErrors > 5)
                {
                    // this normally happens during a FSX crash or shutdown
                    this->disconnectFrom();
                }
                return;
            }
            m_dispatchErrors = 0;
            if (m_useFsuipc && m_fsuipc)
            {
                CSimulatedAircraft fsuipcAircraft(getOwnAircraft());
                //! \todo split in high / low frequency reads
                bool ok = m_fsuipc->read(fsuipcAircraft, true, true, true);
                if (ok)
                {
                    // do whatever is required
                    Q_UNUSED(fsuipcAircraft);
                }
            }
        }

        bool CSimulatorFsx::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            // only remove from sim
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "wrong thred");
            if (!m_simConnectObjects.contains(callsign)) { return false; }
            return physicallyRemoveRemoteAircraft(m_simConnectObjects.value(callsign));
        }

        int CSimulatorFsx::physicallyRemoveAllRemoteAircraft()
        {
            if (m_simConnectObjects.isEmpty()) { return 0; }
            QList<CCallsign> callsigns(m_simConnectObjects.keys());
            int r = 0;
            for (const CCallsign &cs : callsigns)
            {
                if (physicallyRemoveRemoteAircraft(cs)) { r++; }
            }
            return r;
        }

        bool CSimulatorFsx::physicallyRemoveRemoteAircraft(const CSimConnectObject &simObject)
        {
            CCallsign callsign(simObject.getCallsign());
            m_simConnectObjects.remove(callsign);
            SimConnect_AIRemoveObject(m_hSimConnect, static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), static_cast<SIMCONNECT_DATA_REQUEST_ID>(simObject.getRequestId()));
            updateAircraftRendered(callsign, false);
            CLogMessage(this).info("FSX: Removed aircraft %1") << simObject.getCallsign().toQString();
            return true;
        }

        HRESULT CSimulatorFsx::initEvents()
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

            // Mapped events, see event ids here: http://msdn.microsoft.com/en-us/library/cc526980.aspx
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

            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventToggleTaxiLights, "TOGGLE_TAXI_LIGHTS");

            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin error: %1") << "SimConnect_MapClientEventToSimEvent failed";
                return hr;
            }

            // facility
            hr += SimConnect_SubscribeToFacilities(m_hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT, static_cast<SIMCONNECT_DATA_REQUEST_ID>(m_nextObjID++));
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin error: %1") << "SimConnect_SubscribeToFacilities failed";
                return hr;
            }
            return hr;
        }

        HRESULT CSimulatorFsx::initDataDefinitionsWhenConnected()
        {
            return CSimConnectDefinitions::initDataDefinitionsWhenConnected(m_hSimConnect);
        }

        HRESULT CSimulatorFsx::initWhenConnected()
        {
            // called when connected

            HRESULT hr = initEvents();
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin: initEvents failed");
                return hr;
            }

            // inti data definitions and SB data area
            hr += initDataDefinitionsWhenConnected();
            if (hr != S_OK)
            {
                CLogMessage(this).error("FSX plugin: initDataDefinitionsWhenConnected failed");
                return hr;
            }

            return hr;
        }

        void CSimulatorFsx::updateRemoteAircraft()
        {
            static_assert(sizeof(DataDefinitionRemoteAircraftParts) == 120, "DataDefinitionRemoteAircraftParts has an incorrect size.");
            Q_ASSERT_X(this->m_interpolator, Q_FUNC_INFO, "missing interpolator");
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "thread");

            // nothing to do, reset request id and exit
            if (this->isPaused() && this->m_pausedSimFreezesInterpolation) { return; } // no interpolation while paused
            int remoteAircraftNo = this->getAircraftInRangeCount();
            if (remoteAircraftNo < 1) { m_interpolationRequest = 0;  return; }

            // interpolate and send to SIM
            m_interpolationRequest++;

            // values used for position and parts
            bool isOnGround = false;
            qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
            CCallsignSet aircraftWithParts(this->remoteAircraftSupportingParts()); // optimization, fetch all parts supporting aircraft in one step (one lock)

            const QList<CSimConnectObject> simObjects(m_simConnectObjects.values());
            for (const CSimConnectObject &simObj : simObjects)
            {
                if (simObj.getObjectId() < 1) { continue; }

                const CCallsign callsign(simObj.getCallsign());
                Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");
                IInterpolator::InterpolationStatus interpolatorStatus;
                CAircraftSituation interpolatedSituation = this->m_interpolator->getInterpolatedSituation(callsign, currentTimestamp, simObj.isVtol(), interpolatorStatus);

                // having the onGround flag in parts forces me to obtain parts here
                // which is not the smartest thing regarding performance
                IInterpolator::PartsStatus partsStatus;
                partsStatus.supportsParts = aircraftWithParts.contains(callsign);
                CAircraftPartsList parts;
                if (partsStatus.supportsParts)
                {
                    this->m_interpolator->getPartsBeforeTime(callsign, currentTimestamp, partsStatus);
                }

                if (interpolatorStatus.allTrue())
                {
                    // update situation
                    SIMCONNECT_DATA_INITPOSITION position = aircraftSituationToFsxInitPosition(interpolatedSituation);

                    //! \todo The onGround in parts is nuts, as already mentioned in the discussion
                    // Currently ignored here, only guessing which is faster as aircraft without parts can just be
                    // a) I am forced to read parts even if i just want to update position
                    // b) Unlike the other values it is not a fire and forget value, as I need it again in the next cycle
                    if (partsStatus.supportsParts && !parts.isEmpty())
                    {
                        // we have parts, and use the closest ground
                        isOnGround = parts.front().isOnGround();
                    }
                    else
                    {
                        isOnGround = interpolatedSituation.isOnGroundGuessed();
                    }

                    position.OnGround = isOnGround ? 1U : 0U;
                    HRESULT hr = S_OK;
                    hr += SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPosition,
                                                        static_cast<SIMCONNECT_OBJECT_ID>(simObj.getObjectId()), 0, 0,
                                                        sizeof(SIMCONNECT_DATA_INITPOSITION), &position);
                    if (hr != S_OK) { CLogMessage(this).warning("Failed so set position on SimObject %1 callsign: %2") << simObj.getObjectId() << callsign; }

                } // interpolation data

                if (interpolatorStatus.interpolationSucceeded)
                {
                    // aircraft parts
                    // inside "interpolator if", as no parts can be sent without position
                    updateRemoteAircraftParts(simObj, parts, partsStatus, interpolatedSituation, isOnGround); // update and retrieve parts in the same step
                }

            } // all callsigns
            qint64 dt = QDateTime::currentMSecsSinceEpoch() - currentTimestamp;
            m_statsUpdateAircraftTimeTotal += dt;
            m_statsUpdateAircraftCount++;
            m_statsUpdateAircraftTimeAvg = m_statsUpdateAircraftTimeTotal / m_statsUpdateAircraftCount;
        }

        bool CSimulatorFsx::updateRemoteAircraftParts(const CSimConnectObject &simObj, const CAircraftPartsList &parts, IInterpolator::PartsStatus partsStatus, const CAircraftSituation &interpolatedSituation, bool isOnGround) const
        {
            // set parts
            DataDefinitionRemoteAircraftParts ddRemoteAircraftParts;
            if (partsStatus.supportsParts)
            {
                // parts is supported, but do we need to update?
                if (parts.isEmpty()) { return false; }

                // we have parts
                CAircraftParts newestParts = parts.front();
                ddRemoteAircraftParts.lightStrobe = newestParts.getLights().isStrobeOn() ? 1.0 : 0.0;
                ddRemoteAircraftParts.lightLanding = newestParts.getLights().isLandingOn() ? 1.0 : 0.0;
                // ddRemoteAircraftParts.lightTaxi = newestParts.getLights().isTaxiOn() ? 1.0 : 0.0;
                ddRemoteAircraftParts.lightBeacon = newestParts.getLights().isBeaconOn() ? 1.0 : 0.0;
                ddRemoteAircraftParts.lightNav = newestParts.getLights().isNavOn() ? 1.0 : 0.0;
                ddRemoteAircraftParts.lightLogo = newestParts.getLights().isLogoOn() ? 1.0 : 0.0;
                ddRemoteAircraftParts.flapsLeadingEdgeLeftPercent = newestParts.getFlapsPercent() / 100.0;
                ddRemoteAircraftParts.flapsLeadingEdgeRightPercent = newestParts.getFlapsPercent() / 100.0;
                ddRemoteAircraftParts.flapsTrailingEdgeLeftPercent = newestParts.getFlapsPercent() / 100.0;
                ddRemoteAircraftParts.flapsTrailingEdgeRightPercent = newestParts.getFlapsPercent() / 100.0;
                ddRemoteAircraftParts.spoilersHandlePosition = newestParts.isSpoilersOut() ? 1.0 : 0.0;
                ddRemoteAircraftParts.gearHandlePosition = newestParts.isGearDown() ? 1 : 0;
                ddRemoteAircraftParts.engine1Combustion = newestParts.isEngineOn(1) ? 1 : 0;
                ddRemoteAircraftParts.engine2Combustion = newestParts.isEngineOn(2) ? 1 : 0;;
                ddRemoteAircraftParts.engine3Combustion = newestParts.isEngineOn(3) ? 1 : 0;
                ddRemoteAircraftParts.engine4Combustion = newestParts.isEngineOn(4) ? 1 : 0;
            }
            else
            {
                // mode is guessing parts
                if (this->m_interpolationRequest % 20 != 0) { return false; } // only update every 20th cycle
                ddRemoteAircraftParts.gearHandlePosition = isOnGround ? 1 : 0;

                // when first detected moving, lights on
                if (isOnGround)
                {
                    // ddRemoteAircraftParts.lightTaxi = 1.0;
                    ddRemoteAircraftParts.lightBeacon = 1.0;
                    ddRemoteAircraftParts.lightNav = 1.0;

                    double gskmh = interpolatedSituation.getGroundSpeed().value(CSpeedUnit::km_h());
                    if (gskmh > 7.5)
                    {
                        // mode taxi
                        // ddRemoteAircraftParts.lightTaxi = 1.0;
                        ddRemoteAircraftParts.lightLanding = 0.0;
                    }
                    else if (gskmh > 25)
                    {
                        // mode accelaration for takeoff
                        // ddRemoteAircraftParts.lightTaxi = 0.0;
                        ddRemoteAircraftParts.lightLanding = 1.0;
                    }
                    else
                    {
                        // slow movements or parking
                        // ddRemoteAircraftParts.lightTaxi = 0.0;
                        ddRemoteAircraftParts.lightLanding = 0.0;
                    }
                }
                else
                {
                    // ddRemoteAircraftParts.lightTaxi = 0.0;
                    ddRemoteAircraftParts.lightBeacon = 1.0;
                    ddRemoteAircraftParts.lightNav = 1.0;
                    // landing lights for < 10000ft (normally MSL, here ignored)
                    ddRemoteAircraftParts.lightLanding = (interpolatedSituation.getAltitude().value(CLengthUnit::ft()) < 10000) ? 1.0 : 0;
                }
            }

            Q_ASSERT(m_hSimConnect);
            HRESULT hr = S_OK;
            hr += SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftParts,
                                                static_cast<SIMCONNECT_OBJECT_ID>(simObj.getObjectId()), 0, 0,
                                                sizeof(DataDefinitionRemoteAircraftParts), &ddRemoteAircraftParts);

            if (hr != S_OK) { CLogMessage(this).warning("Failed so set parts on SimObject %1 callsign: %2") << simObj.getObjectId() << simObj.getCallsign(); }
            return hr == S_OK;
        }

        SIMCONNECT_DATA_INITPOSITION CSimulatorFsx::aircraftSituationToFsxInitPosition(const CAircraftSituation &situation)
        {
            SIMCONNECT_DATA_INITPOSITION position;
            position.Latitude = situation.latitude().value(CAngleUnit::deg());
            position.Longitude = situation.longitude().value(CAngleUnit::deg());
            position.Altitude = situation.getAltitude().value(CLengthUnit::ft());
            position.Pitch = situation.getPitch().value(CAngleUnit::deg());
            position.Bank = situation.getBank().value(CAngleUnit::deg());
            position.Heading = situation.getHeading().value(CAngleUnit::deg());
            position.Airspeed = situation.getGroundSpeed().value(CSpeedUnit::kts());
            return position;
        }

        void CSimulatorFsx::synchronizeTime(const CTime &zuluTimeSim, const CTime &localTimeSim)
        {
            if (!this->m_simTimeSynced) { return; }
            if (!this->isConnected())   { return; }
            if (m_syncDeferredCounter > 0)
            {
                --m_syncDeferredCounter;
            }
            Q_UNUSED(localTimeSim);

            QDateTime myDateTime = QDateTime::currentDateTimeUtc();
            if (!this->m_syncTimeOffset.isZeroEpsilonConsidered())
            {
                int offsetSeconds = this->m_syncTimeOffset.valueRounded(CTimeUnit::s(), 0);
                myDateTime = myDateTime.addSecs(offsetSeconds);
            }
            QTime myTime = myDateTime.time();
            DWORD h = static_cast<DWORD>(myTime.hour());
            DWORD m = static_cast<DWORD>(myTime.minute());
            int targetMins = myTime.hour() * 60 + myTime.minute();
            int simMins = zuluTimeSim.valueRounded(CTimeUnit::min());
            int diffMins = qAbs(targetMins - simMins);
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
                CLogMessage(this).info("Synchronized time to UTC: %1") << myTime.toString();
            }
        }

        void CSimulatorFsx::injectWeatherGrid(const Weather::CWeatherGrid &weatherGrid)
        {
            m_fsuipc->write(weatherGrid);
        }

        void CSimulatorFsx::reloadWeatherSettings()
        {
            if (m_fsuipc->isConnected())
            {
                auto selectedWeatherScenario = m_weatherScenarioSettings.get();
                if (!CWeatherScenario::isRealWeatherScenario(selectedWeatherScenario))
                {
                    m_lastWeatherPosition = {};
                    injectWeatherGrid(CWeatherGrid::getByScenario(selectedWeatherScenario));
                }
            }
        }

        CSimulatorFsxListener::CSimulatorFsxListener(const CSimulatorPluginInfo &info) :
            ISimulatorListener(info),
            m_timer(new QTimer(this))
        {
            constexpr int QueryInterval = 5 * 1000; // 5 seconds
            m_timer->setInterval(QueryInterval);
            this->m_timer->setObjectName(this->objectName().append(":m_timer"));
            connect(m_timer, &QTimer::timeout, this, &CSimulatorFsxListener::ps_checkConnection);
        }

        void CSimulatorFsxListener::start()
        {
            m_timer->start();
        }

        void CSimulatorFsxListener::stop()
        {
            m_timer->stop();
        }

        void CSimulatorFsxListener::ps_checkConnection()
        {
            Q_ASSERT_X(!CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "Expect to run in background");
            HANDLE hSimConnect;
            HRESULT result = SimConnect_Open(&hSimConnect, sApp->swiftVersionChar(), nullptr, 0, 0, 0);
            SimConnect_Close(hSimConnect);
            if (result == S_OK)
            {
                emit simulatorStarted(this->getPluginInfo());
            }
        }

    } // namespace
} // namespace
