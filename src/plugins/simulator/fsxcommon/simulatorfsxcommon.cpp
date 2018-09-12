/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfsxcommon.h"
#include "simconnectsymbols.h"
#include "simconnectfunctions.h"
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
#include "blackmisc/math/mathutils.h"
#include "blackmisc/country.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include <QTimer>
#include <QPointer>
#include <QStringBuilder>
#include <type_traits>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Math;
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
            return m_simConnected && m_hSimConnect;
        }

        bool CSimulatorFsxCommon::isSimulating() const
        {
            return m_simSimulating && this->isConnected();
        }

        bool CSimulatorFsxCommon::connectTo()
        {
            if (this->isConnected()) { return true; }
            this->reset();

            if (!loadAndResolveSimConnect(true)) { return false; }

            const HRESULT hr = SimConnect_Open(&m_hSimConnect, sApp->swiftVersionChar(), nullptr, 0, nullptr, 0);
            if (isFailure(hr))
            {
                // reset state as expected for unconnected
                this->reset();
                return false;
            }
            if (m_useFsuipc) { m_fsuipc->connect(); } // FSUIPC too

            // set structures and move on
            this->triggerAutoTraceSendId(); // we trace the init phase, so in case something goes wrong there
            this->initEvents();
            this->initEventsP3D();
            this->initDataDefinitionsWhenConnected();

            m_timerId = this->startTimer(DispatchIntervalMs);
            // do not start m_addPendingAircraftTimer here, it will be started when object was added
            return true;
        }

        bool CSimulatorFsxCommon::disconnectFrom()
        {
            if (!m_simConnected) { return true; }
            m_simSimulating = false; // treat as stopped, just setting the flag here avoids overhead of on onSimStopped
            m_traceAutoUntilTs = -1;
            m_traceSendId = false;
            this->reset(); // mark as disconnected and reset all values

            if (m_hSimConnect)
            {
                SimConnect_Close(m_hSimConnect);
                m_hSimConnect = nullptr;
                m_simConnected = false;
            }

            // emit status and disconnect FSUIPC
            return CSimulatorFsCommon::disconnectFrom();
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
                    HRESULT hr = s_ok();

                    hr += SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbIdent, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 1, &ident);
                    hr += SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbStandby, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 1, &standby);
                    if (isFailure(hr))
                    {
                        this->triggerAutoTraceSendId();
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
            case CStatusMessage::SeverityInfo:    type = SIMCONNECT_TEXT_TYPE_PRINT_GREEN;  break;
            case CStatusMessage::SeverityWarning: type = SIMCONNECT_TEXT_TYPE_PRINT_YELLOW; break;
            case CStatusMessage::SeverityError:   type = SIMCONNECT_TEXT_TYPE_PRINT_RED;    break;
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
            callsigns.push_back(m_addPendingAircraft.keys()); // not really rendered, but for the logic it should look like it is
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

        QString CSimulatorFsxCommon::getStatisticsSimulatorSpecific() const
        {
            static const QString specificInfo("dispatch #: %1 %2 times (cur/max): %3ms (%4ms) %5ms (%6ms) %7 %8 simData#: %9");
            return specificInfo.
                   arg(m_dispatchProcCount).arg(m_dispatchProcEmptyCount).
                   arg(m_dispatchTimeMs).arg(m_dispatchProcTimeMs).arg(m_dispatchMaxTimeMs).arg(m_dispatchProcMaxTimeMs).
                   arg(CSimConnectUtilities::simConnectReceiveIdToString(static_cast<DWORD>(m_dispatchReceiveIdMaxTime)), requestIdToString(m_dispatchRequestIdMaxTime)).
                   arg(m_requestSimObjectDataCount);
        }

        bool CSimulatorFsxCommon::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &aircraftCallsign)
        {
            // this is the 32bit FSX version, the P3D x64 is overridden!

            if (this->isShuttingDownOrDisconnected()) { return false; }
            if (!this->isUsingFsxTerrainProbe()) { return false; }
            if (reference.isNull()) { return false; }
            const CSimConnectObject simObject = m_simConnectObjects.getNotPendingProbe();
            if (!simObject.isConfirmedAdded()) { return false; }

            CCoordinateGeodetic pos(reference);
            pos.setGeodeticHeight(terrainProbeAltitude());

            SIMCONNECT_DATA_INITPOSITION position = this->coordinateToFsxPosition(pos);
            const HRESULT hr = this->logAndTraceSendId(
                                   SimConnect_SetDataOnSimObject(
                                       m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                                       simObject.getObjectId(), 0, 0,
                                       sizeof(SIMCONNECT_DATA_INITPOSITION), &position),
                                   simObject, "Cannot request AI elevation", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject");

            if (isFailure(hr)) { return false; }

            const bool ok = this->requestTerrainProbeData(simObject, aircraftCallsign);
            if (ok)
            {
                emit this->requestedElevation(aircraftCallsign);
            }
            return ok;
        }

        bool CSimulatorFsxCommon::isTracingSendId() const
        {
            if (m_traceSendId) { return true; } // explicit
            if (m_traceAutoUntilTs < 0) { return false; } // no auto
            const qint64 ts = QDateTime::currentMSecsSinceEpoch();
            const bool trace = ts <= m_traceAutoUntilTs;
            return trace;
        }

        void CSimulatorFsxCommon::setTractingSendId(bool trace)
        {
            m_traceSendId = trace;
            m_traceAutoUntilTs = -1;
        }

        void CSimulatorFsxCommon::resetAircraftStatistics()
        {
            m_dispatchProcCount = 0;
            m_dispatchProcEmptyCount = 0;
            m_dispatchMaxTimeMs = -1;
            m_dispatchProcMaxTimeMs = -1;
            m_dispatchTimeMs = -1;
            m_dispatchProcTimeMs = -1;
            m_requestSimObjectDataCount = 0;
            m_dispatchReceiveIdLast = SIMCONNECT_RECV_ID_NULL;
            m_dispatchReceiveIdMaxTime = SIMCONNECT_RECV_ID_NULL;
            m_dispatchRequestIdLast = CSimConnectDefinitions::RequestEndMarker;
            m_dispatchRequestIdMaxTime = CSimConnectDefinitions::RequestEndMarker;
            CSimulatorPluginCommon::resetAircraftStatistics();
        }

        void CSimulatorFsxCommon::setFlightNetworkConnected(bool connected)
        {
            if (connected == !this->isFlightNetworkConnected())
            {
                // toggling, we trace for a while to better monitor those "critical" phases
                this->triggerAutoTraceSendId();
            }

            ISimulator::setFlightNetworkConnected(connected);
        }

        CSimConnectDefinitions::SimObjectRequest CSimulatorFsxCommon::requestToSimObjectRequest(DWORD requestId)
        {
            DWORD v = static_cast<DWORD>(CSimConnectDefinitions::SimObjectEndMarker);
            if (isRequestForSimObjAircraft(requestId))
            {
                v = (requestId - RequestSimObjAircraftStart) / MaxSimObjAircraft;
            }
            else if (isRequestForSimObjTerrainProbe(requestId))
            {
                v = (requestId - RequestSimObjTerrainProbeStart) / MaxSimObjProbes;
            }
            Q_ASSERT_X(v <= CSimConnectDefinitions::SimObjectEndMarker, Q_FUNC_INFO, "Invalid value");
            return static_cast<CSimConnectDefinitions::SimObjectRequest>(v);
        }

        bool CSimulatorFsxCommon::stillDisplayReceiveExceptions()
        {
            m_receiveExceptionCount++;
            return m_receiveExceptionCount < IgnoreReceiveExceptions;
        }

        CSimConnectObject CSimulatorFsxCommon::getSimObjectForObjectId(DWORD objectId) const
        {
            return this->getSimConnectObjects().getSimObjectForObjectId(objectId);
        }

        void CSimulatorFsxCommon::setSimConnected()
        {
            m_simConnected = true;
            this->initSimulatorInternals();
            this->emitSimulatorCombinedStatus();

            // Internals depends on simulator data which take a while to be read
            // this is a trick and I re-init again after a while (which is not really expensive)
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(2500, this, [myself]
            {
                if (!myself) { return; }
                myself->initSimulatorInternals();
            });
        }

        void CSimulatorFsxCommon::onSimRunning()
        {
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(DeferSimulatingFlagMs, this, [ = ]
            {
                if (!myself) { return; }
                m_simulatingChangedTs = QDateTime::currentMSecsSinceEpoch();
                this->onSimRunningDeferred(m_simulatingChangedTs);
            });
        }

        void CSimulatorFsxCommon::onSimRunningDeferred(qint64 referenceTs)
        {
            if (m_simSimulating) { return; }
            if (referenceTs != m_simulatingChangedTs) { return; } // changed, so no longer valid
            m_simSimulating = true; // only place where this should be set to true
            m_simConnected = true;
            HRESULT hr1 = this->logAndTraceSendId(
                              SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestOwnAircraft,
                                      CSimConnectDefinitions::DataOwnAircraft, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME),
                              "Cannot request own aircraft data", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            HRESULT hr2 = this->logAndTraceSendId(
                              SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestOwnAircraftTitle,
                                      CSimConnectDefinitions::DataOwnAircraftTitle,
                                      SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED),
                              "Cannot request title", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            HRESULT hr3 = this->logAndTraceSendId(
                              SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestSimEnvironment,
                                      CSimConnectDefinitions::DataSimEnvironment,
                                      SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED),
                              "Cannot request sim.env.", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");


            // Request the data from SB only when its changed and only ONCE so we don't have to run a 1sec event to get/set this info ;)
            HRESULT hr4 = this->logAndTraceSendId(
                              SimConnect_RequestClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::RequestSbData,
                                      CSimConnectDefinitions::DataClientAreaSb, SIMCONNECT_CLIENT_DATA_PERIOD_SECOND, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED),
                              "Cannot request client data", Q_FUNC_INFO, "SimConnect_RequestClientData");

            if (isFailure(hr1, hr2, hr3, hr4)) { return; }
            this->emitSimulatorCombinedStatus(); // force sending status
        }

        void CSimulatorFsxCommon::onSimStopped()
        {
            // stopping events in FSX: Load menu, weather and season
            CLogMessage(this).info("Simulator stopped: %1") << this->getSimulatorDetails();
            const SimulatorStatus oldStatus = this->getSimulatorStatus();
            m_simSimulating = false;
            m_simulatingChangedTs = QDateTime::currentMSecsSinceEpoch();
            this->emitSimulatorCombinedStatus(oldStatus);
        }

        void CSimulatorFsxCommon::onSimFrame()
        {
            if (m_updateRemoteAircraftInProgress) { return; }
            QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                // run decoupled from simconnect event queue
                if (!myself) { return; }
                myself->updateRemoteAircraft();
            });
        }

        void CSimulatorFsxCommon::onSimExit()
        {
            CLogMessage(this).info("Simulator exit: %1") << this->getSimulatorDetails();

            // reset complete state, we are going down
            m_simulatingChangedTs = QDateTime::currentMSecsSinceEpoch();
            this->safeKillTimer();

            // if called from dispatch function, avoid that SimConnectProc disconnects itself while in SimConnectProc
            QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->disconnectFrom();
            });
        }

        SIMCONNECT_DATA_REQUEST_ID CSimulatorFsxCommon::obtainRequestIdForSimObjAircraft()
        {
            const SIMCONNECT_DATA_REQUEST_ID id = m_requestIdSimObjAircraft++;
            if (id > RequestSimObjAircraftEnd) { m_requestIdSimObjAircraft = RequestSimObjAircraftStart; }
            return id;
        }

        SIMCONNECT_DATA_REQUEST_ID CSimulatorFsxCommon::obtainRequestIdForSimObjTerrainProbe()
        {
            const SIMCONNECT_DATA_REQUEST_ID id = m_requestIdSimObjTerrainProbe++;
            if (id > RequestSimObjTerrainProbeEnd) { m_requestIdSimObjTerrainProbe = RequestSimObjTerrainProbeStart; }
            return id;
        }

        bool CSimulatorFsxCommon::isValidSimObjectNotPendingRemoved(const CSimConnectObject &simObject) const
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (simObject.isPendingRemoved()) { return false; }
            if (!m_simConnectObjects.contains(simObject.getCallsign())) { return false; } // removed in meantime
            return true;
        }

        CSimConnectObject CSimulatorFsxCommon::getSimObjectForTrace(const TraceFsxSendId &trace) const
        {
            return m_simConnectObjects.getSimObjectForOtherSimObject(trace.simObject);
        }

        bool CSimulatorFsxCommon::removeSimObjectForTrace(const TraceFsxSendId &trace)
        {
            return m_simConnectObjects.removeByOtherSimObject(trace.simObject);
        }

        bool CSimulatorFsxCommon::triggerAutoTraceSendId(qint64 traceTimeMs)
        {
            if (m_traceSendId) { return false; } // no need
            if (this->isShuttingDownOrDisconnected()) { return false; }
            const qint64 ts = QDateTime::currentMSecsSinceEpoch();
            const qint64 traceUntil = traceTimeMs + ts;
            if (traceUntil <= m_traceAutoUntilTs) { return false; }
            m_traceAutoUntilTs = traceUntil;

            static const QString format("hh:mm:ss.zzz");
            const QString untilString = QDateTime::fromMSecsSinceEpoch(traceUntil).toString(format);
            CLogMessage(this).info("Triggered auto trace until %1") << untilString;
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(traceTimeMs * 1.2, this, [ = ]
            {
                // triggered by mself (ts check), otherwise ignore
                if (!myself) { return; }
                if (m_traceAutoUntilTs > QDateTime::currentMSecsSinceEpoch()) { return; }
                if (m_traceAutoUntilTs < 0) { return; } // alread off
                CLogMessage(this).info("Auto trace id off");
                m_traceAutoUntilTs = -1;
            });
            return true;
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
            aircraftSituation.setGroundElevation(CAltitude(simulatorOwnAircraft.elevation, CAltitude::MeanSeaLevel, CLengthUnit::ft()), CAircraftSituation::FromProvider);
            aircraftSituation.setAltitude(CAltitude(simulatorOwnAircraft.altitude, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
            aircraftSituation.setPressureAltitude(CAltitude(simulatorOwnAircraft.pressureAltitude, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
            // set on ground also in situation for consistency and future usage
            // it is duplicated in parts
            aircraftSituation.setOnGround(dtb(simulatorOwnAircraft.simOnGround) ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround, CAircraftSituation::OutOnGroundOwnAircraft);

            const CAircraftLights lights(dtb(simulatorOwnAircraft.lightStrobe), dtb(simulatorOwnAircraft.lightLanding), dtb(simulatorOwnAircraft.lightTaxi),
                                         dtb(simulatorOwnAircraft.lightBeacon), dtb(simulatorOwnAircraft.lightNav), dtb(simulatorOwnAircraft.lightLogo));

            CAircraftEngineList engines;
            const QList<bool> helperList
            {
                dtb(simulatorOwnAircraft.engine1Combustion), dtb(simulatorOwnAircraft.engine2Combustion),
                dtb(simulatorOwnAircraft.engine3Combustion), dtb(simulatorOwnAircraft.engine4Combustion)
            };

            for (int index = 0; index < simulatorOwnAircraft.numberOfEngines; ++index)
            {
                engines.push_back(CAircraftEngine(index + 1, helperList.at(index)));
            }

            const CAircraftParts parts(lights,
                                       dtb(simulatorOwnAircraft.gearHandlePosition),
                                       qRound(simulatorOwnAircraft.flapsHandlePosition * 100),
                                       dtb(simulatorOwnAircraft.spoilersHandlePosition),
                                       engines,
                                       dtb(simulatorOwnAircraft.simOnGround));

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

                transponder.setTransponderCode(qRound(simulatorOwnAircraft.transponderCode));
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

            // slower updates
            if (m_ownAircraftUpdate % 10 == 0)
            {
                if (m_isWeatherActivated)
                {
                    const auto currentPosition = CCoordinateGeodetic { aircraftSituation.latitude(), aircraftSituation.longitude() };
                    if (CWeatherScenario::isRealWeatherScenario(m_weatherScenarioSettings.get()))
                    {
                        if (m_lastWeatherPosition.isNull() ||
                                calculateGreatCircleDistance(m_lastWeatherPosition, currentPosition).value(CLengthUnit::mi()) > 20)
                        {
                            m_lastWeatherPosition = currentPosition;
                            const auto weatherGrid = CWeatherGrid { { "GLOB", currentPosition } };
                            requestWeatherGrid(weatherGrid, { this, &CSimulatorFsxCommon::injectWeatherGrid });
                        }
                    }
                }

                if (this->m_useFsxTerrainProbe && m_addedProbes < 1)
                {
                    this->physicallyInitAITerrainProbes(position, 2);
                }
            }

            m_ownAircraftUpdate++; // with 50updates/sec long enough even for 32bit
        }

        void CSimulatorFsxCommon::triggerUpdateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionPosData &remoteAircraftData)
        {
            if (this->isShuttingDownOrDisconnected()) { return; }
            QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                myself->updateRemoteAircraftFromSimulator(simObject, remoteAircraftData);
            });
        }

        void CSimulatorFsxCommon::triggerUpdateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionRemoteAircraftModel &remoteAircraftModel)
        {
            if (this->isShuttingDownOrDisconnected()) { return; }
            QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                myself->updateRemoteAircraftFromSimulator(simObject, remoteAircraftModel);
            });
        }

        void CSimulatorFsxCommon::updateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionPosData &remoteAircraftData)
        {
            if (this->isShuttingDownOrDisconnected()) { return; }

            // Near ground we use faster updates
            const CCallsign cs(simObject.getCallsign());
            const CAircraftSituation lastSituation = m_lastSentSituations[cs];
            const bool moving = lastSituation.isMoving();
            if (moving && remoteAircraftData.aboveGroundFt() <= 100.0)
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
            if (remoteAircraftData.aboveGroundFt() < 250)
            {
                const CElevationPlane elevation(remoteAircraftData.latitudeDeg, remoteAircraftData.longitudeDeg, remoteAircraftData.elevationFt, CElevationPlane::singlePointRadius());
                const CLength cg(remoteAircraftData.cgToGroundFt, CLengthUnit::ft());
                this->rememberElevationAndCG(cs, simObject.getAircraftModelString(), elevation, cg);
                this->addLoopbackSituation(cs, elevation, cg);
            }
        }

        void CSimulatorFsxCommon::updateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionRemoteAircraftModel &remoteAircraftModel)
        {
            const CCallsign cs(simObject.getCallsign());
            if (!m_simConnectObjects.contains(cs)) { return; } // no longer existing
            CSimConnectObject &so = m_simConnectObjects[cs];
            if (so.isPendingRemoved()) { return; }

            const QString modelString(remoteAircraftModel.title);
            const CLength cg(remoteAircraftModel.cgToGroundFt, CLengthUnit::ft());
            so.setAircraftCG(cg);
            so.setAircraftModelString(modelString);
            this->insertCG(cg, modelString, cs); // env. provider
            this->updateCGAndModelString(cs, cg, modelString); // remote aircraft provider
        }

        void CSimulatorFsxCommon::updateProbeFromSimulator(const CCallsign &callsign, const DataDefinitionPosData &remoteAircraftData)
        {
            const CElevationPlane elevation(remoteAircraftData.latitudeDeg, remoteAircraftData.longitudeDeg, remoteAircraftData.elevationFt, CElevationPlane::singlePointRadius());
            this->callbackReceivedRequestedElevation(elevation, callsign);
        }

        void CSimulatorFsxCommon::updateOwnAircraftFromSimulator(const DataDefinitionClientAreaSb &sbDataArea)
        {
            CTransponder::TransponderMode newMode = CTransponder::StateIdent;
            if (!sbDataArea.isIdent())
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
            if (this->isShuttingDownOrDisconnected()) { return true; } // pretend everything is fine
            const CSimConnectObject simObject = m_simConnectObjects.getSimObjectForObjectId(objectId);
            const CCallsign callsign(simObject.getCallsign());
            if (!simObject.hasValidRequestAndObjectId() || callsign.isEmpty()) { return false; }

            // we know the object has been created. But it can happen it is directly removed afterwards
            const CSimulatedAircraft verifyAircraft(simObject.getAircraft());
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(1000, this, [ = ]
            {
                // also triggers new add if required
                if (!myself) { return; }
                if (this->isShuttingDownOrDisconnected()) { return; }
                this->verifyAddedRemoteAircraft(verifyAircraft);
            });
            return true;
        }

        void CSimulatorFsxCommon::verifyAddedRemoteAircraft(const CSimulatedAircraft &remoteAircraftIn)
        {
            if (this->isShuttingDownOrDisconnected()) { return; }
            if (remoteAircraftIn.isTerrainProbe())
            {
                this->verifyAddedTerrainProbe(remoteAircraftIn);
                return;
            }

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
                        msg = CLogMessage(this).warning("Callsign '%1' removed in meantime from AI objects, but still in range") << callsign.toQString();
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
                const DWORD objectId = simObject.getObjectId();
                const SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectMisc);
                HRESULT hr = SimConnect_AIReleaseControl(m_hSimConnect, objectId, requestId);
                hr += SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeLat, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                hr += SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAlt, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                hr += SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAtt, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

                if (isFailure(hr))
                {
                    msg = CStatusMessage(this).error("Cannot confirm object %1, cs: '%2' model: '%3'") << objectId << remoteAircraft.getCallsignAsString() << remoteAircraft.getModelString();
                    break;
                }

                // request data on object
                this->requestPositionDataForSimObject(simObject);
                this->requestLightsForSimObject(simObject);
                this->requestModelInfoForSimObject(simObject);

                this->removeFromAddPendingAndAddAgainAircraft(callsign); // no longer try to add
                const bool updated = this->updateAircraftRendered(callsign, true);
                if (updated)
                {
                    emit this->aircraftRenderingChanged(simObject.getAircraft());
                    static const QString debugMsg("CS: '%1' model: '%2' verified, request/object id: %3 %4");
                    if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, debugMsg.arg(callsign.toQString(), remoteAircraft.getModelString()).arg(requestId).arg(objectId)); }
                }
                else
                {
                    CLogMessage(this).warning("Verified aircraft '%1' model '%2', request/object id: %3 %4 was already marked rendered") << callsign.asString() << remoteAircraft.getModelString() << requestId << objectId;
                }
            }
            while (false);

            // log errors and emit signal
            if (!msg.isEmpty())
            {
                CLogMessage::preformatted(msg);
                emit this->physicallyAddingRemoteModelFailed(CSimulatedAircraft(), false, msg);
            }

            // trigger new adding from pending if any
            if (!m_addPendingAircraft.isEmpty())
            {
                this->addPendingAircraftAfterAdded();
            }
        }

        void CSimulatorFsxCommon::addingAircraftFailed(const CSimConnectObject &simObject)
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild()) { Q_ASSERT_X(simObject.isAircraft(), Q_FUNC_INFO, "Need aircraft"); }
            if (!simObject.isAircraft()) { return; }

            CLogMessage(this).warning("Model failed to be added: '%1' details: %2") << simObject.getAircraftModelString() << simObject.getAircraft().toQString(true);
            CLogMessage::preformatted(simObject.getAircraftModel().verifyModelData());
            m_simConnectObjects.removeByOtherSimObject(simObject);

            if (simObject.getAddingExceptions() >= ThresholdAddException)
            {
                const CStatusMessage m = CLogMessage(this).warning("Model %1 failed %2 time(s) before and will be disabled") << simObject.toQString() << simObject.getAddingExceptions();
                this->updateAircraftEnabled(simObject.getCallsign(), false); // disable
                emit this->physicallyAddingRemoteModelFailed(simObject.getAircraft(), true, m);
            }
            else
            {
                CLogMessage(this).info("Will try '%1' again, aircraft: %2") << simObject.getAircraftModelString() << simObject.getAircraft().toQString(true);
                CSimConnectObject simObjAddAgain(simObject);
                simObjAddAgain.increaseAddingExceptions();

                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2000, this, [ = ]
                {
                    if (!myself) { return; }
                    if (this->isShuttingDownOrDisconnected()) { return; }
                    m_addPendingAircraft.insert(simObjAddAgain, true);
                });
            }
        }

        void CSimulatorFsxCommon::verifyAddedTerrainProbe(const CSimulatedAircraft &remoteAircraftIn)
        {
            CSimConnectObject &simObject = m_simConnectObjects[remoteAircraftIn.getCallsign()];
            simObject.setConfirmedAdded(true);

            // trigger new adding from pending if any
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
            const CCallsignSet aircraftCallsignsInRange(this->getAircraftInRangeCallsigns());
            CSimulatedAircraftList toBeAddedAircraft; // aircraft still to be added
            CCallsignSet toBeRemovedCallsigns;
            for (const CSimConnectObject &pendingSimObj : m_addPendingAircraft.values())
            {
                Q_ASSERT_X(!pendingSimObj.getCallsign().isEmpty(), Q_FUNC_INFO, "missing callsign");
                if (pendingSimObj.isTerrainProbe() || aircraftCallsignsInRange.contains(pendingSimObj.getCallsign()))
                {
                    toBeAddedAircraft.insert(pendingSimObj.getAircraft());
                }
                else
                {
                    toBeRemovedCallsigns.push_back(pendingSimObj.getCallsign());
                }
            }

            // no longer required to be added
            m_addPendingAircraft.removeCallsigns(toBeRemovedCallsigns);
            m_addAgainAircraftWhenRemoved.removeByCallsigns(toBeRemovedCallsigns);

            // add aircraft, but "non blocking"
            if (!toBeAddedAircraft.isEmpty())
            {
                const CSimConnectObject oldestSimObject = m_addPendingAircraft.getOldestObject();
                const CSimulatedAircraft nextPendingAircraft = oldestSimObject.getAircraft();
                if (nextPendingAircraft.hasModelString())
                {
                    const QPointer <CSimulatorFsxCommon> myself(this);
                    QTimer::singleShot(100, this, [ = ]
                    {
                        if (!myself) { return; }
                        if (this->isShuttingDownDisconnectedOrNoAircraft(nextPendingAircraft.isTerrainProbe())) { return; }
                        this->physicallyAddRemoteAircraftImpl(nextPendingAircraft, mode);
                    });
                }
                else
                {
                    CLogMessage(this).warning("Pending aircraft without model string will be removed");
                    m_addPendingAircraft.removeByOtherSimObject(oldestSimObject);
                }
            }
        }

        CSimConnectObject CSimulatorFsxCommon::removeFromAddPendingAndAddAgainAircraft(const CCallsign &callsign)
        {
            CSimConnectObject simObjectOld;
            if (callsign.isEmpty()) { return simObjectOld; }

            m_addAgainAircraftWhenRemoved.removeByCallsign(callsign);
            if (m_addPendingAircraft.contains(callsign))
            {
                simObjectOld = m_addPendingAircraft[callsign];
                m_addPendingAircraft.remove(callsign);
            }
            return simObjectOld;
        }

        bool CSimulatorFsxCommon::simulatorReportedObjectRemoved(DWORD objectID)
        {
            if (this->isShuttingDownOrDisconnected()) { return false; }
            CSimConnectObject simObject = m_simConnectObjects.getSimObjectForObjectId(objectID);
            if (!simObject.hasValidRequestAndObjectId()) { return false; } // object id from somewhere else

            const CCallsign callsign(simObject.getCallsign());
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign for removed object");

            if (simObject.isPendingRemoved())
            {
                // good case, object has been removed
                // we can remove the simulator object
            }
            else
            {
                // object was removed, but removal was not requested by us
                // this means we are out of the reality bubble or something else went wrong
                // Possible reasons:
                // 1) out of reality bubble, because we move to another airport or other reasons
                // 2) wrong position (in ground etc.)
                // 3) Simulator not running (ie in stopped mode)
                CStatusMessage msg;
                if (!simObject.getAircraftModelString().isEmpty() && simObject.getAddingDirectlyRemoved() < ThresholdAddedAndDirectlyRemoved)
                {
                    const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
                    simObject.increaseAddingDirectlyRemoved();
                    m_addPendingAircraft.insert(simObject, true); // insert and update ts
                    m_simConnectObjects.removeByOtherSimObject(simObject); // we have it in pending now, no need to keep it in this list

                    msg = CLogMessage(this).warning("Aircraft removed, '%1' '%2' object id '%3' out of reality bubble or other reason. Interpolator: '%4'")
                          << callsign.toQString() << simObject.getAircraftModelString()
                          << objectID << simObject.getInterpolatorInfo(setup.getInterpolatorMode());
                }
                else if (simObject.getAddingDirectlyRemoved() < ThresholdAddedAndDirectlyRemoved)
                {
                    const CStatusMessage m = CLogMessage(this).warning("Aircraft removed again multiple times and will be disabled, '%1' '%2' object id '%3'") << callsign.toQString() << simObject.getAircraftModelString() << objectID;
                    this->updateAircraftEnabled(simObject.getCallsign(), false);
                    emit this->physicallyAddingRemoteModelFailed(simObject.getAircraft(), true, m);
                }
                else
                {
                    msg = CLogMessage(this).warning("Removed '%1' from simulator, but was not initiated by us (swift): %1 '%2' object id %3") << callsign.toQString() << simObject.getAircraftModelString() << objectID;
                }

                // in all cases add verification details
                CLogMessage::preformatted(simObject.addingVerificationMessages());

                // relay messages
                if (!msg.isEmpty()) { emit this->driverMessages(msg); }
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
                    if (!myself) { return; }
                    if (this->isShuttingDownOrDisconnected()) { return; }
                    myself->physicallyAddRemoteAircraftImpl(aircraftAddAgain, AddedAfterRemoved);
                });
            }
            return removedAny;
        }

        bool CSimulatorFsxCommon::setSimConnectObjectId(DWORD requestId, DWORD objectId)
        {
            return m_simConnectObjects.setSimConnectObjectIdForRequestId(requestId, objectId);
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

        HRESULT CSimulatorFsxCommon::initEventsP3D()
        {
            return s_ok();
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

            // statistics
            m_dispatchReceiveIdLast = SIMCONNECT_RECV_ID_NULL;
            m_dispatchRequestIdLast = CSimConnectDefinitions::RequestEndMarker;
            const qint64 start = QDateTime::currentMSecsSinceEpoch();

            // process
            const HRESULT hr = SimConnect_CallDispatch(m_hSimConnect, m_dispatchProc, this);

            // statistics
            const qint64 end = QDateTime::currentMSecsSinceEpoch();
            m_dispatchTimeMs = end - start;
            if (m_dispatchMaxTimeMs < m_dispatchTimeMs)
            {
                m_dispatchMaxTimeMs = m_dispatchTimeMs;
                m_dispatchReceiveIdMaxTime = m_dispatchReceiveIdLast;
                m_dispatchRequestIdMaxTime = m_dispatchRequestIdLast;
            }

            // error handling
            if (isFailure(hr))
            {
                m_dispatchErrors++;
                this->triggerAutoTraceSendId();
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
            const bool probe = newRemoteAircraft.isTerrainProbe();

            // entry checks
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
            Q_ASSERT_X(newRemoteAircraft.hasModelString(), Q_FUNC_INFO, "missing model string");

            // reset timer
            m_addPendingSimObjTimer.start(AddPendingAircraftIntervalMs); // restart

            const CSimConnectObjects outdatedAdded = m_simConnectObjects.removeOutdatedPendingAdded(CSimConnectObject::AllTypes);
            if (!outdatedAdded.isEmpty())
            {
                const CCallsignSet callsigns = outdatedAdded.getAllCallsigns(false);
                CLogMessage(this).warning("Removed %1 outdated objects pending for added: %2") << outdatedAdded.size() << callsigns.getCallsignsAsString(true);
                this->updateMultipleAircraftEnabled(callsigns, false);

                static const QString msgText("%1 oudated adding, %2");
                for (const CSimConnectObject &simObjOutdated : outdatedAdded)
                {
                    const CStatusMessage msg = CStatusMessage(this).warning(msgText.arg(simObjOutdated.getCallsign().asString(), simObjOutdated.toQString()));
                    emit this->physicallyAddingRemoteModelFailed(simObjOutdated.getAircraft(), true, msg);
                }

                // if this aircraft is also outdated, ignore
                if (callsigns.contains(newRemoteAircraft.getCallsign())) { return false; }
            }

            const bool hasPendingAdded = m_simConnectObjects.containsPendingAdded();
            bool canAdd = this->isSimulating() && !hasPendingAdded;

            Q_ASSERT_X(!hasPendingAdded || m_simConnectObjects.countPendingAdded() < 2, Q_FUNC_INFO, "There must be only 0..1 pending objects");
            if (this->showDebugLogMessage())
            {
                this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1' mode: '%2' model: '%3'").arg(newRemoteAircraft.getCallsignAsString(), modeToString(addMode), newRemoteAircraft.getModelString()));
                this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1' pending callsigns: '%2', pending objects: '%3'").arg(newRemoteAircraft.getCallsignAsString(), m_addPendingAircraft.getAllCallsignStrings(true).join(", "), m_simConnectObjects.getPendingAddedCallsigns().getCallsignStrings().join(", ")));
            }

            // do we need to remove/add again because something has changed?
            // this handles changed model strings or an update of the model
            if (m_simConnectObjects.contains(callsign))
            {
                const CSimConnectObject simObject = m_simConnectObjects[callsign];
                const QString newModelString(newRemoteAircraft.getModelString());
                const QString simObjModelString(simObject.getAircraftModelString());
                const bool sameModel = (simObjModelString == newModelString); // compare on string only (other attributes might change such as mode)

                // same model, nothing will change, otherwise add again when removed
                if (sameModel)
                {
                    if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1' re-added same model '%2'").arg(newRemoteAircraft.getCallsignAsString(), newModelString)); }
                    return true;
                }

                this->physicallyRemoveRemoteAircraft(newRemoteAircraft.getCallsign());
                m_addAgainAircraftWhenRemoved.replaceOrAddByCallsign(newRemoteAircraft);
                if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1' re-added changed model '%2', will be added again").arg(newRemoteAircraft.getCallsignAsString(), newModelString)); }
                return false;
            }

            // situation check
            CAircraftSituation situation(newRemoteAircraft.getSituation());
            if (canAdd && situation.isPositionOrAltitudeNull())
            {
                // invalid position because position or altitude is null
                const CAircraftSituationList situations(this->remoteAircraftSituations(callsign));
                if (situations.isEmpty())
                {
                    CLogMessage(this).warning("No valid situations for '%1', will be added as pending") << callsign.asString();
                    canAdd = false;
                }
                else
                {
                    CLogMessage(this).warning("Invalid aircraft situation for new aircraft '%1', use closest situation") << callsign.asString();
                    situation = situations.findClosestTimeDistanceAdjusted(QDateTime::currentMSecsSinceEpoch());
                    Q_ASSERT_X(!situation.isPositionOrAltitudeNull(), Q_FUNC_INFO, "Invalid situation for new aircraft");
                }

                // still invalid?
                const bool invalidSituation = situation.isPositionOrAltitudeNull();
                canAdd = invalidSituation;
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    BLACK_VERIFY_X(invalidSituation, Q_FUNC_INFO, "Expect valid situation");
                    const CStatusMessage sm = CStatusMessage(this).warning("Invalid situation for '%1'") << callsign;
                    this->clampedLog(callsign, sm);
                }
            }

            // check if we can add, do not add if simulator is stopped or other objects pending
            if (!canAdd)
            {
                CSimConnectObject &addPendingObj = m_addPendingAircraft[newRemoteAircraft.getCallsign()];
                addPendingObj.setAircraft(newRemoteAircraft);
                return false;
            }

            // remove from pending and keep for later to remember fail counters
            const CSimConnectObject removedPendingObj = this->removeFromAddPendingAndAddAgainAircraft(callsign);

            // create AI after crosschecking it
            if (!probe && !this->isAircraftInRange(callsign))
            {
                CLogMessage(this).info("Skipping adding of '%1' since it is no longer in range") << callsign.asString();
                return false;
            }

            // setup
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign);
            const bool sendGround = setup.isSendingGndFlagToSimulator();

            // FSX/P3D adding
            Q_ASSERT_X(!probe || m_useFsxTerrainProbe, Q_FUNC_INFO, "Adding probe, but FSX probe mode is off");

            bool adding = false; // will be added flag
            const SIMCONNECT_DATA_REQUEST_ID requestId = probe ? this->obtainRequestIdForSimObjTerrainProbe() : this->obtainRequestIdForSimObjAircraft();
            const SIMCONNECT_DATA_INITPOSITION initialPosition = CSimulatorFsxCommon::aircraftSituationToFsxPosition(newRemoteAircraft.getSituation(), sendGround);
            const QString modelString(newRemoteAircraft.getModelString());
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1' model: '%2' request: %3, init pos: %4").arg(callsign.toQString(), modelString).arg(requestId).arg(fsxPositionToString(initialPosition))); }

            const HRESULT hr = !probe ?
                               SimConnect_AICreateNonATCAircraft(m_hSimConnect, qPrintable(modelString), qPrintable(callsign.toQString().left(12)), initialPosition, requestId) :
                               SimConnect_AICreateSimulatedObject(m_hSimConnect, qPrintable(modelString), initialPosition, requestId);
            if (isFailure(hr))
            {
                const CStatusMessage msg = CStatusMessage(this).error("SimConnect, can not create AI traffic: '%1' '%2'") << callsign.toQString() << modelString;
                CLogMessage::preformatted(msg);
                emit this->physicallyAddingRemoteModelFailed(newRemoteAircraft, true, msg);
            }
            else
            {
                // we will request a new aircraft by request ID, later we will receive its object id
                // so far this object id is 0 (DWORD)
                static const QString mode("mode: %1");
                const CSimConnectObject simObject = this->insertNewSimConnectObject(newRemoteAircraft, requestId, removedPendingObj);
                this->traceSendId(simObject, Q_FUNC_INFO, mode.arg(CSimulatorFsxCommon::modeToString(addMode)), true);
                adding = true;
            }
            return adding;
        }

        bool CSimulatorFsxCommon::physicallyAddAITerrainProbe(const ICoordinateGeodetic &coordinate, int number)
        {
            if (coordinate.isNull()) { return false; }
            if (!this->isUsingFsxTerrainProbe()) { return false; }
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");

            // static const QString modelString("OrcaWhale");
            // static const QString modelString("Water Drop");
            // static const QString modelString("A321ACA");
            static const QString modelString("AI_Tracker_Object_0");
            static const QString pseudoCallsign("PROBE%1"); // max 12 chars
            static const CCountry ctry("SW", "SWIFT");
            static const CAirlineIcaoCode swiftAirline("SWI", "swift probe", ctry, "SWIFT", false, false);
            static const CLivery swiftLivery(CLivery::getStandardCode(swiftAirline), swiftAirline, "swift probe");

            const CCallsign cs(pseudoCallsign.arg(number));
            const CAircraftModel model(modelString, CAircraftModel::TypeTerrainProbe, QStringLiteral("swift terrain probe"), CAircraftIcaoCode::unassignedIcao(), swiftLivery);
            CAircraftSituation situation(cs, coordinate);
            situation.setAltitude(terrainProbeAltitude());
            situation.setZeroPBH();
            const CSimulatedAircraft pseudoAircraft(cs, model, CUser("123456", "swift", cs), situation);
            return this->physicallyAddRemoteAircraftImpl(pseudoAircraft, ExternalCall);
        }

        int CSimulatorFsxCommon::physicallyInitAITerrainProbes(const ICoordinateGeodetic &coordinate, int number)
        {
            if (number < 1) { return 0; }
            int c = 0;
            for (int n = 1; n <= number; ++n)
            {
                if (this->physicallyAddAITerrainProbe(coordinate, n)) { c++; }
            }

            CLogMessage(this).info("Adding %1 FSX terrain probes") << number;
            m_addedProbes = c;
            return c;
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
            if (simObject.isTerrainProbe())
            {
                return false;
            }

            // check for pending objects
            m_addPendingAircraft.remove(callsign); // just in case still in list of pending aircraft
            const bool pendingAdded = simObject.isPendingAdded(); // already added in simulator, but not yet confirmed
            const bool stillWaitingForLights = !simObject.hasCurrentLightsInSimulator();
            if (!simObject.isRemovedWhileAdding() && (pendingAdded || stillWaitingForLights))
            {
                // problem: we try to delete an aircraft just requested to be added
                // best solution so far, call remove again with a delay
                CLogMessage(this).warning("'%1' requested to be removed, but pending added (%2) / or pending lights(%3). Object be removed again: %4")
                        << callsign.asString()
                        << boolToYesNo(pendingAdded) << boolToYesNo(stillWaitingForLights)
                        << simObject.toQString();
                simObject.setRemovedWhileAdding(true); // next time kill
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2000, this, [ = ]
                {
                    if (!myself) { return; }
                    myself->physicallyRemoveRemoteAircraft(callsign);
                });
                return false; // not yet deleted
            }

            // no more data from simulator
            this->stopRequestingDataForSimObject(simObject);

            // mark as removed
            simObject.setPendingRemoved(true);
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1' request/object id: %2/%3").arg(callsign.toQString()).arg(simObject.getRequestId()).arg(simObject.getObjectId())); }

            // call in SIM
            const SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectRemove);
            const HRESULT result = SimConnect_AIRemoveObject(m_hSimConnect, static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), requestId);
            if (isOk(result))
            {
                if (this->isTracingSendId()) { this->traceSendId(simObject, Q_FUNC_INFO);}
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
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(100, this, [ = ]
            {
                if (!myself) { return; }
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
            HRESULT hr = s_ok();
            // System events, see http://msdn.microsoft.com/en-us/library/cc526983.aspx#SimConnect_SubscribeToSystemEvent
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventSimStatus, "Sim");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventObjectAdded, "ObjectAdded");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventObjectRemoved, "ObjectRemoved");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventFrame, "Frame");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventPause, "Pause");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, SystemEventFlightLoaded, "FlightLoaded");
            if (isFailure(hr))
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

            if (isFailure(hr))
            {
                CLogMessage(this).error("FSX plugin error: %1") << "SimConnect_MapClientEventToSimEvent failed";
                return hr;
            }

            // facility
            SIMCONNECT_DATA_REQUEST_ID requestId = static_cast<SIMCONNECT_DATA_REQUEST_ID>(CSimConnectDefinitions::RequestFacility);
            hr += SimConnect_SubscribeToFacilities(m_hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT, requestId);
            if (isFailure(hr))
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
            if (isFailure(hr))
            {
                CLogMessage(this).error("FSX plugin: initEvents failed");
                return hr;
            }

            // init data definitions and SB data area
            hr += this->initDataDefinitionsWhenConnected();
            if (isFailure(hr))
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
            if (remoteAircraftNo < 1) { m_statsUpdateAircraftRuns = 0;  return; }

            // values used for position and parts
            const qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
            if (this->isUpdateAircraftLimitedWithStats(currentTimestamp)) { return; }
            m_updateRemoteAircraftInProgress = true;

            // interpolation for all remote aircraft
            const QList<CSimConnectObject> simObjects(m_simConnectObjects.values());

            int simObjectNumber = 0;
            const bool traceSendId = this->isTracingSendId();
            for (const CSimConnectObject &simObject : simObjects)
            {
                // happening if aircraft is not yet added to simulator or to be deleted
                if (!simObject.isReadyToSend()) { continue; }
                if (!simObject.hasCurrentLightsInSimulator()) { continue; } // wait until we have light state

                const CCallsign callsign(simObject.getCallsign());
                Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");
                Q_ASSERT_X(simObject.hasValidRequestAndObjectId(), Q_FUNC_INFO, "Missing ids");
                const DWORD objectId = simObject.getObjectId();

                // setup
                const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign);
                const bool sendGround = setup.isSendingGndFlagToSimulator();

                // Interpolated situation
                const CInterpolationResult result = simObject.getInterpolation(currentTimestamp, setup, simObjectNumber++);
                if (result.getInterpolationStatus().hasValidSituation())
                {
                    // update situation
                    if (!this->isEqualLastSent(result))
                    {
                        SIMCONNECT_DATA_INITPOSITION position = this->aircraftSituationToFsxPosition(result, sendGround);
                        const HRESULT hr = this->logAndTraceSendId(
                                               SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                                                       static_cast<SIMCONNECT_OBJECT_ID>(objectId), 0, 0, sizeof(SIMCONNECT_DATA_INITPOSITION), &position),
                                               traceSendId, simObject, "Failed so set position", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject");
                        if (isOk(hr))
                        {
                            this->rememberLastSent(result); // remember
                            this->removedClampedLog(callsign);
                        }
                    }
                }
                else
                {
                    static const QString so("SimObject id: %1");
                    const QString msg = this->getInvalidSituationLogMessage(callsign, result.getInterpolationStatus(), so.arg(objectId));
                    const CStatusMessage sm(this, CStatusMessage::SeverityWarning, msg);
                    this->clampedLog(callsign, sm);
                }

                // Interpolated parts
                this->updateRemoteAircraftParts(simObject, result);

            } // all callsigns

            // stats
            this->setStatsRemoteAircraftUpdate(currentTimestamp);
        }

        bool CSimulatorFsxCommon::updateRemoteAircraftParts(const CSimConnectObject &simObject, const CInterpolationResult &result)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }

            const CAircraftParts parts = result;
            if (parts.getPartsDetails() != CAircraftParts::GuessedParts && !result.getPartsStatus().isSupportingParts()) { return false; }
            if (result.getPartsStatus().isReusedParts() || this->isEqualLastSent(parts, simObject.getCallsign())) { return true; }

            DataDefinitionRemoteAircraftPartsWithoutLights ddRemoteAircraftPartsWithoutLights(parts); // no init, all values will be set
            return this->sendRemoteAircraftPartsToSimulator(simObject, ddRemoteAircraftPartsWithoutLights, parts.getAdjustedLights());
        }

        void CSimulatorFsxCommon::triggerUpdateAirports(const CAirportList &airports)
        {
            if (this->isShuttingDownOrDisconnected()) { return; }
            if (airports.isEmpty()) { return; }
            QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->updateAirports(airports);
            });
        }

        void CSimulatorFsxCommon::updateAirports(const CAirportList &airports)
        {
            if (airports.isEmpty()) { return; }

            static const CLength maxDistance(200.0, CLengthUnit::NM());
            const CCoordinateGeodetic posAircraft(this->getOwnAircraftPosition());

            for (const CAirport &airport : airports)
            {
                CAirport consolidatedAirport(airport);
                const CLength d = consolidatedAirport.calculcateAndUpdateRelativeDistanceAndBearing(posAircraft);
                if (d > maxDistance) { continue; }
                consolidatedAirport.updateMissingParts(this->getWebServiceAirport(airport.getIcao()));
                m_airportsInRangeFromSimulator.replaceOrAddByIcao(consolidatedAirport);
                if (m_airportsInRangeFromSimulator.size() > this->maxAirportsInRange())
                {
                    m_airportsInRangeFromSimulator.sortByDistanceToReferencePosition();
                    m_airportsInRangeFromSimulator.truncate(this->maxAirportsInRange());
                }
            }
        }

        bool CSimulatorFsxCommon::sendRemoteAircraftPartsToSimulator(const CSimConnectObject &simObject, DataDefinitionRemoteAircraftPartsWithoutLights &ddRemoteAircraftPartsWithoutLights, const CAircraftLights &lights)
        {
            Q_ASSERT(m_hSimConnect);

            if (!simObject.isReadyToSend()) { return false; }

            const DWORD objectId = simObject.getObjectId();
            const bool traceId = this->isTracingSendId();

            // in case we sent, we sent everything
            HRESULT hr1 = this->logAndTraceSendId(
                              SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftParts,
                                      objectId, SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0,
                                      sizeof(DataDefinitionRemoteAircraftPartsWithoutLights), &ddRemoteAircraftPartsWithoutLights),
                              traceId, simObject, "Failed so set parts", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject");

            // lights we can set directly
            HRESULT hr2 = this->logAndTraceSendId(
                              SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventLandingLightsSet, lights.isLandingOn() ? 1.0 : 0.0,
                                      SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                              traceId, simObject, "Failed so set landing lights", Q_FUNC_INFO, "SimConnect_TransmitClientEvent::EventLandingLightsSet");


            HRESULT hr3 = this->logAndTraceSendId(
                              SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventStrobesSet, lights.isStrobeOn() ? 1.0 : 0.0,
                                      SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                              traceId, simObject, "Failed to set strobe lights", Q_FUNC_INFO, "SimConnect_TransmitClientEvent::EventStrobesSet");

            // lights we need to toggle
            // (potential risk with quickly changing values that we accidentally toggle back, also we need the light state before we can toggle)
            this->sendToggledLightsToSimulator(simObject, lights);

            // done
            return isOk(hr1, hr2, hr3);
        }

        void CSimulatorFsxCommon::sendToggledLightsToSimulator(const CSimConnectObject &simObj, const CAircraftLights &lightsWanted, bool force)
        {
            if (!simObj.isReadyToSend()) { return; } // stale

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
                const bool trace = this->isTracingSendId();

                if (lightsWanted.isTaxiOn() != lightsIsState.isTaxiOn())
                {
                    this->logAndTraceSendId(
                        SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleTaxiLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                        trace, simObj, "Toggle taxi lights", Q_FUNC_INFO, "EventToggleTaxiLights");
                }
                if (lightsWanted.isNavOn() != lightsIsState.isNavOn())
                {
                    this->logAndTraceSendId(
                        SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleNavLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                        trace, simObj, "Toggle nav.lights", Q_FUNC_INFO, "EventToggleNavLights");
                }
                if (lightsWanted.isBeaconOn() != lightsIsState.isBeaconOn())
                {
                    this->logAndTraceSendId(
                        SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleBeaconLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                        trace, simObj, "Toggle becon lights", Q_FUNC_INFO, "EventToggleBeaconLights");
                }
                if (lightsWanted.isLogoOn() != lightsIsState.isLogoOn())
                {
                    this->logAndTraceSendId(
                        SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleLogoLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                        trace, simObj, "Toggle logo lights", Q_FUNC_INFO, "EventToggleLogoLights");
                }
                if (lightsWanted.isRecognitionOn() != lightsIsState.isRecognitionOn())
                {
                    this->logAndTraceSendId(
                        SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleRecognitionLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                        trace, simObj, "Toggle recognition lights", Q_FUNC_INFO, "EventToggleRecognitionLights");
                }
                if (lightsWanted.isCabinOn() != lightsIsState.isCabinOn())
                {
                    this->logAndTraceSendId(
                        SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventToggleCabinLights, 0.0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                        trace, simObj, "Toggle cabin lights", Q_FUNC_INFO, "EventToggleCabinLights");
                }
                return;
            }

            // missing lights info from simulator so far
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Missing light state in simulator for '%1', model '%2'").arg(callsign.asString(), simObj.getAircraftModelString())); }

            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(DeferResendingLights, this, [ = ]
            {
                if (!myself) { return; }
                if (!m_simConnectObjects.contains(callsign)) { return; }
                const CSimConnectObject currentSimObject = m_simConnectObjects[callsign];
                if (!currentSimObject.isReadyToSend()) { return; } // stale
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
            const double gsKts = situation.getGroundSpeed().value(CSpeedUnit::kts());
            position.Airspeed = static_cast<DWORD>(qRound(gsKts));

            // sanity check
            if (gsKts < 0.0)
            {
                // we get negative GS for pushback and helicopters
                // do do we handle them her with DWORD
                position.Airspeed = 0U;
            }

            // MSFS has inverted pitch and bank angles
            position.Pitch = -situation.getPitch().value(CAngleUnit::deg());
            position.Bank  = -situation.getBank().value(CAngleUnit::deg());
            position.OnGround = 0U; // not on ground

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
                int offsetSeconds = m_syncTimeOffset.valueInteger(CTimeUnit::s());
                myDateTime = myDateTime.addSecs(offsetSeconds);
            }
            const QTime myTime = myDateTime.time();
            const DWORD h = static_cast<DWORD>(myTime.hour());
            const DWORD m = static_cast<DWORD>(myTime.minute());
            const int targetMins = myTime.hour() * 60 + myTime.minute();
            const int simMins = zuluTimeSim.valueInteger(CTimeUnit::min());
            const int diffMins = qAbs(targetMins - simMins);
            if (diffMins < 2) { return; }
            const HRESULT hr1 = SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTimeZuluHours, h, SIMCONNECT_GROUP_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            const HRESULT hr2 = SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTimeZuluMinutes, m, SIMCONNECT_GROUP_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

            if (isFailure(hr1, hr2))
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
            if (this->isShuttingDownOrDisconnected()) { return false; }
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (simObject.isPending()) { return false; } // wait until confirmed
            if (simObject.getSimDataPeriod() == period) { return true; } // already queried like this
            if (!m_simConnectObjects.contains(simObject.getCallsign())) { return false; } // removed in meantime

            // always request, not only when something has changed
            const SIMCONNECT_DATA_REQUEST_ID reqId = static_cast<SIMCONNECT_DATA_REQUEST_ID>(simObject.getRequestId(CSimConnectDefinitions::SimObjectPositionData));
            const HRESULT result = this->logAndTraceSendId(
                                       SimConnect_RequestDataOnSimObject(
                                           m_hSimConnect, reqId,
                                           CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                           simObject.getObjectId(), period),
                                       simObject, "Cannot request simulator data", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            if (isOk(result))
            {
                m_requestSimObjectDataCount++;
                m_simConnectObjects[simObject.getCallsign()].setSimDataPeriod(period);
                return true;
            }
            return false;
        }

        bool CSimulatorFsxCommon::requestTerrainProbeData(const CSimConnectObject &simObject, const CCallsign &aircraftCallsign)
        {
            static const QString w("Cannot request terrain probe data for id '%1'");
            const SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectPositionData);
            const DWORD objectId = simObject.getObjectId();
            const HRESULT result = this->logAndTraceSendId(
                                       SimConnect_RequestDataOnSimObject(
                                           m_hSimConnect, requestId,
                                           CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                           objectId, SIMCONNECT_PERIOD_ONCE),
                                       simObject, w.arg(requestId), Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");
            const bool ok = isOk(result);
            if (ok) { m_pendingProbeRequests.insert(requestId, aircraftCallsign); }
            return ok;
        }

        bool CSimulatorFsxCommon::requestLightsForSimObject(const CSimConnectObject &simObject)
        {
            if (!this->isValidSimObjectNotPendingRemoved(simObject)) { return false; }
            if (!m_hSimConnect) { return false; }

            // always request, not only when something has changed
            const SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectLights);
            const HRESULT result = this->logAndTraceSendId(
                                       SimConnect_RequestDataOnSimObject(
                                           m_hSimConnect, requestId, CSimConnectDefinitions::DataRemoteAircraftLights,
                                           simObject.getObjectId(), SIMCONNECT_PERIOD_SECOND),
                                       simObject, "Cannot request lights data", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");
            return isOk(result);
        }

        bool CSimulatorFsxCommon::requestModelInfoForSimObject(const CSimConnectObject &simObject)
        {
            if (!this->isValidSimObjectNotPendingRemoved(simObject)) { return false; }
            if (!m_hSimConnect) { return false; }

            // always request, not only when something has changed
            const SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectModel);
            const HRESULT result = this->logAndTraceSendId(
                                       SimConnect_RequestDataOnSimObject(
                                           m_hSimConnect, requestId,
                                           CSimConnectDefinitions::DataRemoteAircraftModelData,
                                           simObject.getObjectId(), SIMCONNECT_PERIOD_ONCE),
                                       simObject, "Cannot request model info", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");
            return isOk(result);
        }

        bool CSimulatorFsxCommon::stopRequestingDataForSimObject(const CSimConnectObject &simObject)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (!m_hSimConnect) { return false; }

            // stop by setting SIMCONNECT_PERIOD_NEVER
            SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectPositionData);
            const HRESULT hr1 = this->logAndTraceSendId(
                                    SimConnect_RequestDataOnSimObject(
                                        m_hSimConnect, requestId,
                                        CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                        simObject.getObjectId(), SIMCONNECT_PERIOD_NEVER),
                                    simObject, "Stopping position request", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectLights);
            const HRESULT hr2 = this->logAndTraceSendId(
                                    SimConnect_RequestDataOnSimObject(
                                        m_hSimConnect, requestId,
                                        CSimConnectDefinitions::DataRemoteAircraftLights,
                                        simObject.getObjectId(), SIMCONNECT_PERIOD_NEVER),
                                    simObject, "Stopping lights request", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");
            return isOk(hr1, hr2);
        }

        void CSimulatorFsxCommon::initSimulatorInternals()
        {
            CSimulatorFsCommon::initSimulatorInternals();
            m_simulatorInternals.setValue("fsx/simConnectCfgFilename", CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename());
            m_simulatorInternals.setValue("fsx/simConnectVersion", m_simConnectVersion);
        }

        void CSimulatorFsxCommon::reset()
        {
            this->safeKillTimer();
            this->removeAllProbes(); // still requires connection

            // cleared below:
            // m_simConnectObjects
            // m_simConnectObjectsPositionAndPartsTraces
            // m_addPendingAircraft
            // m_updateRemoteAircraftInProgress
            CSimulatorFsCommon::reset(); // clears all pending aircraft etc

            // reset values
            m_simulatingChangedTs = -1;
            m_simConnected = false;
            m_simSimulating = false;
            m_syncDeferredCounter =  0;
            m_skipCockpitUpdateCycles = 0;
            m_ownAircraftUpdate = 0;
            m_requestIdSimObjAircraft = static_cast<SIMCONNECT_DATA_REQUEST_ID>(RequestSimObjAircraftStart);
            m_dispatchErrors = 0;
            m_receiveExceptionCount = 0;
            m_addedProbes = 0;
            m_sendIdTraces.clear();
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
            const CCallsignSet simObjectCallsigns(m_simConnectObjects.getAllCallsigns(true));
            const CCallsignSet providerCallsigns(this->getAircraftInRangeCallsigns());
            return simObjectCallsigns.difference(providerCallsigns);
        }

        void CSimulatorFsxCommon::traceSendId(const CSimConnectObject &simObject, const QString &functionName, const QString &details, bool forceTrace)
        {
            if (!forceTrace && !this->isTracingSendId()) { return; }
            if (MaxSendIdTraces < 1) { return; }
            DWORD dwLastId = 0;
            const HRESULT hr = SimConnect_GetLastSentPacketID(m_hSimConnect, &dwLastId);
            if (isFailure(hr)) { return; }
            if (m_sendIdTraces.size() > MaxSendIdTraces) { m_sendIdTraces.removeLast(); }
            const TraceFsxSendId trace(dwLastId, simObject, details.isEmpty() ? functionName : details % QStringLiteral(", ") % functionName);
            m_sendIdTraces.push_front(trace);
        }

        HRESULT CSimulatorFsxCommon::logAndTraceSendId(HRESULT hr, const QString &warningMsg, const QString &functionName, const QString &functionDetails)
        {
            const CSimConnectObject empty;
            return this->logAndTraceSendId(hr, empty, warningMsg, functionName, functionDetails);
        }

        HRESULT CSimulatorFsxCommon::logAndTraceSendId(HRESULT hr, const CSimConnectObject &simObject, const QString &warningMsg, const QString &functionName, const QString &functionDetails)
        {
            return this->logAndTraceSendId(hr, this->isTracingSendId(), simObject, warningMsg, functionName, functionDetails);
        }

        HRESULT CSimulatorFsxCommon::logAndTraceSendId(HRESULT hr, bool traceSendId, const CSimConnectObject &simObject, const QString &warningMsg, const QString &functionName, const QString &functionDetails)
        {
            if (traceSendId) { this->traceSendId(simObject, functionName, functionDetails); }
            if (isOk(hr)) { return hr; }
            if (!warningMsg.isEmpty())
            {
                CLogMessage(this).warning(warningMsg % QStringLiteral(" SimObject: ") % simObject.toQString());
            }
            this->triggerAutoTraceSendId();
            return hr;
        }

        TraceFsxSendId CSimulatorFsxCommon::getSendIdTrace(DWORD sendId) const
        {
            for (const TraceFsxSendId &trace : m_sendIdTraces)
            {
                if (trace.sendId == sendId) { return trace; }
            }
            return TraceFsxSendId::invalid();
        }

        QString CSimulatorFsxCommon::getSendIdTraceDetails(DWORD sendId) const
        {
            const TraceFsxSendId trace = this->getSendIdTrace(sendId);
            if (trace.sendId == sendId)
            {
                return this->getSendIdTraceDetails(trace);
            }
            return "";
        }

        QString CSimulatorFsxCommon::getSendIdTraceDetails(const TraceFsxSendId &trace) const
        {
            static const QString d("Send id: %1 obj.id.: %2 SimObj: %3 | '%4'");
            if (trace.isInvalid()) { return QString(); }

            // update with latest sim object
            const CSimConnectObject simObject = this->getSimObjectForTrace(trace);
            return d.arg(trace.sendId).arg(simObject.getObjectId()).arg(simObject.toQString(), trace.comment);
        }

        int CSimulatorFsxCommon::removeAllProbes()
        {
            if (!m_hSimConnect) { return 0; } // already disconnected
            const QList<CSimConnectObject> probes = m_simConnectObjects.getProbes();

            int c = 0;
            for (const CSimConnectObject &probeSimObject : probes)
            {
                if (!probeSimObject.isConfirmedAdded()) { continue; }
                const SIMCONNECT_DATA_REQUEST_ID requestId = probeSimObject.getRequestId(CSimConnectDefinitions::SimObjectRemove);
                const HRESULT result = SimConnect_AIRemoveObject(m_hSimConnect, static_cast<SIMCONNECT_OBJECT_ID>(probeSimObject.getObjectId()), requestId);
                if (isOk(result))
                {
                    c++;
                }
                else
                {
                    CLogMessage(this).warning("Removing probe '%1' from simulator failed") << probeSimObject.getObjectId();
                }
            }
            m_simConnectObjects.removeAllProbes();
            m_pendingProbeRequests.clear();
            return c;
        }

        CSimConnectObject CSimulatorFsxCommon::insertNewSimConnectObject(const CSimulatedAircraft &aircraft, DWORD requestId, const CSimConnectObject &removedPendingObject)
        {
            if (m_simConnectObjects.contains(aircraft.getCallsign()))
            {
                // error, ...?
                CSimConnectObject &simObject = m_simConnectObjects[aircraft.getCallsign()];
                simObject.copyAddingFailureCounters(removedPendingObject);
                simObject.resetTimestampToNow();
                return simObject;
            }

            CSimConnectObject simObject;
            if (m_simConnectObjectsPositionAndPartsTraces.contains(aircraft.getCallsign()))
            {
                // if in traces, get the object and reuse it
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
            simObject.copyAddingFailureCounters(removedPendingObject);
            m_simConnectObjects.insert(simObject, true); // update timestamp
            return simObject;
        }

        const CAltitude &CSimulatorFsxCommon::terrainProbeAltitude()
        {
            static const CAltitude alt(10000, CLengthUnit::ft());
            return alt;
        }

        QString CSimulatorFsxCommon::fsxCharToQString(const char *fsxChar, int size)
        {
            return QString::fromLatin1(fsxChar, size);
        }

        QString CSimulatorFsxCommon::requestIdToString(DWORD requestId)
        {
            if (requestId <= CSimConnectDefinitions::RequestEndMarker)
            {
                return CSimConnectDefinitions::requestToString(static_cast<CSimConnectDefinitions::Request>(requestId));
            }

            const CSimConnectDefinitions::SimObjectRequest simRequest = requestToSimObjectRequest(requestId);
            const CSimConnectObject::SimObjectType simType =  CSimConnectObject::requestIdToType(requestId);

            static const QString req("%1 %2 %3");
            return req.arg(requestId).arg(CSimConnectObject::typeToString(simType)).arg(CSimConnectDefinitions::simObjectRequestToString(simRequest));
        }

        DWORD CSimulatorFsxCommon::unitTestRequestId(CSimConnectObject::SimObjectType type)
        {
            int start;
            int end;
            switch (type)
            {
            case CSimConnectObject::TerrainProbe:
                start = RequestSimObjTerrainProbeStart; end = RequestSimObjTerrainProbeEnd;
                break;
            case CSimConnectObject::Aircraft:
            default:
                start = RequestSimObjAircraftStart; end = RequestSimObjAircraftEnd;
                break;
            }

            const int id = CMathUtils::randomInteger(start, end);
            return static_cast<DWORD>(id);
        }

        CCallsignSet CSimulatorFsxCommon::physicallyRemoveAircraftNotInProvider()
        {
            const CCallsignSet callsignsToBeRemoved(this->getCallsignsMissingInProvider());
            if (callsignsToBeRemoved.isEmpty()) { return callsignsToBeRemoved; }
            for (const CCallsign &callsign : callsignsToBeRemoved)
            {
                this->physicallyRemoveRemoteAircraft(callsign);
            }

            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("CS: '%1'").arg(callsignsToBeRemoved.toStringList().join(", "))); }
            return callsignsToBeRemoved;
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

            if (!loadAndResolveSimConnect(true)) { return; }

            m_timer.start();
        }

        void CSimulatorFsxCommonListener::stopImpl()
        {
            m_timer.stop();
        }

        void CSimulatorFsxCommonListener::checkImpl()
        {
            if (!m_timer.isActive()) { return; }
            if (this->isShuttingDown()) { return; }

            m_timer.start(); // restart because we will check just now
            QPointer<CSimulatorFsxCommonListener> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->checkConnection();
            });
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
            HRESULT result = SimConnect_Open(&hSimConnect, sApp->swiftVersionChar(), nullptr, 0, nullptr, 0);
            bool check = false;
            if (isOk(result))
            {
                for (int i = 0; !check && i < 3 && !this->isShuttingDown(); i++)
                {
                    // result not always in first dispatch as we first have to obtain simulator name
                    result = SimConnect_CallDispatch(hSimConnect, CSimulatorFsxCommonListener::SimConnectProc, this);
                    if (isFailure(result)) { break; } // means serious failure
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
            if (pluginSim.isP3D())
            {
                // P3D drivers only works with P3D
                return connectedSimName.contains("lockheed") || connectedSimName.contains("martin") || connectedSimName.contains("p3d") || connectedSimName.contains("prepar");
            }
            else if (pluginSim.isFSX())
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
                    SIMCONNECT_RECV_OPEN *event = static_cast<SIMCONNECT_RECV_OPEN *>(pData);
                    simListener->m_simulatorVersion = QString("%1.%2.%3.%4").arg(event->dwApplicationVersionMajor).arg(event->dwApplicationVersionMinor).arg(event->dwApplicationBuildMajor).arg(event->dwApplicationBuildMinor);
                    simListener->m_simConnectVersion = QString("%1.%2.%3.%4").arg(event->dwSimConnectVersionMajor).arg(event->dwSimConnectVersionMinor).arg(event->dwSimConnectBuildMajor).arg(event->dwSimConnectBuildMinor);
                    simListener->m_simulatorName = CSimulatorFsxCommon::fsxCharToQString(event->szApplicationName);
                    simListener->m_simulatorDetails = QString("Name: '%1' Version: %2 SimConnect: %3").arg(simListener->m_simulatorName, simListener->m_simulatorVersion, simListener->m_simConnectVersion);
                    CLogMessage(static_cast<CSimulatorFsxCommonListener *>(nullptr)).info("Connect to %1: '%2'") << simListener->getPluginInfo().getIdentifier() << simListener->backendInfo();
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION: break;
            default: break;
            }
        }
    } // namespace
} // namespace
