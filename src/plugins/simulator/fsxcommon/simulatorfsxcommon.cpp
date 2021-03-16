/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorfsxcommon.h"
#include "simconnectsymbols.h"
#include "../fscommon/simulatorfscommonfunctions.h"
#include "blackcore/application.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/fscommon/bcdconversions.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
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
#include <QElapsedTimer>

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
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackSimPlugin::FsCommon;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        CSimulatorFsxCommon::CSimulatorFsxCommon(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider    *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider    *weatherGridProvider,
                IClientProvider         *clientProvider,
                QObject *parent) :
            CSimulatorFsCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            Q_ASSERT_X(ownAircraftProvider, Q_FUNC_INFO, "Missing provider");
            Q_ASSERT_X(remoteAircraftProvider, Q_FUNC_INFO, "Missing provider");
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing global object");

            m_simObjectTimer.setInterval(AddPendingAircraftIntervalMs);
            m_useFsuipc = false;
            // default model will be set in derived class

            CSimulatorFsxCommon::registerHelp();
            connect(&m_simObjectTimer, &QTimer::timeout, this, &CSimulatorFsxCommon::timerBasedObjectAddOrRemove);
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

            const HRESULT hr = SimConnect_Open(&m_hSimConnect, sApp->swiftVersionChar(), nullptr, 0, nullptr, 0);
            if (isFailure(hr))
            {
                // reset state as expected for unconnected
                this->reset();
                return false;
            }

            // FSUIPC too
            if (m_useFsuipc)
            {
                m_fsuipc->open();
            }

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
            m_simSimulating    = false; // thread as stopped, just setting the flag here avoids overhead of on onSimStopped
            m_traceAutoUntilTs = -1;
            m_traceSendId      = false;
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
            this->logAddingAircraftModel(newRemoteAircraft);
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
                // use one way to transfer XPDR ident/mode not both
                if (m_useSbOffsets)
                {
                    byte ident = newTransponder.isIdentifying() ? 1U : 0U; // 1 is ident
                    byte standby = newTransponder.isInStandby() ? 1U : 0U; // 1 is standby
                    HRESULT hr = s_ok();

                    hr += SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbIdent, SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, 1, &ident);
                    hr += SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbStandby, SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, 1, &standby);
                    if (isFailure(hr))
                    {
                        this->triggerAutoTraceSendId();
                        CLogMessage(this).warning(u"Setting transponder mode failed (SB offsets)");
                    }
                    else
                    {
                        if (m_logSbOffsets)
                        {
                            const QString lm = "SB sent: ident " % QString::number(ident) % u" standby " % QString::number(standby);
                            CLogMessage(this).info(lm);
                        }
                    }
                    changed = true;
                }
                else if (m_useFsuipc && m_fsuipc)
                {
                    m_fsuipc->write(newTransponder);
                    changed = true;
                }
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

            //! KB 2018-11 that would need to go to updateOwnAircraftFromSimulator if the simulator ever supports SELCAL
            //! KB 2018-11 als we would need to send the value to FS9/FSX (currently we only deal with it on FS9/FSX level)
            m_selcal = selcal;
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
            Q_UNUSED(hr)
        }

        void CSimulatorFsxCommon::displayTextMessage(const CTextMessage &message) const
        {
            QByteArray m = message.asString(true, true).toLatin1().constData();
            m.append('\0');

            SIMCONNECT_TEXT_TYPE type = SIMCONNECT_TEXT_TYPE_PRINT_BLACK;
            if (message.isSupervisorMessage())   { type = SIMCONNECT_TEXT_TYPE_PRINT_RED; }
            else if (message.isPrivateMessage()) { type = SIMCONNECT_TEXT_TYPE_PRINT_YELLOW; }
            else if (message.isRadioMessage())   { type = SIMCONNECT_TEXT_TYPE_PRINT_GREEN; }

            const HRESULT hr = SimConnect_Text(m_hSimConnect, type, 7.5, EventTextMessage, static_cast<DWORD>(m.size()), m.data());
            Q_UNUSED(hr)
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
            if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return msgs; }
            msgs = CSimulatorFsCommon::debugVerifyStateAfterAllAircraftRemoved();
            if (!m_simConnectObjects.isEmpty()) { msgs.push_back(CStatusMessage(this).error(u"m_simConnectObjects not empty: '%1'") << m_simConnectObjects.getAllCallsignStringsAsString(true)); }
            if (!m_simConnectObjectsPositionAndPartsTraces.isEmpty()) { msgs.push_back(CStatusMessage(this).error(u"m_simConnectObjectsPositionAndPartsTraces not empty: '%1'") << m_simConnectObjectsPositionAndPartsTraces.getAllCallsignStringsAsString(true)); }
            if (!m_addAgainAircraftWhenRemoved.isEmpty()) { msgs.push_back(CStatusMessage(this).error(u"m_addAgainAircraftWhenRemoved not empty: '%1'") << m_addAgainAircraftWhenRemoved.getCallsignStrings(true).join(", ")); }
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
            const CSimConnectObject simObject = m_simConnectObjects.getOldestNotPendingProbe(); // probes round robin
            if (!simObject.isConfirmedAdded()) { return false; }
            m_simConnectObjects[simObject.getCallsign()].resetTimestampToNow(); // mark probe as just used

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

        void CSimulatorFsxCommon::setAddingAsSimulatedObjectEnabled(bool enabled)
        {
            m_useAddSimulatedObj = enabled;
            const CSimulatorInfo sim = this->getSimulatorInfo();
            CFsxP3DSettings settings = m_detailsSettings.getSettings(sim);
            settings.setAddingAsSimulatedObjectEnabled(enabled);
            m_detailsSettings.setSettings(settings, sim);
        }

        void CSimulatorFsxCommon::setUsingSbOffsetValues(bool enabled)
        {
            m_useSbOffsets = enabled;
            const CSimulatorInfo sim = this->getSimulatorInfo();
            CFsxP3DSettings settings = m_detailsSettings.getSettings(sim);
            settings.setSbOffsetsEnabled(enabled);
            m_detailsSettings.setSettings(settings, sim);
        }

        void CSimulatorFsxCommon::resetAircraftStatistics()
        {
            m_dispatchProcCount      =  0;
            m_dispatchProcEmptyCount =  0;
            m_dispatchMaxTimeMs      = -1;
            m_dispatchProcMaxTimeMs  = -1;
            m_dispatchTimeMs         = -1;
            m_dispatchProcTimeMs     = -1;
            m_requestSimObjectDataCount = 0;
            m_dispatchReceiveIdLast    = SIMCONNECT_RECV_ID_NULL;
            m_dispatchReceiveIdMaxTime = SIMCONNECT_RECV_ID_NULL;
            m_dispatchRequestIdLast    = CSimConnectDefinitions::RequestEndMarker;
            m_dispatchRequestIdMaxTime = CSimConnectDefinitions::RequestEndMarker;
            CSimulatorPluginCommon::resetAircraftStatistics();
        }

        void CSimulatorFsxCommon::setFlightNetworkConnected(bool connected)
        {
            // toggled?
            if (connected == !this->isFlightNetworkConnected())
            {
                // toggling, we trace for a while to better monitor those "critical" phases
                this->triggerAutoTraceSendId();
            }

            // update SB area network connected
            byte sbNetworkConnected = connected ? 1u : 0u;
            const HRESULT hr = SimConnect_SetClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbConnected, SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, 1, &sbNetworkConnected);
            if (isFailure(hr))
            {
                CLogMessage(this).warning(u"Setting network connected failed (SB offsets)");
            }

            ISimulator::setFlightNetworkConnected(connected);
        }

        CStatusMessageList CSimulatorFsxCommon::getInterpolationMessages(const CCallsign &callsign) const
        {
            if (!m_simConnectObjects.contains(callsign)) { return CStatusMessageList(); }
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, false);
            return (m_simConnectObjects[callsign]).getInterpolationMessages(setup.getInterpolatorMode());
        }

        bool CSimulatorFsxCommon::testSendSituationAndParts(const CCallsign &callsign, const CAircraftSituation &situation, const CAircraftParts &parts)
        {
            if (!m_simConnectObjects.contains(callsign)) { return false; }
            CSimConnectObject simObject = m_simConnectObjects.value(callsign);
            int u = 0;
            if (!parts.isNull()) { this->sendRemoteAircraftPartsToSimulator(simObject, parts); u++; }
            if (!situation.isNull())
            {
                SIMCONNECT_DATA_INITPOSITION position = this->aircraftSituationToFsxPosition(situation, true);
                const bool traceSendId = this->isTracingSendId();
                const HRESULT hr = this->logAndTraceSendId(
                                       SimConnect_SetDataOnSimObject(
                                           m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                                           static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), 0, 0, sizeof(SIMCONNECT_DATA_INITPOSITION), &position),
                                       traceSendId, simObject, "Failed to set position", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject");
                if (hr == S_OK) { u++; }
            }
            return u > 0;
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
            if (m_simSimulating) { return; } // already simulatig
            if (referenceTs != m_simulatingChangedTs) { return; } // changed, so no longer valid
            m_simSimulating = true; // only place where this should be set to true
            m_simConnected  = true;

            const CFsxP3DSettings settings = m_detailsSettings.getSettings(this->getSimulatorInfo());
            m_useAddSimulatedObj = settings.isAddingAsSimulatedObjectEnabled();
            m_useSbOffsets = settings.isSbOffsetsEnabled();

            const HRESULT hr1 = this->logAndTraceSendId(
                                    SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestOwnAircraft,
                                            CSimConnectDefinitions::DataOwnAircraft, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME),
                                    "Cannot request own aircraft data", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            const HRESULT hr2 = this->logAndTraceSendId(
                                    SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestOwnAircraftTitle,
                                            CSimConnectDefinitions::DataOwnAircraftTitle,
                                            SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED),
                                    "Cannot request title", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            const HRESULT hr3 = this->logAndTraceSendId(
                                    SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::RequestSimEnvironment,
                                            CSimConnectDefinitions::DataSimEnvironment,
                                            SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED),
                                    "Cannot request sim.env.", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            // Request the data from SB only when its changed and only ONCE so we don't have to run a 1sec event to get/set this info ;)
            // there was a bug with SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, see https://www.prepar3d.com/forum/viewtopic.php?t=124789
            const HRESULT hr4 = this->logAndTraceSendId(
                                    SimConnect_RequestClientData(m_hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::RequestSbData,
                                            CSimConnectDefinitions::DataClientAreaSb, SIMCONNECT_CLIENT_DATA_PERIOD_SECOND, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED),
                                    "Cannot request client data", Q_FUNC_INFO, "SimConnect_RequestClientData");

            if (isFailure(hr1, hr2, hr3, hr4)) { return; }
            this->emitSimulatorCombinedStatus(); // force sending status
        }

        void CSimulatorFsxCommon::onSimStopped()
        {
            // stopping events in FSX: Load menu, weather and season
            CLogMessage(this).info(u"Simulator stopped: %1") << this->getSimulatorDetails();
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
            CLogMessage(this).info(u"Simulator exit: %1") << this->getSimulatorDetails();

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

        bool CSimulatorFsxCommon::releaseAIControl(const CSimConnectObject &simObject, SIMCONNECT_DATA_REQUEST_ID requestId)
        {
            const SIMCONNECT_OBJECT_ID objectId = simObject.getObjectId();
            const HRESULT hr1 = this->logAndTraceSendId(
                                    SimConnect_AIReleaseControl(m_hSimConnect, objectId, requestId),
                                    simObject, "Release control", Q_FUNC_INFO, "SimConnect_AIReleaseControl");
            const HRESULT hr2 = this->logAndTraceSendId(
                                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeLatLng, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                                    simObject, "EventFreezeLatLng", Q_FUNC_INFO, "SimConnect_TransmitClientEvent");
            const HRESULT hr3 = this->logAndTraceSendId(
                                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAlt, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                                    simObject, "EventFreezeAlt", Q_FUNC_INFO, "SimConnect_TransmitClientEvent");
            const HRESULT hr4 = this->logAndTraceSendId(
                                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFreezeAtt, 1, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                                    simObject, "EventFreezeAtt", Q_FUNC_INFO, "SimConnect_TransmitClientEvent");

            return isOk(hr1, hr2, hr3, hr4);
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

        void CSimulatorFsxCommon::removeCamera(CSimConnectObject &simObject)
        {
            // not in FSX
            Q_UNUSED(simObject)
        }

        void CSimulatorFsxCommon::removeObserver(CSimConnectObject &simObject)
        {
            // not in FSX
            Q_UNUSED(simObject)
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
            CLogMessage(this).info(u"Triggered FSX/P3D auto trace until %1") << untilString;
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(traceTimeMs * 1.2, this, [ = ]
            {
                // triggered by mself (ts check), otherwise ignore
                if (!myself) { return; }
                if (m_traceAutoUntilTs > QDateTime::currentMSecsSinceEpoch()) { return; }
                if (m_traceAutoUntilTs < 0) { return; } // alread off
                CLogMessage(this).info(u"Auto trace id off");
                m_traceAutoUntilTs = -1;
            });
            return true;
        }

        void CSimulatorFsxCommon::updateOwnAircraftFromSimulator(const DataDefinitionOwnAircraft &simulatorOwnAircraft)
        {
            const qint64 ts = QDateTime::currentMSecsSinceEpoch();

            CSimulatedAircraft myAircraft(getOwnAircraft());
            CCoordinateGeodetic position;
            position.setLatitude(CLatitude(simulatorOwnAircraft.latitudeDeg, CAngleUnit::deg()));
            position.setLongitude(CLongitude(simulatorOwnAircraft.longitudeDeg, CAngleUnit::deg()));

            if (simulatorOwnAircraft.pitchDeg < -90.0 || simulatorOwnAircraft.pitchDeg >= 90.0)
            {
                CLogMessage(this).warning(u"FSX: Pitch value (own aircraft) out of limits: %1") << simulatorOwnAircraft.pitchDeg;
            }
            CAircraftSituation aircraftSituation;
            aircraftSituation.setMSecsSinceEpoch(ts);
            aircraftSituation.setPosition(position);
            // MSFS has inverted pitch and bank angles
            aircraftSituation.setPitch(CAngle(-simulatorOwnAircraft.pitchDeg, CAngleUnit::deg()));
            aircraftSituation.setBank(CAngle(-simulatorOwnAircraft.bankDeg, CAngleUnit::deg()));
            aircraftSituation.setHeading(CHeading(simulatorOwnAircraft.trueHeadingDeg, CHeading::True, CAngleUnit::deg()));
            aircraftSituation.setGroundSpeed(CSpeed(simulatorOwnAircraft.velocity, CSpeedUnit::kts()));
            aircraftSituation.setGroundElevation(CAltitude(simulatorOwnAircraft.elevationFt, CAltitude::MeanSeaLevel, CLengthUnit::ft()), CAircraftSituation::FromProvider);
            aircraftSituation.setAltitude(CAltitude(simulatorOwnAircraft.altitudeFt, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
            aircraftSituation.setPressureAltitude(CAltitude(simulatorOwnAircraft.pressureAltitudeM, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m()));
            // set on ground also in situation for consistency and future usage
            // it is duplicated in parts
            aircraftSituation.setOnGround(dtb(simulatorOwnAircraft.simOnGround) ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround, CAircraftSituation::OutOnGroundOwnAircraft);

            const CAircraftLights lights(dtb(simulatorOwnAircraft.lightStrobe), dtb(simulatorOwnAircraft.lightLanding), dtb(simulatorOwnAircraft.lightTaxi),
                                         dtb(simulatorOwnAircraft.lightBeacon), dtb(simulatorOwnAircraft.lightNav),     dtb(simulatorOwnAircraft.lightLogo));

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
                                       dtb(simulatorOwnAircraft.simOnGround),
                                       ts);

            // set values
            this->updateOwnSituationAndGroundElevation(aircraftSituation);
            this->updateOwnParts(parts);

            // When I change cockpit values in the sim (from GUI to simulator, not originating from simulator)
            // it takes a little while before these values are set in the simulator.
            // To avoid jitters, I wait some update cylces to stabilize the values
            if (m_skipCockpitUpdateCycles < 1)
            {
                // defaults
                CComSystem com1(myAircraft.getCom1System()); // set defaults
                CComSystem com2(myAircraft.getCom2System());

                // updates: https://www.fsdeveloper.com/forum/threads/com-unit-receiving-status-com-transmit-x-com-test-1-and-volume.445187/
                // COM: If you're set to transmit on a unit, you WILL receive that unit.
                // Otherwise if you're NOT set to transmit on a unit, then it will only receive if COM RECEIVE ALL is true.
                // There is no control of COM volume.
                com1.setFrequencyActive(CFrequency(simulatorOwnAircraft.com1ActiveMHz, CFrequencyUnit::MHz()));
                com1.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com1StandbyMHz, CFrequencyUnit::MHz()));
                const bool comReceiveAll = dtb(simulatorOwnAircraft.comReceiveAll);
                const bool com1Test      = dtb(simulatorOwnAircraft.comTest1);
                const bool com1Transmit  = dtb(simulatorOwnAircraft.comTransmit1);
                const int  com1Status    = qRound(simulatorOwnAircraft.comStatus1); // Radio status flag : -1 =Invalid 0 = OK 1 = Does not exist 2 = No electricity 3 = Failed
                com1.setTransmitEnabled(com1Status == 0 && com1Transmit);
                com1.setReceiveEnabled(com1Status == 0 && (comReceiveAll || com1Transmit));
                const bool changedCom1 = myAircraft.getCom1System() != com1;
                m_simCom1 = com1;
                Q_UNUSED(com1Test)

                com2.setFrequencyActive(CFrequency(simulatorOwnAircraft.com2ActiveMHz, CFrequencyUnit::MHz()));
                com2.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com2StandbyMHz, CFrequencyUnit::MHz()));
                const bool com2Test     = dtb(simulatorOwnAircraft.comTest2);
                const bool com2Transmit = dtb(simulatorOwnAircraft.comTransmit2);
                const int  com2Status   = qRound(simulatorOwnAircraft.comStatus2); // Radio status flag : -1 =Invalid 0 = OK 1 = Does not exist 2 = No electricity 3 = Failed
                com2.setTransmitEnabled(com2Status == 0 && com2Transmit);
                com2.setReceiveEnabled(com2Status == 0 && (comReceiveAll || com2Transmit));
                const bool changedCom2 = myAircraft.getCom2System() != com2;
                m_simCom2 = com2;
                Q_UNUSED(com2Test)

                CTransponder transponder(myAircraft.getTransponder());
                transponder.setTransponderCode(qRound(simulatorOwnAircraft.transponderCode));
                m_simTransponder = transponder;

                // if the simulator ever sends SELCAL, add it here.
                // m_selcal SELCAL sync.would go here

                const bool changedXpr = (myAircraft.getTransponderCode() != transponder.getTransponderCode());

                if (changedCom1 || changedCom2 || changedXpr)
                {
                    // set in own aircraft provider
                    this->updateCockpit(com1, com2, transponder, identifier());
                }
            }
            else
            {
                --m_skipCockpitUpdateCycles;
            }

            // slower updates
            if (m_ownAircraftUpdateCycles % 10 == 0)
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
                            requestWeatherGrid(currentPosition, this->identifier());
                        }
                    }
                }

                // init terrain probes here has the advantage we can also switch it on/off at runtime
                if (m_useFsxTerrainProbe && !m_initFsxTerrainProbes)
                {
                    this->physicallyInitAITerrainProbes(position, 2); // init probe
                }

                // SB3 offsets updating
                m_simulatorInternals.setValue(QStringLiteral("fsx/sb3"), boolToEnabledDisabled(m_useSbOffsets));
                m_simulatorInternals.setValue(QStringLiteral("fsx/sb3packets"), m_useSbOffsets ? QString::number(m_sbDataReceived) : QStringLiteral("disabled"));

                // CG
                const CLength cg(simulatorOwnAircraft.cgToGroundFt, CLengthUnit::ft());
                this->updateOwnCG(cg);

                // Simulated objects instead of NON ATC
                m_simulatorInternals.setValue(QStringLiteral("fsx/addAsSimulatedObject"), boolToEnabledDisabled(m_useAddSimulatedObj));

            } // slow updates

            m_ownAircraftUpdateCycles++; // with 50 updates/sec long enough even for 32bit
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
            CAircraftSituation lastSituation = m_lastSentSituations[cs];
            const bool moving   = lastSituation.isMoving();
            const bool onGround = remoteAircraftData.isOnGround();

            // CElevationPlane: deg, deg, feet
            // we only remember near ground
            const CElevationPlane elevation = CElevationPlane(remoteAircraftData.latitudeDeg, remoteAircraftData.longitudeDeg, remoteAircraftData.elevationFt, CElevationPlane::singlePointRadius());
            if (remoteAircraftData.aboveGroundFt() < 250)
            {
                const CLength cg(remoteAircraftData.cgToGroundFt, CLengthUnit::ft());
                this->rememberElevationAndSimulatorCG(cs, simObject.getAircraftModel(), onGround, elevation, cg);
            }

            const bool log = this->isLogCallsign(cs);
            if (log)
            {
                // update lat/lng/alt with real data from sim
                const CAltitude alt(remoteAircraftData.altitudeFt, CAltitude::MeanSeaLevel, CAltitude::TrueAltitude, CLengthUnit::ft());
                lastSituation.setPosition(elevation);
                lastSituation.setAltitude(alt);
                lastSituation.setGroundElevation(elevation, CAircraftSituation::FromProvider);
                this->addLoopbackSituation(lastSituation);
            }

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

            // update in 2 providers
            this->rememberElevationAndSimulatorCG(cs, simObject.getAircraftModel(), false, CElevationPlane::null(), cg); // env. provider
            this->updateCGAndModelString(cs, cg, modelString); // remote aircraft provider
        }

        void CSimulatorFsxCommon::updateProbeFromSimulator(const CCallsign &callsign, const DataDefinitionPosData &remoteAircraftData)
        {
            const CElevationPlane elevation(remoteAircraftData.latitudeDeg, remoteAircraftData.longitudeDeg, remoteAircraftData.elevationFt, CElevationPlane::singlePointRadius());
            this->callbackReceivedRequestedElevation(elevation, callsign, false);
        }

        void CSimulatorFsxCommon::updateOwnAircraftFromSimulator(const DataDefinitionClientAreaSb &sbDataArea)
        {
            if (m_skipCockpitUpdateCycles > 0) { return; }

            // log SB offset
            if (m_logSbOffsets) { CLogMessage(this).info(u"SB from sim: " % sbDataArea.toQString()); }

            // SB XPDR mode
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

        void CSimulatorFsxCommon::updateOwnAircraftFromSimulatorFsuipc(const CTransponder &xpdr)
        {
            if (!m_useFsuipc) { return; }
            if (m_skipCockpitUpdateCycles > 0) { return; }
            const CSimulatedAircraft myAircraft(this->getOwnAircraft());
            const bool changed = (myAircraft.getTransponderMode() != xpdr.getTransponderMode());
            if (!changed) { return; }
            CTransponder myXpdr = myAircraft.getTransponder();
            myXpdr.setTransponderMode(xpdr.getTransponderMode());
            this->updateCockpit(myAircraft.getCom1System(), myAircraft.getCom2System(), myXpdr, this->identifier());
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
                // verify aircraft and also triggers new add if required
                // do not do this in the event loop, so we do this deferred
                if (!myself || this->isShuttingDownOrDisconnected()) { return; }
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
                    msg = CLogMessage(this).error(u"Cannot confirm AI object, empty callsign");
                    break;
                }

                // removed in meantime
                const bool aircraftStillInRange = this->isAircraftInRange(callsign);
                if (!m_simConnectObjects.contains(callsign))
                {
                    if (aircraftStillInRange)
                    {
                        msg = CLogMessage(this).warning(u"Callsign '%1' removed in meantime from AI objects, but still in range") << callsign.toQString();
                    }
                    else
                    {
                        this->removeFromAddPendingAndAddAgainAircraft(callsign);
                        msg = CLogMessage(this).info(u"Callsign '%1' removed in meantime and no longer in range") << callsign.toQString();
                    }
                    break;
                }

                CSimConnectObject &simObject = m_simConnectObjects[callsign];
                remoteAircraft = simObject.getAircraft(); // update, if something has changed

                if (!simObject.hasValidRequestAndObjectId() || simObject.isPendingRemoved())
                {
                    msg = CStatusMessage(this).warning(u"Object for callsign '%1'/id: %2 removed in meantime/invalid") << callsign.toQString() << simObject.getObjectId();
                    break;
                }

                // P3D also has SimConnect_AIReleaseControlEx which also allows to destroy the aircraft
                const SIMCONNECT_DATA_REQUEST_ID requestReleaseId = this->obtainRequestIdForSimObjAircraft();
                const bool released = this->releaseAIControl(simObject, requestReleaseId);

                if (!released)
                {
                    msg = CStatusMessage(this).error(u"Cannot confirm model '%1' %2") << remoteAircraft.getModelString() << simObject.toQString();
                    break;
                }

                // confirm as added, this is also required to request light, etc
                Q_ASSERT_X(simObject.isPendingAdded(), Q_FUNC_INFO, "Already confirmed, this should be the only place");
                simObject.setConfirmedAdded(true); // aircraft

                // request data on object
                this->requestPositionDataForSimObject(simObject);
                this->requestLightsForSimObject(simObject);
                this->requestModelInfoForSimObject(simObject);

                this->removeFromAddPendingAndAddAgainAircraft(callsign); // no longer try to add
                const bool updated = this->updateAircraftRendered(callsign, true);
                if (updated)
                {
                    static const QString debugMsg("CS: '%1' model: '%2' verified, request/object id: %3 %4");
                    if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, debugMsg.arg(callsign.toQString(), remoteAircraft.getModelString()).arg(simObject.getRequestId()).arg(simObject.getObjectId())); }

                    this->sendRemoteAircraftAtcDataToSimulator(simObject);
                    emit this->aircraftRenderingChanged(simObject.getAircraft());
                }
                else
                {
                    CLogMessage(this).warning(u"Verified aircraft '%1' model '%2', request/object id: %3 %4 was already marked rendered") << callsign.asString() << remoteAircraft.getModelString() << simObject.getRequestId() << simObject.getObjectId();
                }

                if (simObject.isConfirmedAdded() && simObject.getType() == CSimConnectObject::AircraftSimulatedObject)
                {
                    CLogMessage(this).warning(u"Confirm added model '%1' '%2', but as '%3'") << remoteAircraft.getCallsignAsString() << remoteAircraft.getModelString() << simObject.getTypeAsString();
                    this->triggerAutoTraceSendId(); // trace for some time (issues regarding this workaround?)
                    simObject.decreaseAddingExceptions(); // if previously increased and now working, reset
                }
            }
            while (false);

            // log errors and emit signal
            if (!msg.isEmpty() && msg.isWarningOrAbove())
            {
                CLogMessage::preformatted(msg);
                emit this->physicallyAddingRemoteModelFailed(CSimulatedAircraft(), false, false, msg);
            }

            // trigger adding pending aircraft if there are any
            if (!m_addPendingAircraft.isEmpty())
            {
                this->addPendingAircraftAfterAdded();
            }
        }

        void CSimulatorFsxCommon::addingAircraftFailed(const CSimConnectObject &simObject)
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild()) { Q_ASSERT_X(simObject.isAircraft(), Q_FUNC_INFO, "Need aircraft"); }
            if (!simObject.isAircraft()) { return; }

            // clean up
            m_simConnectObjects.removeByOtherSimObject(simObject);
            this->removeFromAddPendingAndAddAgainAircraft(simObject.getCallsign());

            CLogMessage(this).warning(u"Model failed to be added: '%1' details: %2") << simObject.getAircraftModelString() << simObject.getAircraft().toQString(true);
            CStatusMessage verifyMsg;
            const bool verifiedAircraft = this->verifyFailedAircraftInfo(simObject, verifyMsg); // aircraft.cfg existing?
            if (!verifyMsg.isEmpty()) { CLogMessage::preformatted(verifyMsg); }

            CSimConnectObject simObjAddAgain(simObject);
            simObjAddAgain.increaseAddingExceptions();
            if (!simObject.hasCallsign())
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Missing callsign");
                return;
            }

            if (!verifiedAircraft || simObjAddAgain.getAddingExceptions() > ThresholdAddException)
            {
                const CStatusMessage msg = verifiedAircraft ?
                                           CLogMessage(this).warning(u"Model '%1' %2 failed %3 time(s) before and will be disabled") << simObjAddAgain.getAircraftModelString() << simObjAddAgain.toQString() << simObjAddAgain.getAddingExceptions() :
                                           CLogMessage(this).warning(u"Model '%1' %2 failed verification and will be disabled") << simObjAddAgain.getAircraftModelString() << simObjAddAgain.toQString();
                this->updateAircraftEnabled(simObjAddAgain.getCallsign(), false); // disable
                emit this->physicallyAddingRemoteModelFailed(simObjAddAgain.getAircraft(), true, true, msg); // verify failed
            }
            else
            {
                CLogMessage(this).info(u"Will try '%1' again, aircraft: %2") << simObject.getAircraftModelString() << simObject.getAircraft().toQString(true);
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2000, this, [ = ]
                {
                    if (!myself) { return; }
                    if (this->isShuttingDownOrDisconnected()) { return; }
                    m_addPendingAircraft.insert(simObjAddAgain, true); // add failed object
                });
            }
        }

        bool CSimulatorFsxCommon::verifyFailedAircraftInfo(const CSimConnectObject &simObject, CStatusMessage &details) const
        {
            CAircraftModel model = simObject.getAircraftModel();

            const CSpecializedSimulatorSettings settings = this->getSimulatorSettings();
            const bool fileExists = CFsCommonUtil::adjustFileDirectory(model, settings.getModelDirectoriesOrDefault());
            bool canBeUsed = true;

            CStatusMessageList messages;
            if (fileExists)
            {
                // we can access the aircraft.cfg file
                bool parsed = false;
                const CAircraftCfgEntriesList entries = CAircraftCfgParser::performParsingOfSingleFile(model.getFileName(), parsed, messages);
                if (parsed)
                {
                    if (entries.containsTitle(model.getModelString()))
                    {
                        messages.push_back(CStatusMessage(this).info(u"Model '%1' exists in re-parsed file '%2'.") << model.getModelString() << model.getFileName());
                        canBeUsed = true; // all OK
                    }
                    else
                    {
                        messages.push_back(CStatusMessage(this).warning(u"Model '%1' no longer in re-parsed file '%2'. Models are: %3.") << model.getModelString() << model.getFileName() << entries.getTitlesAsString(true));
                        canBeUsed = false; // absolute no chance to use that one
                    }
                }
                else
                {
                    messages.push_back(CStatusMessage(this).warning(u"CS: '%1' Cannot parse file: '%2' (existing: %3)") << model.getCallsign().asString() << model.getFileName() << boolToYesNo(model.hasExistingCorrespondingFile()));
                }
            }
            else
            {
                // the file cannot be accessed right now, but the pilot client necessarily has access to them
                // so we just carry on
                messages = model.verifyModelData();
            }

            // as single message
            details = messages.toSingleMessage();

            // status
            return canBeUsed;
        }

        bool CSimulatorFsxCommon::logVerifyFailedAircraftInfo(const CSimConnectObject &simObject) const
        {
            CStatusMessage m;
            const bool r = verifyFailedAircraftInfo(simObject, m);
            if (!m.isEmpty()) { CLogMessage::preformatted(m); }
            return r;
        }

        void CSimulatorFsxCommon::verifyAddedTerrainProbe(const CSimulatedAircraft &remoteAircraftIn)
        {
            bool verified = false;
            CCallsign cs;

            // no simObject reference outside that block, because it will be deleted
            {
                CSimConnectObject &simObject = m_simConnectObjects[remoteAircraftIn.getCallsign()];
                simObject.setConfirmedAdded(true); // terrain probe
                simObject.resetTimestampToNow();
                cs = simObject.getCallsign();
                CLogMessage(this).info(u"Probe: '%1' '%2' confirmed, %3") << simObject.getCallsignAsString() << simObject.getAircraftModelString() << simObject.toQString();

                // fails for probe
                // SIMCONNECT_DATA_REQUEST_ID requestId = this->obtainRequestIdForSimObjTerrainProbe();
                // verified = this->releaseAIControl(simObject, requestId); // release probe
                verified = true;
            }

            if (!verified)
            {
                CLogMessage(this).info(u"Disable probes: '%1' failed to relase control") << cs.asString();
                m_useFsxTerrainProbe = false;
            }

            // trigger new adding from pending if any
            if (!m_addPendingAircraft.isEmpty())
            {
                this->addPendingAircraftAfterAdded();
            }
        }

        void CSimulatorFsxCommon::timerBasedObjectAddOrRemove()
        {
            this->addPendingAircraft(AddByTimer);
            if (!this->isTestMode()) { this->physicallyRemoveAircraftNotInProvider(); }
        }

        void CSimulatorFsxCommon::addPendingAircraftAfterAdded()
        {
            this->addPendingAircraft(AddAfterAdded); // addPendingAircraft is already "non blocking"
        }

        void CSimulatorFsxCommon::addPendingAircraft(AircraftAddMode mode)
        {
            if (m_addPendingAircraft.isEmpty()) { return; }
            const CCallsignSet aircraftCallsignsInRange(this->getAircraftInRangeCallsigns());
            CSimulatedAircraftList toBeAddedAircraft; // aircraft still to be added
            CCallsignSet toBeRemovedCallsigns;

            for (const CSimConnectObject &pendingSimObj : as_const(m_addPendingAircraft))
            {
                BLACK_VERIFY_X(pendingSimObj.hasCallsign(), Q_FUNC_INFO, "missing callsign");
                if (!pendingSimObj.hasCallsign()) { continue; }
                if (pendingSimObj.isTerrainProbe() || aircraftCallsignsInRange.contains(pendingSimObj.getCallsign()))
                {
                    toBeAddedAircraft.push_back(pendingSimObj.getAircraft());
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
                        this->physicallyAddRemoteAircraftImpl(nextPendingAircraft, mode, oldestSimObject);
                    });
                }
                else
                {
                    CLogMessage(this).warning(u"Pending aircraft without model string will be removed");
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
                    simObject.increaseAddingDirectlyRemoved();
                    m_addPendingAircraft.insert(simObject, true); // insert removed objects and update ts
                    m_simConnectObjects.removeByOtherSimObject(simObject); // we have it in pending now, no need to keep it in this list

                    const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
                    msg = CLogMessage(this).warning(u"Aircraft removed, '%1' '%2' object id '%3' out of reality bubble or other reason. Interpolator: '%4'")
                          << callsign.toQString() << simObject.getAircraftModelString()
                          << objectID << simObject.getInterpolatorInfo(setup.getInterpolatorMode());
                }
                else if (simObject.getAddingDirectlyRemoved() < ThresholdAddedAndDirectlyRemoved)
                {
                    const CStatusMessage m = CLogMessage(this).warning(u"Aircraft removed again multiple times and will be disabled, '%1' '%2' object id '%3'") << callsign.toQString() << simObject.getAircraftModelString() << objectID;
                    this->updateAircraftEnabled(simObject.getCallsign(), false);
                    emit this->physicallyAddingRemoteModelFailed(simObject.getAircraft(), true, true, m); // directly removed again
                }
                else
                {
                    msg = CLogMessage(this).warning(u"Removed '%1' from simulator, but was not initiated by us (swift): %1 '%2' object id %3") << callsign.toQString() << simObject.getAircraftModelString() << objectID;
                }

                // in all cases add verification details
                this->logVerifyFailedAircraftInfo(simObject);

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
            Q_UNUSED(event)
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
                CLogMessage(this, CLogCategories::cmdLine()).info(u"Tracing %1 driver sendIds is '%2'") << this->getSimulatorPluginInfo().getIdentifier() << boolToOnOff(trace);
                return true;
            }

            // .driver sboffsets on|off
            if (parser.matchesPart(1, "sboffsets") && parser.hasPart(2))
            {
                const bool on = parser.toBool(2);
                this->setUsingSbOffsetValues(on);
                CLogMessage(this, CLogCategories::cmdLine()).info(u"SB offsets is '%1'") << boolToOnOff(on);
                return true;
            }

            // .driver sblog on|off
            if (parser.matchesPart(1, "sblog") && parser.hasPart(2))
            {
                const bool on = parser.toBool(2);
                m_logSbOffsets = on;
                CLogMessage(this, CLogCategories::cmdLine()).info(u"SB log. offsets is '%1'") << boolToOnOff(on);
                return true;
            }

            return CSimulatorFsCommon::parseDetails(parser);
        }

        void CSimulatorFsxCommon::registerHelp()
        {
            if (CSimpleCommandParser::registered("BlackSimPlugin::CSimulatorFsxCommon::CSimulatorFsxCommon")) { return; }
            CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
            CSimpleCommandParser::registerCommand({".drv sendid on|off", "Trace simConnect sendId on|off"});
            CSimpleCommandParser::registerCommand({".drv sboffsets on|off", "SB offsets via simConnect on|off"});
            CSimpleCommandParser::registerCommand({".drv sblog on|off", "SB offsets logging on|off"});
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
                // on FSX we normally receive this one here when simulator goes down, and NOT onSimExit
                // in that case sim status is Connected, but not PAUSED or SIMULATING
                const SimulatorStatus simStatus = this->getSimulatorStatus();
                const bool disconnectedOrNotSimulating = simStatus.testFlag(Disconnected) || !simStatus.testFlag(Simulating);

                m_dispatchErrors++;
                this->triggerAutoTraceSendId();
                if (m_dispatchErrors == 2)
                {
                    // 2nd time, an error / avoid multiple messages
                    // idea: if it happens once ignore
                    const QString msg = QStringLiteral(u"%1: Dispatch error, sim.status: %2").arg(this->getSimulatorPluginInfo().getIdentifier(), ISimulator::statusToString(simStatus));
                    CLogMessage(this).log(disconnectedOrNotSimulating ? CStatusMessage::SeverityWarning : CStatusMessage::SeverityError, msg);
                }
                else if (m_dispatchErrors > 5)
                {
                    // this normally happens during a FSX crash or shutdown with simconnect
                    const QString msg = QStringLiteral(u"%1: Multiple dispatch errors, disconnecting. Sim.status: %2").arg(this->getSimulatorPluginInfo().getIdentifier(), ISimulator::statusToString(simStatus));
                    CLogMessage(this).log(disconnectedOrNotSimulating ? CStatusMessage::SeverityWarning : CStatusMessage::SeverityError, msg);
                    this->disconnectFrom();
                }
                return;
            }
            m_dispatchErrors = 0;
            if (m_useFsuipc && m_fsuipc)
            {
                if (m_dispatchProcCount % 10 == 0)
                {
                    // slow updates, here only when SB/SimConnect is disabled as those do the same thing
                    if (!m_useSbOffsets)
                    {
                        CSimulatedAircraft fsuipcAircraft(this->getOwnAircraft());
                        const bool ok = m_fsuipc->read(fsuipcAircraft, true, false, false);
                        if (ok)
                        {
                            this->updateOwnAircraftFromSimulatorFsuipc(fsuipcAircraft.getTransponder());
                        }
                    }
                }
                else
                {
                    // fast
                }
            }
        }

        bool CSimulatorFsxCommon::physicallyAddRemoteAircraftImpl(const CSimulatedAircraft &newRemoteAircraft, CSimulatorFsxCommon::AircraftAddMode addMode, const CSimConnectObject &correspondingSimObject)
        {
            const CCallsign callsign(newRemoteAircraft.getCallsign());
            const bool probe = newRemoteAircraft.isTerrainProbe();

            // entry checks
            Q_ASSERT_X(CThreadUtils::isInThisThread(this),  Q_FUNC_INFO, "thread");
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
            Q_ASSERT_X(newRemoteAircraft.hasModelString(), Q_FUNC_INFO, "missing model string");

            // reset timer
            m_simObjectTimer.start(AddPendingAircraftIntervalMs); // restart

            // remove outdated objects
            const CSimConnectObjects outdatedAdded = m_simConnectObjects.removeOutdatedPendingAdded(CSimConnectObject::AllTypes);
            if (!outdatedAdded.isEmpty())
            {
                const CCallsignSet callsigns = outdatedAdded.getAllCallsigns(false);
                CLogMessage(this).warning(u"Removed %1 outdated object(s) pending for added: %2") << outdatedAdded.size() << callsigns.getCallsignsAsString(true);
                this->updateMultipleAircraftEnabled(callsigns, false);

                static const QString msgText("%1 outdated adding, %2");
                for (const CSimConnectObject &simObjOutdated : outdatedAdded)
                {
                    const CStatusMessage msg = CStatusMessage(this).warning(msgText.arg(simObjOutdated.getCallsign().asString(), simObjOutdated.toQString()));
                    emit this->physicallyAddingRemoteModelFailed(simObjOutdated.getAircraft(), true, true, msg); // outdated
                }

                // if this aircraft is also outdated, ignore
                if (callsigns.contains(newRemoteAircraft.getCallsign())) { return false; }
            }

            const bool hasPendingAdded = m_simConnectObjects.containsPendingAdded();
            bool canAdd = this->isSimulating() && !hasPendingAdded;

            Q_ASSERT_X(!hasPendingAdded || m_simConnectObjects.countPendingAdded() < 2, Q_FUNC_INFO, "There must be only 0..1 pending objects");
            if (this->showDebugLogMessage())
            {
                this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("CS: '%1' mode: '%2' model: '%3'").arg(newRemoteAircraft.getCallsignAsString(), modeToString(addMode), newRemoteAircraft.getModelString()));
                this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("CS: '%1' pending callsigns: '%2', pending objects: '%3'").arg(newRemoteAircraft.getCallsignAsString(), m_addPendingAircraft.getAllCallsignStrings(true).join(", "), m_simConnectObjects.getPendingAddedCallsigns().getCallsignStrings().join(", ")));
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
                    CLogMessage(this).info(u"CS: '%1' re-added same model '%2'") << newRemoteAircraft.getCallsignAsString() << newModelString;

                    // we restore rendered flag in case we are sure we are rendered
                    // this is used with rematching
                    const bool rendered = simObject.isConfirmedAdded() && simObject.isPending();
                    if (rendered) { this->updateAircraftRendered(callsign, rendered); }
                    return true;
                }

                this->physicallyRemoveRemoteAircraft(newRemoteAircraft.getCallsign());
                m_addAgainAircraftWhenRemoved.replaceOrAddByCallsign(newRemoteAircraft);
                if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("CS: '%1' re-added changed model '%2', will be added again").arg(newRemoteAircraft.getCallsignAsString(), newModelString)); }
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
                    CLogMessage(this).warning(u"No valid situations for '%1', will be added as pending") << callsign.asString();
                }
                else
                {
                    CLogMessage(this).warning(u"Invalid aircraft situation for new aircraft '%1', use closest situation") << callsign.asString();
                    situation = situations.findClosestTimeDistanceAdjusted(QDateTime::currentMSecsSinceEpoch());
                    Q_ASSERT_X(!situation.isPositionOrAltitudeNull(), Q_FUNC_INFO, "Invalid situation for new aircraft");
                }

                // still invalid?
                canAdd = situation.isPositionOrAltitudeNull();
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    BLACK_VERIFY_X(canAdd, Q_FUNC_INFO, "Expect valid situation");
                    CLogMessage(this).warning(u"Invalid situation for '%1'") << callsign;
                }
            }

            // check if we can add, do not add if simulator is stopped or other objects pending
            if (!canAdd)
            {
                CSimConnectObject &addPendingObj = m_addPendingAircraft[newRemoteAircraft.getCallsign()];
                addPendingObj.setAircraft(newRemoteAircraft);
                addPendingObj.resetTimestampToNow();
                return false;
            }

            // remove from pending and keep for later to remember fail counters
            const CSimConnectObject removedPendingObj = this->removeFromAddPendingAndAddAgainAircraft(callsign);

            // create AI after crosschecking it
            if (!probe && !this->isAircraftInRangeOrTestMode(callsign))
            {
                CLogMessage(this).info(u"Skipping adding of '%1' since it is no longer in range") << callsign.asString();
                return false;
            }

            // setup
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, true);
            const bool sendGround = setup.isSendingGndFlagToSimulator();

            // FSX/P3D adding
            bool adding = false; // will be added flag
            const SIMCONNECT_DATA_REQUEST_ID requestId = probe ? this->obtainRequestIdForSimObjTerrainProbe() : this->obtainRequestIdForSimObjAircraft();

            // Initial situation, if possible from interpolation
            CAircraftSituation initialSituation = newRemoteAircraft.getSituation(); // default
            {
                // Dummy CSimConnectObject just for interpolation
                const CSimConnectObject dummyObject = CSimConnectObject(newRemoteAircraft, 0, this, this, this->getRemoteAircraftProvider(), &m_interpolationLogger);
                const CInterpolationResult result = dummyObject.getInterpolation(QDateTime::currentMSecsSinceEpoch(), setup, 0);
                if (result.getInterpolationStatus().isInterpolated())
                {
                    initialSituation = result.getInterpolatedSituation();
                }
            }

            // under flow can cause a model not to be added
            // FSX: underflow and NO(!) gnd flag can cause adding/remove issue
            // P3D: underflow did not cause such issue
            CStatusMessage underflowStatus;
            const SIMCONNECT_DATA_INITPOSITION initialPosition = CSimulatorFsxCommon::aircraftSituationToFsxPosition(initialSituation, sendGround, true, &underflowStatus);

            const QString modelString(newRemoteAircraft.getModelString());
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("CS: '%1' model: '%2' request: %3, init pos: %4").arg(callsign.toQString(), modelString).arg(requestId).arg(fsxPositionToString(initialPosition))); }

            const QByteArray modelStringBa = toFsxChar(modelString);
            const QByteArray csBa = toFsxChar(callsign.toQString().left(12));
            CSimConnectObject::SimObjectType type = CSimConnectObject::AircraftNonAtc;
            HRESULT hr = S_OK;

            if (probe)
            {
                hr = SimConnect_AICreateSimulatedObject(m_hSimConnect, modelStringBa.constData(), initialPosition, requestId);
                type = CSimConnectObject::TerrainProbe;
            }
            else
            {
                if (this->isAddingAsSimulatedObjectEnabled() && correspondingSimObject.hasCallsign() && correspondingSimObject.getAddingExceptions() > 0 && correspondingSimObject.getType() == CSimConnectObject::AircraftNonAtc)
                {
                    CStatusMessage(this).warning(u"Model '%1' for '%2' failed %1 time(s) before, using AICreateSimulatedObject now") << newRemoteAircraft.getModelString() << callsign.toQString();
                    hr = SimConnect_AICreateSimulatedObject(m_hSimConnect, modelStringBa.constData(), initialPosition, requestId);
                    type = CSimConnectObject::AircraftSimulatedObject;
                }
                else
                {
                    hr = SimConnect_AICreateNonATCAircraft(m_hSimConnect, modelStringBa.constData(), csBa.constData(), initialPosition, requestId);
                    type = CSimConnectObject::AircraftNonAtc;
                }
            }

            if (!underflowStatus.isEmpty())
            {
                CStatusMessage(this).warning(u"Underflow detecion for '%1', details '%2'") << callsign.asString() << underflowStatus.getMessage();
            }

            if (isFailure(hr))
            {
                const CStatusMessage msg = CStatusMessage(this).error(u"SimConnect, can not create AI traffic: '%1' '%2'") << callsign.toQString() << modelString;
                CLogMessage::preformatted(msg);
                emit this->physicallyAddingRemoteModelFailed(newRemoteAircraft, true, true, msg); // SimConnect error
            }
            else
            {
                // we will request a new aircraft by request ID, later we will receive its object id
                // so far this object id is 0 (DWORD)
                const CSimConnectObject simObject = this->insertNewSimConnectObject(newRemoteAircraft, requestId, type, removedPendingObj);
                this->traceSendId(simObject, Q_FUNC_INFO, QStringLiteral("mode: %1").arg(CSimulatorFsxCommon::modeToString(addMode)), true);
                adding = true;
            }
            return adding;
        }

        bool CSimulatorFsxCommon::physicallyAddAITerrainProbe(const ICoordinateGeodetic &coordinate, int number)
        {
            if (coordinate.isNull()) { return false; }
            if (!this->isUsingFsxTerrainProbe()) { return false; }
            Q_ASSERT_X(CThreadUtils::isInThisThread(this),  Q_FUNC_INFO, "thread");

            // static const QString modelString("OrcaWhale");
            // static const QString modelString("Water Drop"); // not working on P3Dx86/FSX, no requests on that id possible
            // static const QString modelString("A321ACA");
            // static const QString modelString("AI_Tracker_Object_0");
            // static const QString modelString("Piper Cub"); // P3Dv86 works as nonATC/SimulatedObject
            // static const QString modelString("Discovery Spaceshuttle"); // P3Dx86 works as nonATC/SimulatedObject
            static const QString modelString("swiftTerrainProbe0");
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
            if (m_initFsxTerrainProbes) { return m_addedProbes; }
            m_initFsxTerrainProbes = true; // no multiple inits
            this->triggerAutoTraceSendId();

            int c = 0;
            for (int n = 0; n < number; ++n)
            {
                if (this->physicallyAddAITerrainProbe(coordinate, n)) { c++; }
            }

            CLogMessage(this).info(u"Adding %1 FSX terrain probes") << number;
            m_addedProbes = c;
            return c;
        }

        bool CSimulatorFsxCommon::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            // only remove from sim
            Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "wrong thread");
            if (callsign.isEmpty()) { return false; } // can happen if an object is not an aircraft

            // clean up anyway
            this->removeFromAddPendingAndAddAgainAircraft(callsign);

            // really remove from simulator
            if (!m_simConnectObjects.contains(callsign)) { return false; } // already fully removed or not yet added
            CSimConnectObject &simObject = m_simConnectObjects[callsign];
            if (simObject.isPendingRemoved()) { return true; }
            if (simObject.isTerrainProbe()) { return false; }

            // check for pending objects
            m_addPendingAircraft.remove(callsign); // just in case still in list of pending aircraft
            const bool pendingAdded = simObject.isPendingAdded(); // already added in simulator, but not yet confirmed
            const bool stillWaitingForLights = !simObject.hasCurrentLightsInSimulator();
            if (!simObject.isRemovedWhileAdding() && (pendingAdded || stillWaitingForLights))
            {
                // problem: we try to delete an aircraft just requested to be added
                // best solution so far, call remove again with a delay
                CLogMessage(this).warning(u"'%1' requested to be removed, but pending added (%2) / or pending lights(%3). Object will be removed again: %4")
                        << callsign.asString() << boolToYesNo(pendingAdded)
                        << boolToYesNo(stillWaitingForLights) << simObject.toQString();
                simObject.setRemovedWhileAdding(true); // next time kill
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2000, this, [ = ]
                {
                    if (!myself) { return; }
                    CLogMessage(this).info(u"Next trial to remove '%1'") << callsign.asString();
                    myself->physicallyRemoveRemoteAircraft(callsign);
                });
                return false; // not yet deleted
            }

            // no more data from simulator
            this->stopRequestingDataForSimObject(simObject);

            // mark as removed
            simObject.setPendingRemoved(true);
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("CS: '%1' request/object id: %2/%3").arg(callsign.toQString()).arg(simObject.getRequestId()).arg(simObject.getObjectId())); }

            // call in SIM
            const SIMCONNECT_DATA_REQUEST_ID requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectRemove);
            this->removeCamera(simObject);
            this->removeObserver(simObject);
            const HRESULT result = SimConnect_AIRemoveObject(m_hSimConnect, static_cast<SIMCONNECT_OBJECT_ID>(simObject.getObjectId()), requestId);
            if (isOk(result))
            {
                if (this->isTracingSendId()) { this->traceSendId(simObject, Q_FUNC_INFO);}
            }
            else
            {
                CLogMessage(this).warning(u"Removing aircraft '%1' from simulator failed") << callsign.asString();
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
            this->physicallyRemoveAircraftNotInProviderAsync();

            // bye
            return CSimulatorPluginCommon::physicallyRemoveRemoteAircraft(callsign);
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

            CSimulatorFsCommon::physicallyRemoveAllRemoteAircraft();
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
                CLogMessage(this).error(u"FSX plugin error: %1") << "SimConnect_SubscribeToSystemEvent failed";
                return hr;
            }

            // Mapped events, see event ids here:
            // http://msdn.microsoft.com/en-us/library/cc526980.aspx
            // http://www.prepar3d.com/SDKv2/LearningCenter/utilities/variables/event_ids.html
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventPauseToggle, "PAUSE_TOGGLE");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, SystemEventSlewToggle, "SLEW_TOGGLE");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeLatLng, "FREEZE_LATITUDE_LONGITUDE_SET"); // FSX old standard
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeAlt, "FREEZE_ALTITUDE_SET"); // FSX old standard
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeAtt, "FREEZE_ATTITUDE_SET"); // FSX old standard
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

            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFlapsSet, "FLAPS_SET");

            if (isFailure(hr))
            {
                CLogMessage(this).error(u"FSX plugin error: %1") << "SimConnect_MapClientEventToSimEvent failed";
                return hr;
            }

            // facility
            SIMCONNECT_DATA_REQUEST_ID requestId = static_cast<SIMCONNECT_DATA_REQUEST_ID>(CSimConnectDefinitions::RequestFacility);
            hr += SimConnect_SubscribeToFacilities(m_hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT, requestId);
            if (isFailure(hr))
            {
                CLogMessage(this).error(u"FSX plugin error: %1") << "SimConnect_SubscribeToFacilities failed";
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
                CLogMessage(this).error(u"FSX plugin: initEvents failed");
                return hr;
            }

            // init data definitions and SB data area
            hr += this->initDataDefinitionsWhenConnected();
            if (isFailure(hr))
            {
                CLogMessage(this).error(u"FSX plugin: initDataDefinitionsWhenConnected failed");
                return hr;
            }

            return hr;
        }

        void CSimulatorFsxCommon::updateRemoteAircraft()
        {
            static_assert(sizeof(DataDefinitionRemoteAircraftPartsWithoutLights) == sizeof(double) * 10, "DataDefinitionRemoteAircraftPartsWithoutLights has an incorrect size.");
            Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "thread");

            // Freeze interpolation while paused
            if (this->isPaused() && m_pausedSimFreezesInterpolation) { return; }

            // nothing to do, reset request id and exit
            const int remoteAircraftNo = this->getAircraftInRangeCount();
            if (remoteAircraftNo < 1) { m_statsUpdateAircraftRuns = 0;  return; }

            // values used for position and parts
            const qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
            if (this->isUpdateAircraftLimitedWithStats(currentTimestamp))
            {
                this->finishUpdateRemoteAircraftAndSetStatistics(currentTimestamp, true);
                return;
            }
            m_updateRemoteAircraftInProgress = true;

            // interpolation for all remote aircraft
            const QList<CSimConnectObject> simObjects(m_simConnectObjects.values());

            int simObjectNumber = 0;
            const bool traceSendId       = this->isTracingSendId();
            const bool updateAllAircraft = this->isUpdateAllRemoteAircraft(currentTimestamp);
            for (const CSimConnectObject &simObject : simObjects)
            {
                // happening if aircraft is not yet added to simulator or to be deleted
                if (!simObject.isReadyToSend()) { continue; }
                if (!simObject.hasCurrentLightsInSimulator()) { continue; } // wait until we have light state

                const CCallsign callsign(simObject.getCallsign());
                const bool hasCs = !callsign.isEmpty();
                const bool hasValidIds = simObject.hasValidRequestAndObjectId();
                BLACK_VERIFY_X(hasCs, Q_FUNC_INFO, "missing callsign");
                BLACK_AUDIT_X(hasValidIds, Q_FUNC_INFO, "Missing ids");
                if (!hasCs || !hasValidIds) { continue; } // not supposed to happen
                const DWORD objectId = simObject.getObjectId();

                // setup
                const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, updateAllAircraft);
                const bool sendGround = setup.isSendingGndFlagToSimulator();

                // Interpolated situation
                // simObjectNumber is passed to equally distributed steps like guessing parts
                const bool slowUpdate = (((m_statsUpdateAircraftRuns + simObjectNumber) % 40) == 0);
                const CInterpolationResult result = simObject.getInterpolation(currentTimestamp, setup, simObjectNumber++);
                const bool forceUpdate = slowUpdate || updateAllAircraft || setup.isForcingFullInterpolation();
                if (result.getInterpolationStatus().hasValidSituation())
                {
                    // update situation
                    if (forceUpdate || !this->isEqualLastSent(result.getInterpolatedSituation()))
                    {
                        SIMCONNECT_DATA_INITPOSITION position = this->aircraftSituationToFsxPosition(result, sendGround);
                        const HRESULT hr = this->logAndTraceSendId(
                                               SimConnect_SetDataOnSimObject(
                                                   m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                                                   static_cast<SIMCONNECT_OBJECT_ID>(objectId), 0, 0, sizeof(SIMCONNECT_DATA_INITPOSITION), &position),
                                               traceSendId, simObject, "Failed to set position", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject");
                        if (isOk(hr))
                        {
                            this->rememberLastSent(result); // remember situation
                        }
                    }
                }
                else
                {
                    // already logged in interpolator
                    continue;
                }

                // Interpolated parts
                const bool updatedParts = this->updateRemoteAircraftParts(simObject, result, forceUpdate);
                Q_UNUSED(updatedParts)

            } // all callsigns

            // stats
            this->finishUpdateRemoteAircraftAndSetStatistics(currentTimestamp);
        }

        bool CSimulatorFsxCommon::updateRemoteAircraftParts(const CSimConnectObject &simObject, const CInterpolationResult &result, bool forcedUpdate)
        {
            if (!simObject.hasValidRequestAndObjectId()) { return false; }
            if (!simObject.isConfirmedAdded())           { return false; }

            const CAircraftParts parts = result;
            if (parts.isNull()) { return false; }
            if (parts.getPartsDetails() != CAircraftParts::GuessedParts && !result.getPartsStatus().isSupportingParts()) { return false; }

            const CCallsign cs = simObject.getCallsign();
            if (!forcedUpdate && (result.getPartsStatus().isReusedParts() || this->isEqualLastSent(parts, cs))) { return true; }

            const bool ok = this->sendRemoteAircraftPartsToSimulator(simObject, parts);
            if (ok) { this->rememberLastSent(parts, cs); }
            return ok;
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

        bool CSimulatorFsxCommon::sendRemoteAircraftPartsToSimulator(const CSimConnectObject &simObject, const CAircraftParts &parts)
        {
            Q_ASSERT(m_hSimConnect);
            if (!simObject.isReadyToSend()) { return false; }

            const DWORD objectId = simObject.getObjectId();
            const bool traceId = this->isTracingSendId();

            DataDefinitionRemoteAircraftPartsWithoutLights ddRemoteAircraftPartsWithoutLights(parts);
            const CAircraftLights lights = parts.getAdjustedLights();

            // in case we sent, we sent everything
            const bool simObjectAircraftType = simObject.isAircraftSimulatedObject(); // no real aircraft type
            const HRESULT hr1 = simObjectAircraftType ?
                                S_OK :
                                this->logAndTraceSendId(
                                    SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                            static_cast<SIMCONNECT_OBJECT_ID>(objectId), SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0,
                                            sizeof(DataDefinitionRemoteAircraftPartsWithoutLights), &ddRemoteAircraftPartsWithoutLights),
                                    traceId, simObject, "Failed so set parts", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject::ddRemoteAircraftPartsWithoutLights");

            // Sim variable version, not working, setting the value, but flaps retracting to 0 again
            // Sets flap handle to closest increment (0 to 16383)
            const DWORD flapsDw = static_cast<DWORD>(qMin(16383, qRound((parts.getFlapsPercent() / 100.0) * 16383)));
            const HRESULT hr2 = this->logAndTraceSendId(
                                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventFlapsSet, flapsDw,
                                            SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                                    traceId, simObject, "Failed so set flaps", Q_FUNC_INFO, "SimConnect_TransmitClientEvent::EventFlapsSet");

            // lights we can set directly
            const HRESULT hr3 = this->logAndTraceSendId(
                                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventLandingLightsSet, lights.isLandingOn() ? 1.0 : 0.0,
                                            SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                                    traceId, simObject, "Failed so set landing lights", Q_FUNC_INFO, "SimConnect_TransmitClientEvent::EventLandingLightsSet");

            const HRESULT hr4 = this->logAndTraceSendId(
                                    SimConnect_TransmitClientEvent(m_hSimConnect, objectId, EventStrobesSet, lights.isStrobeOn() ? 1.0 : 0.0,
                                            SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY),
                                    traceId, simObject, "Failed to set strobe lights", Q_FUNC_INFO, "SimConnect_TransmitClientEvent::EventStrobesSet");

            // lights we need to toggle
            // (potential risk with quickly changing values that we accidentally toggle back, also we need the light state before we can toggle)
            this->sendToggledLightsToSimulator(simObject, lights);

            // done
            return isOk(hr1, hr2, hr3, hr4);
        }

        bool CSimulatorFsxCommon::sendRemoteAircraftAtcDataToSimulator(const CSimConnectObject &simObject)
        {
            if (!simObject.isReadyToSend()) { return false; }
            if (simObject.isTerrainProbe()) { return false; }
            // if (simObject.getType() != CSimConnectObject::AircraftNonAtc) { return false; } // otherwise errors

            const DWORD objectId = simObject.getObjectId();
            const bool traceId = this->isTracingSendId();

            DataDefinitionRemoteAtc ddAtc;
            ddAtc.setDefaultValues();
            const QByteArray csBa = simObject.getCallsignByteArray();
            const QByteArray airlineBa = simObject.getAircraft().getAirlineIcaoCode().getName().toLatin1();
            const QByteArray flightNumberBa = QString::number(simObject.getObjectId()).toLatin1();

            ddAtc.copyAtcId(csBa.constData());
            ddAtc.copyAtcAirline(airlineBa.constData());
            ddAtc.copyFlightNumber(flightNumberBa.constData());

            // in case we sent, we sent everything
            const HRESULT hr = this->logAndTraceSendId(
                                   SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetData,
                                           static_cast<SIMCONNECT_OBJECT_ID>(objectId), SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0,
                                           sizeof(DataDefinitionRemoteAtc), &ddAtc),
                                   traceId, simObject, "Failed so aircraft ATC data", Q_FUNC_INFO, "SimConnect_SetDataOnSimObject");
            // done
            return isOk(hr);
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
            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("Missing light state in simulator for '%1', model '%2'").arg(callsign.asString(), simObj.getAircraftModelString())); }

            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(DeferResendingLights, this, [ = ]
            {
                if (!myself) { return; }
                if (!m_simConnectObjects.contains(callsign)) { return; }
                const CSimConnectObject currentSimObject = m_simConnectObjects[callsign];
                if (!currentSimObject.isReadyToSend()) { return; } // stale
                if (lightsWanted != currentSimObject.getLightsAsSent())  { return; } // changed in between, so another call sendToggledLightsToSimulator is pending
                if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("Resending light state for '%1', model '%2'").arg(callsign.asString(), simObj.getAircraftModelString())); }
                this->sendToggledLightsToSimulator(currentSimObject, lightsWanted, true);
            });
        }

        SIMCONNECT_DATA_INITPOSITION CSimulatorFsxCommon::aircraftSituationToFsxPosition(const CAircraftSituation &situation, bool sendGnd, bool forceUnderflowDetection, CStatusMessage *details)
        {
            Q_ASSERT_X(!situation.isGeodeticHeightNull(), Q_FUNC_INFO, "Missing height (altitude)");
            Q_ASSERT_X(!situation.isPositionNull(), Q_FUNC_INFO,  "Missing position");

            // lat/Lng, NO PBH
            CAircraftSituation::AltitudeCorrection altCorrection = CAircraftSituation::UnknownCorrection;
            SIMCONNECT_DATA_INITPOSITION position = CSimulatorFsxCommon::coordinateToFsxPosition(situation);
            if (forceUnderflowDetection)
            {
                const CAltitude alt = situation.getCorrectedAltitude(true, &altCorrection);
                position.Altitude = alt.value(CLengthUnit::ft());
            }

            // MSFS has inverted pitch and bank angles
            position.Pitch    = -situation.getPitch().value(CAngleUnit::deg());
            position.Bank     = -situation.getBank().value(CAngleUnit::deg());
            position.Heading  = situation.getHeading().value(CAngleUnit::deg());
            position.OnGround = 0U; // not on ground

            const double gsKts = situation.getGroundSpeed().value(CSpeedUnit::kts());
            position.Airspeed = static_cast<DWORD>(qRound(gsKts));

            // sanity check
            if (gsKts < 0.0)
            {
                // we get negative GS for pushback and helicopters
                // here we handle them her with DWORD (unsigned)
                position.Airspeed = 0U;
            }
            else
            {
                position.Airspeed = static_cast<DWORD>(qRound(gsKts));
            }

            // send GND flag also when underflow detection is available
            if ((sendGnd || forceUnderflowDetection) && situation.isOnGroundInfoAvailable())
            {
                const bool onGround = (situation.getOnGround() == CAircraftSituation::OnGround);
                position.OnGround = onGround ? 1U : 0U;
            }

            // if we have no GND flag yet (gnd flag prevents underflow)
            if (forceUnderflowDetection && position.OnGround == 0 && !CAircraftSituation::isCorrectedAltitude(altCorrection))
            {
                // logical resolution failed so far, likely we have no CG or elevantion
                // primitive guessing
                do
                {
                    if (position.Airspeed < 2)
                    {
                        position.OnGround = 1U;
                        if (details) { *details = CStatusMessage(static_cast<CSimulatorFsxCommon *>(nullptr)).warning(u"Force GND flag for underflow protection"); }
                        break;
                    }
                }
                while (false);
            }

            // crosscheck
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(isValidFsxPosition(position), Q_FUNC_INFO, "Invalid FSX pos.");
            }

            return position;
        }

        SIMCONNECT_DATA_PBH CSimulatorFsxCommon::aircraftSituationToFsxPBH(const CAircraftSituation &situation)
        {
            // MSFS has inverted pitch and bank angles
            SIMCONNECT_DATA_PBH pbh;
            pbh.Pitch   = -situation.getPitch().value(CAngleUnit::deg());
            pbh.Bank    = -situation.getBank().value(CAngleUnit::deg());
            pbh.Heading = situation.getHeading().value(CAngleUnit::deg());
            return pbh;
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

        SIMCONNECT_DATA_LATLONALT CSimulatorFsxCommon::coordinateToFsxLatLonAlt(const ICoordinateGeodetic &coordinate)
        {
            SIMCONNECT_DATA_LATLONALT lla;
            lla.Latitude = coordinate.latitude().value(CAngleUnit::deg());
            lla.Longitude = coordinate.longitude().value(CAngleUnit::deg());
            lla.Altitude = coordinate.geodeticHeight().value(CLengthUnit::ft()); // already corrected in interpolator if there is an underflow
            return lla;
        }

        bool CSimulatorFsxCommon::isValidFsxPosition(const SIMCONNECT_DATA_INITPOSITION &fsxPos)
        {
            // double  Latitude;   // degrees | double  Longitude;  // degrees | double  Altitude;   // feet
            // double  Pitch;      // degrees | double  Bank;       // degrees | double  Heading;    // degrees
            // DWORD   OnGround;   // 1=force to be on the ground | DWORD   Airspeed;   // knots
            // https://www.prepar3d.com/SDKv4/sdk/simconnect_api/references/simobject_functions.html
            // examples show heaading 180 => we assume values +-180deg
            if (!isValid180Deg(fsxPos.Pitch))     { return false; }
            if (!isValid180Deg(fsxPos.Bank))      { return false; }
            if (!isValid180Deg(fsxPos.Heading))   { return false; }
            if (!isValid180Deg(fsxPos.Latitude))  { return false; }
            if (!isValid180Deg(fsxPos.Longitude)) { return false; }
            return true;
        }

        void CSimulatorFsxCommon::synchronizeTime(const DataDefinitionSimEnvironment *simEnv)
        {
            if (!m_simTimeSynced) { return; }
            if (!this->isConnected())   { return; }
            if (m_syncTimeDeferredCounter > 0)
            {
                --m_syncTimeDeferredCounter;
                return; // wait some time before we snyc again
            }

            const int zh = simEnv->zuluTimeSeconds / 3600;
            const int zm = (simEnv->zuluTimeSeconds - (zh * 3600)) / 60;
            const CTime zuluTimeSim(zh, zm);
            // const int lh = simEnv->localTimeSeconds / 3600;
            // const int lm = (simEnv->localTimeSeconds - (lh * 3600)) / 60;
            // const CTime localTimeSim(lh, lm);
            // Q_UNUSED(localTimeSim);

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
            if (diffMins < 2)
            {
                // checked and no relevant difference
                m_syncTimeDeferredCounter = 10; // wait some time to check again
                return;
            }
            const HRESULT hr1 = SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTimeZuluHours, h, SIMCONNECT_GROUP_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            const HRESULT hr2 = SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTimeZuluMinutes, m, SIMCONNECT_GROUP_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

            if (isFailure(hr1, hr2))
            {
                CLogMessage(this).warning(u"Sending time sync failed!");
            }
            else
            {
                m_syncTimeDeferredCounter = 5; // allow some time to sync
                CLogMessage(this).info(u"Synchronized time to UTC: '%1'") << myTime.toString();
            }
        }

        void CSimulatorFsxCommon::injectWeatherGrid(const CWeatherGrid &weatherGrid)
        {
            if (this->isShuttingDownOrDisconnected()) { return; }
            if (weatherGrid.isEmpty()) { return; }

            if (!CThreadUtils::isInThisThread(this))
            {
                BLACK_VERIFY_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Wrong thread");
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(0, this, [ = ]
                {
                    if (!myself) { return; }
                    myself->injectWeatherGrid(weatherGrid);
                });
                return;
            }

            // So far, there is only global weather
            const bool isFSX = this->getSimulatorPluginInfo().getSimulatorInfo().isFSX();
            CGridPoint glob = weatherGrid.frontOrDefault();
            glob.setIdentifier("GLOB");
            const QString metar = CSimConnectUtilities::convertToSimConnectMetar(glob, isFSX);
            const QByteArray metarBa = toFsxChar(metar);

            // send
            SimConnect_WeatherSetModeCustom(m_hSimConnect);
            SimConnect_WeatherSetModeGlobal(m_hSimConnect);

            if (!metarBa.isEmpty())
            {
                // Q_ASSERT_X(metarBa.back() == 0, Q_FUNC_INFO, "Need 0 terminated string");
                SimConnect_WeatherSetObservation(m_hSimConnect, 0, metarBa.constData());
                CLogMessage(this).debug(u"Injecting weather: %1") << metar;
            }
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
                                           m_hSimConnect, reqId, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
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
                                           m_hSimConnect, static_cast<SIMCONNECT_DATA_REQUEST_ID>(requestId),
                                           CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                           static_cast<SIMCONNECT_OBJECT_ID>(objectId), SIMCONNECT_PERIOD_ONCE),
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
                                           m_hSimConnect, requestId, CSimConnectDefinitions::DataRemoteAircraftModelData,
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
                                        m_hSimConnect, requestId, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                        simObject.getObjectId(), SIMCONNECT_PERIOD_NEVER),
                                    simObject, "Stopping position request", Q_FUNC_INFO, "SimConnect_RequestDataOnSimObject");

            requestId = simObject.getRequestId(CSimConnectDefinitions::SimObjectLights);
            const HRESULT hr2 = this->logAndTraceSendId(
                                    SimConnect_RequestDataOnSimObject(
                                        m_hSimConnect, requestId, CSimConnectDefinitions::DataRemoteAircraftLights,
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

            // cleared below:
            // physicallyRemoveAllRemoteAircraft
            // m_simConnectObjects
            // m_simConnectObjectsPositionAndPartsTraces
            // m_addPendingAircraft
            // m_updateRemoteAircraftInProgress
            CSimulatorFsCommon::reset(); // clears all pending aircraft etc

            // reset values
            m_simulatingChangedTs = -1;
            m_simConnected  = false;
            m_simSimulating = false;
            m_sbDataReceived = 0;
            m_syncTimeDeferredCounter = 0;
            m_requestIdSimObjAircraft = static_cast<SIMCONNECT_DATA_REQUEST_ID>(RequestSimObjAircraftStart);
            m_dispatchErrors = 0;
            m_receiveExceptionCount = 0;
            m_addedProbes    = 0;
            m_initFsxTerrainProbes = false;
            m_sendIdTraces.clear();
        }

        void CSimulatorFsxCommon::clearAllRemoteAircraftData()
        {
            const bool reinitProbe = m_useFsxTerrainProbe && m_initFsxTerrainProbes; // re-init if enabled and was initialized
            this->removeAllProbes();

            // m_addAgainAircraftWhenRemoved cleared below
            CSimulatorFsCommon::clearAllRemoteAircraftData(); // also removes aircraft
            m_simConnectObjects.clear();
            m_addPendingAircraft.clear();
            m_simConnectObjectsPositionAndPartsTraces.clear();

            if (reinitProbe)
            {
                // if we are still alive we re-init the probes
                QPointer<CSimulatorFsxCommon> myself(this);
                QTimer::singleShot(2000, this, [ = ]
                {
                    // Shutdown or unloaded
                    if (this->isShuttingDown() || !myself) { return; }
                    m_initFsxTerrainProbes = false; // probes will re-init
                });
            }
        }

        void CSimulatorFsxCommon::onOwnModelChanged(const CAircraftModel &newModel)
        {
            m_sbDataReceived = 0;
            CSimulatorFsCommon::onOwnModelChanged(newModel);
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
            if (m_simConnectObjects.isEmpty()) { return CCallsignSet(); }
            const CCallsignSet simObjectCallsigns(m_simConnectObjects.getAllCallsigns(true));
            const CCallsignSet providerCallsigns(this->getAircraftInRangeCallsigns());
            return simObjectCallsigns.difference(providerCallsigns);
        }

        void CSimulatorFsxCommon::traceSendId(const CSimConnectObject &simObject, const QString &functionName, const QString &details, bool forceTrace)
        {
            if (!forceTrace && !this->isTracingSendId()) { return; }
            // cppcheck-suppress knownConditionTrueFalse
            if (MaxSendIdTraces < 1) { return; }
            DWORD dwLastId = 0;
            const HRESULT hr = SimConnect_GetLastSentPacketID(m_hSimConnect, &dwLastId);
            if (isFailure(hr)) { return; }
            if (m_sendIdTraces.size() > MaxSendIdTraces) { m_sendIdTraces.removeLast(); }
            const TraceFsxSendId trace(dwLastId, simObject, details.isEmpty() ? functionName : details % u", " % functionName);
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
                CLogMessage(this).warning(warningMsg % u" SimObject: " % simObject.toQString());
            }
            this->triggerAutoTraceSendId();
            return hr;
        }

        QByteArray CSimulatorFsxCommon::toFsxChar(const QString &string)
        {
            return string.toLatin1();
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
            return {};
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
                    CLogMessage(this).warning(u"Removing probe '%1' from simulator failed") << probeSimObject.getObjectId();
                }
            }
            m_simConnectObjects.removeAllProbes();
            m_pendingProbeRequests.clear();
            return c;
        }

        CSimConnectObject CSimulatorFsxCommon::insertNewSimConnectObject(const CSimulatedAircraft &aircraft, DWORD requestId, CSimConnectObject::SimObjectType type, const CSimConnectObject &removedPendingObject)
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
            simObject.setType(type);
            m_simConnectObjects.insert(simObject, true); // update timestamp
            return simObject;
        }

        const CAltitude &CSimulatorFsxCommon::terrainProbeAltitude()
        {
            static const CAltitude alt(50000, CLengthUnit::ft());
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
            case CSimConnectObject::AircraftNonAtc:
            case CSimConnectObject::AircraftSimulatedObject:
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

            if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("CS: '%1'").arg(callsignsToBeRemoved.toStringList().join(", "))); }
            return callsignsToBeRemoved;
        }

        void CSimulatorFsxCommon::physicallyRemoveAircraftNotInProviderAsync()
        {
            const QPointer<CSimulatorFsxCommon> myself(this);
            QTimer::singleShot(100, this, [ = ]
            {
                if (!myself || this->isShuttingDown()) { return; }
                CSimulatorFsxCommon::physicallyRemoveAircraftNotInProvider();
            });
        }

        CSimulatorFsxCommonListener::CSimulatorFsxCommonListener(const CSimulatorPluginInfo &info) :
            ISimulatorListener(info)
        {
            m_timer.setInterval(MinQueryIntervalMs);
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
            this->disconnectFromSimulator();
        }

        void CSimulatorFsxCommonListener::checkImpl()
        {
            if (!m_timer.isActive())    { return; }
            if (this->isShuttingDown()) { return; }

            QPointer<CSimulatorFsxCommonListener> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself || !sApp || sApp->isShuttingDown()) { return; }
                this->checkConnection();
            });

            // restart because we have just checked now
            m_timer.start();
        }

        QString CSimulatorFsxCommonListener::backendInfo() const
        {
            if (m_simulatorName.isEmpty()) { return ISimulatorListener::backendInfo(); }
            return m_simulatorDetails;
        }

        void CSimulatorFsxCommonListener::checkConnection()
        {
            Q_ASSERT_X(!CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Expect to run in background");

            // check before we access the sim. connection
            if (this->isShuttingDown() || this->thread()->isInterruptionRequested())
            {
                this->stopImpl();
                return;
            }

            QElapsedTimer t; t.start();
            bool check = false;
            do
            {
                // if we can connect, but not dispatch, it can mean a previously started FSX/P3D
                // blocks remote calls -> RESTART
                if (!this->connectToSimulator()) { break; }

                // check if we have the right sim.
                // this check on a remote FSX/P3D not running/existing might TAKE LONG!
                const HRESULT result = SimConnect_CallDispatch(m_hSimConnect, CSimulatorFsxCommonListener::SimConnectProc, this);

                // make sure we did not stop in meantime
                if (this->isShuttingDown() || this->thread()->isInterruptionRequested())
                {
                    this->stopImpl();
                    return;
                }

                if (isFailure(result)) { break; } // means serious failure
                check = this->checkVersionAndSimulator();

            }
            while (false);

            this->adjustTimerInterval(t.elapsed());
            if (check)
            {
                emit this->simulatorStarted(this->getPluginInfo());
            }
        }

        void CSimulatorFsxCommonListener::adjustTimerInterval(qint64 checkTimeMs)
        {
            const QString sim = this->getPluginInfo().getSimulatorInfo().toQString(true);
            CLogMessage(this).debug(u"Checked sim.'%1' connection in %2ms") << sim << checkTimeMs;
            if (checkTimeMs > qRound(1.25 * MinQueryIntervalMs))
            {
                const int newIntervalMs = qRound(1.2 * checkTimeMs / 1000.0) * 1000;
                CLogMessage(this).debug(u"Check for simulator sim.'%1' connection in %2ms, too slow. Setting %3ms") << sim << checkTimeMs << newIntervalMs;
                if (m_timer.interval() != newIntervalMs) { m_timer.setInterval(newIntervalMs); }
            }
            else
            {
                if (m_timer.interval() != MinQueryIntervalMs) { m_timer.setInterval(MinQueryIntervalMs); }
            }

            // restart
            m_timer.start();
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
            else if (pluginSim.isMSFS())
            {
                // MSFS 2020 drivers only works with MSFS
                return connectedSimName.contains("kittyhawk");
            }
            return false;
        }

        bool CSimulatorFsxCommonListener::checkSimConnectDll() const
        {
            static const CWinDllUtils::DLLInfo simConnectInfo = CSimConnectUtilities::simConnectDllInfo();
            if (!simConnectInfo.errorMsg.isEmpty()) { return false; }
            return true;
        }

        bool CSimulatorFsxCommonListener::connectToSimulator()
        {
            if (m_simConnected) { return true; }
            const HRESULT result = SimConnect_Open(&m_hSimConnect, sApp->swiftVersionChar(), nullptr, 0, nullptr, 0);
            const bool ok = isOk(result);
            m_simConnected = ok;
            return ok;
        }

        bool CSimulatorFsxCommonListener::disconnectFromSimulator()
        {
            if (!m_simConnected) { return false; }
            SimConnect_Close(m_hSimConnect);
            m_hSimConnect = nullptr;
            m_simConnected   = false;
            return true;
        }

        void CSimulatorFsxCommonListener::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
        {
            Q_UNUSED(cbData)
            CSimulatorFsxCommonListener *simListener = static_cast<CSimulatorFsxCommonListener *>(pContext);
            switch (pData->dwID)
            {
            case SIMCONNECT_RECV_ID_OPEN:
                {
                    SIMCONNECT_RECV_OPEN *event = static_cast<SIMCONNECT_RECV_OPEN *>(pData);
                    simListener->m_simulatorVersion = QStringLiteral("%1.%2.%3.%4").arg(event->dwApplicationVersionMajor).arg(event->dwApplicationVersionMinor).arg(event->dwApplicationBuildMajor).arg(event->dwApplicationBuildMinor);
                    simListener->m_simConnectVersion = QStringLiteral("%1.%2.%3.%4").arg(event->dwSimConnectVersionMajor).arg(event->dwSimConnectVersionMinor).arg(event->dwSimConnectBuildMajor).arg(event->dwSimConnectBuildMinor);
                    simListener->m_simulatorName = CSimulatorFsxCommon::fsxCharToQString(event->szApplicationName);
                    simListener->m_simulatorDetails = QStringLiteral("Name: '%1' Version: %2 SimConnect: %3").arg(simListener->m_simulatorName, simListener->m_simulatorVersion, simListener->m_simConnectVersion);
                    const CStatusMessage msg = CStatusMessage(simListener).info(u"Connect to %1: '%2'") << simListener->getPluginInfo().getIdentifier() << simListener->backendInfo();

                    // avoid the same message over and over again
                    if (msg.getMessage() != simListener->m_lastMessage.getMessage())
                    {
                        CLogMessage::preformatted(msg);
                        simListener->m_lastMessage = msg;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION: break;
            default: break;
            }
        }
    } // namespace
} // namespace
