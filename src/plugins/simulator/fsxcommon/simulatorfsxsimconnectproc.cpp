// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <cstring>

#include "simconnectdatadefinition.h"
#include "simulatorfsxcommon.h"

#include "config/buildconfig.h"
#include "core/application.h"
#include "misc/aviation/airportlist.h"
#include "misc/logmessage.h"
#include "misc/simulation/fscommon/bcdconversions.h"
#include "misc/simulation/fsx/simconnectutilities.h"
#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::core;
using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::fsx;

namespace swift::simplugin::fsxcommon
{
    void CALLBACK CSimulatorFsxCommon::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
    {
        // IMPORTANT:
        // all tasks called in this function (ie all called functions) must perform fast or shall be called
        // asynchronously

        const qint64 procTimeStart = QDateTime::currentMSecsSinceEpoch();
        CSimulatorFsxCommon *simulatorFsxP3D = static_cast<CSimulatorFsxCommon *>(pContext);
        const SIMCONNECT_RECV_ID recvId = static_cast<SIMCONNECT_RECV_ID>(pData->dwID);
        simulatorFsxP3D->m_dispatchReceiveIdLast = recvId;
        simulatorFsxP3D->m_dispatchProcCount++;
        switch (recvId)
        {
        case SIMCONNECT_RECV_ID_OPEN:
        {
            SIMCONNECT_RECV_OPEN *event = static_cast<SIMCONNECT_RECV_OPEN *>(pData);
            const QString simConnectVersion = QStringLiteral("%1.%2.%3.%4")
                                                  .arg(event->dwSimConnectVersionMajor)
                                                  .arg(event->dwSimConnectVersionMinor)
                                                  .arg(event->dwSimConnectBuildMajor)
                                                  .arg(event->dwSimConnectBuildMinor);
            const QString version = QStringLiteral("%1.%2.%3.%4")
                                        .arg(event->dwApplicationVersionMajor)
                                        .arg(event->dwApplicationVersionMinor)
                                        .arg(event->dwApplicationBuildMajor)
                                        .arg(event->dwApplicationBuildMinor);
            const QString name = CSimulatorFsxCommon::fsxCharToQString(event->szApplicationName);
            const QString details =
                QStringLiteral("Name: '%1' Version: %2 SimConnect: %3").arg(name, version, simConnectVersion);
            simulatorFsxP3D->setSimulatorDetails(name, details, version);
            simulatorFsxP3D->m_simConnectVersion = simConnectVersion;
            CLogMessage(simulatorFsxP3D).info(u"Connected to %1: '%2'")
                << simulatorFsxP3D->getSimulatorPluginInfo().getIdentifier() << details;
            simulatorFsxP3D->setSimConnected();
            break; // SIMCONNECT_RECV_ID_OPEN
        }
        case SIMCONNECT_RECV_ID_EXCEPTION:
        {
            if (!simulatorFsxP3D->stillDisplayReceiveExceptions()) { break; }
            simulatorFsxP3D->triggerAutoTraceSendId();

            SIMCONNECT_RECV_EXCEPTION *exception = static_cast<SIMCONNECT_RECV_EXCEPTION *>(pData);
            const DWORD exceptionId = exception->dwException;
            const DWORD sendId = exception->dwSendID;
            const DWORD index = exception->dwIndex; // index of parameter that was source of error,
                                                    // 4294967295/0xFFFFFFFF means unknown, 0 means also UNKNOWN INDEX
            const DWORD data = cbData;
            const TraceFsxSendId trace = simulatorFsxP3D->getSendIdTrace(sendId);
            bool logGenericExceptionInfo = true;

            switch (exceptionId)
            {
            case SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE: break;
            case SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED:
            {
                // MSFS-specific SimVars might fail on P3D.
                // Silently ignore them as they are not accessed.
                logGenericExceptionInfo = false;
                break;
            }
            case SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID:
                break; // Specifies that the client event, request ID, data definition ID, or object ID was not
                       // recognized
            case SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED:
            {
                if (trace.isValid())
                {
                    // it can happen the object is not yet existing
                    CSimConnectObject simObject = simulatorFsxP3D->getSimObjectForTrace(trace);
                    if (simObject.isInvalid()) { simObject = trace.simObject; } // take the one in the trace
                    if (simObject.isValid())
                    {
                        if (simObject.isAircraft())
                        {
                            simulatorFsxP3D->addingAircraftFailed(simObject);
                            logGenericExceptionInfo = false;
                        }
                        else
                        {
                            const bool removed = simulatorFsxP3D->m_simConnectObjects.remove(simObject.getCallsign());
                            Q_UNUSED(removed);
                            CLogMessage(simulatorFsxP3D).warning(u"Adding probe failed: %1 %2")
                                << simObject.getCallsign().asString() << simObject.getAircraftModelString();
                            if (simulatorFsxP3D->isUsingFsxTerrainProbe())
                            {
                                CLogMessage(simulatorFsxP3D).warning(u"Disabling terrain probe");
                                simulatorFsxP3D->setUsingFsxTerrainProbe(false);
                            }
                            logGenericExceptionInfo = false;
                        } // aircraft
                    } // valid
                } // trace
            } // SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED:
            break;
            default: break;
            } // switch exception id

            // generic exception warning
            if (logGenericExceptionInfo)
            {
                QString ex = QString::asprintf("Exception=%lu | SendID=%lu | Index=%lu | cbData=%lu", exceptionId,
                                               sendId, index, data);
                const QString exceptionString(
                    CSimConnectUtilities::simConnectExceptionToString(static_cast<DWORD>(exception->dwException)));
                const QString sendIdDetails = simulatorFsxP3D->getSendIdTraceDetails(sendId);
                CLogMessage(simulatorFsxP3D).warning(u"Caught SimConnect exception: '%1' '%2' | send details: '%3'")
                    << exceptionString << ex << (sendIdDetails.isEmpty() ? "N/A" : sendIdDetails);
            }
            break; // SIMCONNECT_RECV_ID_EXCEPTION
        }
        case SIMCONNECT_RECV_ID_QUIT:
        {
            simulatorFsxP3D->onSimExit();
            break;
        }
        case SIMCONNECT_RECV_ID_EVENT:
        {
            const SIMCONNECT_RECV_EVENT *event = static_cast<SIMCONNECT_RECV_EVENT *>(pData);
            switch (event->uEventID)
            {
            case SystemEventSimStatus:
            {
                const bool running = event->dwData ? true : false;
                if (running) { simulatorFsxP3D->onSimRunning(); }
                else { simulatorFsxP3D->onSimStopped(); }
                break;
            }
            case SystemEventPause:
            {
                const bool paused = event->dwData ? true : false;
                if (simulatorFsxP3D->m_simPaused != paused)
                {
                    simulatorFsxP3D->m_simPaused = paused;
                    simulatorFsxP3D->emitSimulatorCombinedStatus();
                }
                break;
            }
            default: break;
            }
            break; // SIMCONNECT_RECV_ID_EVENT
        }
        case SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE:
        {
            const SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event =
                static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *>(pData);
            const DWORD objectId = event->dwData;
            const SIMCONNECT_SIMOBJECT_TYPE objectType = event->eObjType;
            if (objectType != SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT && objectType != SIMCONNECT_SIMOBJECT_TYPE_HELICOPTER)
            {
                break; // SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE
            }

            // such an object is not necessarily one of ours
            // for instance, I always see object "5" when I start the simulator
            if (simulatorFsxP3D->getSimConnectObjects().isKnownSimObjectId(objectId))
            {
                switch (event->uEventID)
                {
                case SystemEventObjectRemoved: simulatorFsxP3D->simulatorReportedObjectRemoved(objectId); break;
                case SystemEventObjectAdded:
                    // added in SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
                    // this event here is normally received before SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
                    break;
                default: break;
                }
            }
            break; // SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE
        }
        case SIMCONNECT_RECV_ID_EVENT_FRAME:
        {
            const SIMCONNECT_RECV_EVENT_FRAME *event = static_cast<SIMCONNECT_RECV_EVENT_FRAME *>(pData);
            switch (event->uEventID)
            {
            case SystemEventFrame:
                // doing interpolation
                simulatorFsxP3D->onSimFrame();
                break;
            default: break;
            }
            break; // SIMCONNECT_RECV_ID_EVENT_FRAME
        }
        case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
        {
            const SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *event = static_cast<SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *>(pData);
            const DWORD requestId = event->dwRequestID;
            const DWORD objectId = event->dwObjectID;
            simulatorFsxP3D->m_dispatchRequestIdLast = requestId;

            if (CSimulatorFsxCommon::isRequestForSimConnectObject(requestId))
            {
                bool success = simulatorFsxP3D->setSimConnectObjectId(requestId, objectId);
                if (!success) { break; } // not an request ID of ours
                success = simulatorFsxP3D->simulatorReportedObjectAdded(
                    objectId); // adding failed (no IDs), trigger follow up actions
                if (!success)
                {
                    // getting here would mean object was removed in the meantime
                    // otherwise we will detect it in verification
                    const CSimConnectObject simObject = simulatorFsxP3D->getSimObjectForObjectId(objectId);
                    const CSimulatedAircraft remoteAircraft(simObject.getAircraft());
                    const CStatusMessage msg =
                        CStatusMessage(simulatorFsxP3D).error(u"Cannot add object %1, cs: '%2' model: '%3'")
                        << objectId << remoteAircraft.getCallsignAsString() << remoteAircraft.getModelString();
                    CLogMessage::preformatted(msg);
                    emit simulatorFsxP3D->physicallyAddingRemoteModelFailed(remoteAircraft, false, false, msg);
                }
            }
            break; // SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
        }
        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
        {
            // SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *)pData;
            break;
        }
        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
        {
            const SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = static_cast<SIMCONNECT_RECV_SIMOBJECT_DATA *>(pData);
            const DWORD requestId = pObjData->dwRequestID;
            simulatorFsxP3D->m_dispatchRequestIdLast = requestId;

            switch (requestId)
            {
            case CSimConnectDefinitions::RequestOwnAircraft:
            {
                static_assert(sizeof(DataDefinitionOwnAircraft) == 49 * sizeof(double),
                              "DataDefinitionOwnAircraft has an incorrect size.");
                const DataDefinitionOwnAircraft *ownAircaft =
                    reinterpret_cast<const DataDefinitionOwnAircraft *>(&pObjData->dwData);
                simulatorFsxP3D->updateOwnAircraftFromSimulator(*ownAircaft);
                break;
            }
            case CSimConnectDefinitions::RequestOwnAircraftTitle:
            {
                const DataDefinitionOwnAircraftModel *dataDefinitionModel =
                    reinterpret_cast<const DataDefinitionOwnAircraftModel *>(&pObjData->dwData);
                const CAircraftModel model(dataDefinitionModel->title, CAircraftModel::TypeOwnSimulatorModel);
                simulatorFsxP3D->reverseLookupAndUpdateOwnAircraftModel(model);
                break;
            }
            default:
            {
                const DWORD objectId = pObjData->dwObjectID;
                if (CSimulatorFsxCommon::isRequestForSimObjAircraft(requestId))
                {
                    const CSimConnectObject simObject = simulatorFsxP3D->getSimObjectForObjectId(objectId);
                    if (!simObject.hasValidRequestAndObjectId()) { break; }
                    const CSimConnectDefinitions::SimObjectRequest subRequest =
                        CSimulatorFsxCommon::requestToSimObjectRequest(requestId);

                    if (subRequest == CSimConnectDefinitions::SimObjectPositionData)
                    {
                        static_assert(sizeof(DataDefinitionPosData) == 5 * sizeof(double),
                                      "DataDefinitionPosData has an incorrect size.");
                        const DataDefinitionPosData *remoteAircraftSimData =
                            reinterpret_cast<const DataDefinitionPosData *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == simObject.getObjectId())
                        {
                            simulatorFsxP3D->triggerUpdateRemoteAircraftFromSimulator(simObject,
                                                                                      *remoteAircraftSimData);
                        }
                    } // position
                    else if (subRequest == CSimConnectDefinitions::SimObjectModel)
                    {
                        static_assert(sizeof(DataDefinitionRemoteAircraftModel) == sizeof(double) + 168 + 256,
                                      "DataDefinitionRemoteAircraftModel has an incorrect size.");
                        const DataDefinitionRemoteAircraftModel *remoteAircraftModel =
                            reinterpret_cast<const DataDefinitionRemoteAircraftModel *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == simObject.getObjectId())
                        {
                            simulatorFsxP3D->triggerUpdateRemoteAircraftFromSimulator(simObject, *remoteAircraftModel);
                        }
                    } // model
                    else if (subRequest == CSimConnectDefinitions::SimObjectLights)
                    {
                        static_assert(sizeof(DataDefinitionRemoteAircraftLights) == 8 * sizeof(double),
                                      "DataDefinitionRemoteAircraftLights has an incorrect size.");
                        const DataDefinitionRemoteAircraftLights *remoteAircraftLights =
                            reinterpret_cast<const DataDefinitionRemoteAircraftLights *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == simObject.getObjectId())
                        {
                            const CCallsign callsign(simObject.getCallsign());
                            const CAircraftLights lights(remoteAircraftLights->toLights()); // as in simulator
                            simulatorFsxP3D->setCurrentLights(callsign, lights);
                            if (simObject.getLightsAsSent().isNull())
                            {
                                // allows to compare for toggle
                                simulatorFsxP3D->setLightsAsSent(callsign, lights);
                            }
                        }
                        break;
                    } // lights
                    else
                    {
                        if (CBuildConfig::isLocalDeveloperDebugBuild())
                        {
                            CLogMessage(simulatorFsxP3D).error(u"Unknown subrequest (aircraft): '%1' %2")
                                << CSimConnectDefinitions::simObjectRequestToString(subRequest)
                                << simObject.toQString();
                        }
                    }
                }
                else if (CSimulatorFsxCommon::isRequestForSimObjTerrainProbe(requestId))
                {
                    const CSimConnectObject probeObj = simulatorFsxP3D->getSimObjectForObjectId(objectId);
                    if (!probeObj.hasValidRequestAndObjectId()) { break; }
                    Q_ASSERT_X(probeObj.isTerrainProbe(), Q_FUNC_INFO, "No probe");
                    const CSimConnectDefinitions::SimObjectRequest subRequest =
                        CSimulatorFsxCommon::requestToSimObjectRequest(requestId);

                    if (subRequest == CSimConnectDefinitions::SimObjectPositionData)
                    {
                        static_assert(sizeof(DataDefinitionPosData) == 5 * sizeof(double),
                                      "DataDefinitionRemoteAircraftSimData has an incorrect size.");
                        const DataDefinitionPosData *probeSimData =
                            reinterpret_cast<const DataDefinitionPosData *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == probeObj.getObjectId())
                        {
                            const CCallsign cs = simulatorFsxP3D->m_pendingProbeRequests.value(requestId);
                            if (cs.isEmpty()) { break; }
                            simulatorFsxP3D->updateProbeFromSimulator(cs, *probeSimData);
                        }
                    }
                    else
                    {
                        if (CBuildConfig::isLocalDeveloperDebugBuild())
                        {
                            CLogMessage(simulatorFsxP3D).error(u"Unknown subrequest (probe): '%1' %2")
                                << CSimConnectDefinitions::simObjectRequestToString(subRequest) << probeObj.toQString();
                        }
                    }
                } // probe
            }
            break; // default (SIMCONNECT_RECV_ID_SIMOBJECT_DATA)
            }
            break; // SIMCONNECT_RECV_ID_SIMOBJECT_DATA
        }
        case SIMCONNECT_RECV_ID_CLIENT_DATA:
        {
            if (!simulatorFsxP3D->m_useSbOffsets) { break; }
            simulatorFsxP3D->m_sbDataReceived++;
            const SIMCONNECT_RECV_CLIENT_DATA *clientData = static_cast<SIMCONNECT_RECV_CLIENT_DATA *>(pData);
            if (clientData->dwRequestID == CSimConnectDefinitions::RequestSbData)
            {
                //! \fixme FSUIPC vs SimConnect why is offset 19 ident 2/0? In FSUIPC it is 0/1, according to
                //! documentation it is 0/1 but I receive 2/0 here. Whoever knows, add comment or fix if wrong
                DataDefinitionClientAreaSb sbData;
                std::memcpy(&sbData.data, &clientData->dwData, 128);
                simulatorFsxP3D->updateOwnAircraftFromSimulator(sbData);
            }
            break; // SIMCONNECT_RECV_ID_CLIENT_DATA
        }
        case SIMCONNECT_RECV_ID_EVENT_FILENAME:
        {
            const SIMCONNECT_RECV_EVENT_FILENAME *event = static_cast<SIMCONNECT_RECV_EVENT_FILENAME *>(pData);
            switch (event->uEventID)
            {
            case SystemEventFlightLoaded: break;
            default: break;
            }
            break; // SIMCONNECT_RECV_ID_EVENT_FILENAME
        }
        default: simulatorFsxP3D->m_dispatchProcEmptyCount++; break;
        } // main switch

        // performance stats
        const qint64 procTimeEnd = QDateTime::currentMSecsSinceEpoch();
        simulatorFsxP3D->m_dispatchProcTimeMs = procTimeEnd - procTimeStart;
        if (simulatorFsxP3D->m_dispatchProcTimeMs > simulatorFsxP3D->m_dispatchProcMaxTimeMs)
        {
            simulatorFsxP3D->m_dispatchProcMaxTimeMs = simulatorFsxP3D->m_dispatchProcTimeMs;
        }
    } // method
} // namespace swift::simplugin::fsxcommon
