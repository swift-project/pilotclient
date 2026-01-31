// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <strsafe.h>

#include <cstring>

#include "simconnectdatadefinitionmsfs2024.h"
#include "simulatormsfs2024common.h"

#include "config/buildconfig.h"
#include "core/application.h"
#include "core/simulator.h"
#include "misc/aviation/airportlist.h"
#include "misc/logmessage.h"
#include "misc/simulation/fscommon/bcdconversions.h"
#include "misc/simulation/fsx/simconnectutilities.h"
#include "misc/simulation/settings/simulatorsettings.h"
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
using namespace swift::misc::simulation::settings;

namespace swift::simplugin::msfs2024common
{
    void CALLBACK CSimulatorMsfs2024::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
    {
        // IMPORTANT:
        // all tasks called in this function (ie all called functions) must perform fast or shall be called
        // asynchronously

        const qint64 procTimeStart = QDateTime::currentMSecsSinceEpoch();
        CSimulatorMsfs2024 *simulatorMsfs2024 = static_cast<CSimulatorMsfs2024 *>(pContext);
        const SIMCONNECT_RECV_ID recvId = static_cast<SIMCONNECT_RECV_ID>(pData->dwID);
        static const DataDefinitionOwnAircraftModel *dataDefinitionModel;
        simulatorMsfs2024->m_dispatchReceiveIdLast = recvId;
        simulatorMsfs2024->m_dispatchProcCount++;
        const CSpecializedSimulatorSettings settings = simulatorMsfs2024->getSimulatorSettings();
        CSimulatorSettings m_generic = settings.getGenericSettings();

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
            const QString name = CSimulatorMsfs2024::fsxCharToQString(event->szApplicationName);
            const QString details =
                QStringLiteral("Name: '%1' Version: %2 SimConnect: %3").arg(name, version, simConnectVersion);
            simulatorMsfs2024->setSimulatorDetails(name, details, version);
            simulatorMsfs2024->m_simConnectVersion = simConnectVersion;
            CLogMessage(simulatorMsfs2024).info(u"Connected to %1: '%2'")
                << simulatorMsfs2024->getSimulatorPluginInfo().getIdentifier() << details;
            simulatorMsfs2024->setSimConnected();
            break; // SIMCONNECT_RECV_ID_OPEN
        }
        case SIMCONNECT_RECV_ID_EXCEPTION:
        {
            if (!simulatorMsfs2024->stillDisplayReceiveExceptions()) { break; }
            simulatorMsfs2024->triggerAutoTraceSendId();

            SIMCONNECT_RECV_EXCEPTION *exception = static_cast<SIMCONNECT_RECV_EXCEPTION *>(pData);
            const DWORD exceptionId = exception->dwException;
            const DWORD sendId = exception->dwSendID;
            const DWORD index = exception->dwIndex; // index of parameter that was source of error,
                                                    // 4294967295/0xFFFFFFFF means unknown, 0 means also UNKNOWN INDEX
            const DWORD data = cbData;
            const TraceFsxSendId trace = simulatorMsfs2024->getSendIdTrace(sendId);
            bool logGenericExceptionInfo = false;

            switch (exceptionId)
            {
            case SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE: break;
            case SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID:
                break; // Specifies that the client event, request ID, data definition ID, or object ID was not
                       // recognized
            case SIMCONNECT_EXCEPTION_DATA_ERROR:
                logGenericExceptionInfo = true;
                break; // data error, can happen during data request
            case SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED:
            {
                if (trace.isValid())
                {
                    // it can happen the object is not yet existing
                    CSimConnectObject simObject = simulatorMsfs2024->getSimObjectForTrace(trace);
                    if (simObject.isInvalid()) { simObject = trace.simObject; } // take the one in the trace
                    if (simObject.isValid())
                    {
                        if (simObject.isAircraft())
                        {
                            CLogMessage(simulatorMsfs2024).warning(u"Adding Aircraft failed: %1 %2")
                                << simObject.getCallsign().asString() << simObject.getAircraftModelString();
                            simulatorMsfs2024->addingAircraftFailed(simObject);
                            logGenericExceptionInfo = false;
                        }
                        else
                        {
                            const bool removed = simulatorMsfs2024->m_simConnectObjects.remove(simObject.getCallsign());
                            Q_UNUSED(removed);
                            CLogMessage(simulatorMsfs2024).warning(u"Adding probe failed: %1 %2")
                                << simObject.getCallsign().asString() << simObject.getAircraftModelString();
                            logGenericExceptionInfo = false;
                        } // aircraft
                    } // valid
                } // trace
            } // SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED:
            break;
            default: logGenericExceptionInfo = true; break;
            } // switch exception id

            // generic exception warning
            if (logGenericExceptionInfo)
            {
                QString ex = QString::asprintf("Exception=%lu | SendID=%lu | Index=%lu | cbData=%lu", exceptionId,
                                               sendId, index, data);
                const QString exceptionString(
                    CSimConnectUtilities::simConnectExceptionToString(static_cast<DWORD>(exception->dwException)));
                const QString sendIdDetails = simulatorMsfs2024->getSendIdTraceDetails(sendId);
                CLogMessage(simulatorMsfs2024).warning(u"Caught simConnect exception: '%1' '%2' | send details: '%3'")
                    << exceptionString << ex << (sendIdDetails.isEmpty() ? "N/A" : sendIdDetails);
            }
            break; // SIMCONNECT_RECV_ID_EXCEPTION
        }
        case SIMCONNECT_RECV_ID_QUIT:
        {
            simulatorMsfs2024->onSimExit();
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
                if (running) { simulatorMsfs2024->onSimRunning(); }
                else { simulatorMsfs2024->onSimStopped(); }

                // If the simulation stops, the model will be reloaded when it is restarted.
                dataDefinitionModel = NULL;

                break;
            }
            case SystemEventPause:
            {
                const bool paused = event->dwData ? true : false;
                if (simulatorMsfs2024->m_simPaused != paused)
                {
                    simulatorMsfs2024->m_simPaused = paused;
                    simulatorMsfs2024->emitSimulatorCombinedStatus();
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
            if (simulatorMsfs2024->getSimConnectObjects().isKnownSimObjectId(objectId))
            {
                switch (event->uEventID)
                {
                case SystemEventObjectRemoved: simulatorMsfs2024->simulatorReportedObjectRemoved(objectId); break;
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
                simulatorMsfs2024->onSimFrame();
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

            simulatorMsfs2024->m_dispatchRequestIdLast = requestId;

            if (CSimulatorMsfs2024::isRequestForSimConnectObject(requestId))
            {
                bool success = simulatorMsfs2024->setSimConnectObjectId(requestId, objectId);
                if (!success) { break; } // not an request ID of ours
                success = simulatorMsfs2024->simulatorReportedObjectAdded(
                    objectId); // adding failed (no IDs), trigger follow up actions
                if (!success)
                {
                    // getting here would mean object was removed in the meantime
                    // otherwise we will detect it in verification
                    const CSimConnectObject simObject = simulatorMsfs2024->getSimObjectForObjectId(objectId);
                    const CSimulatedAircraft remoteAircraft(simObject.getAircraft());
                    const CStatusMessage msg =
                        CStatusMessage(simulatorMsfs2024).error(u"Cannot add object %1, cs: '%2' model: '%3'")
                        << objectId << remoteAircraft.getCallsignAsString() << remoteAircraft.getModelString();
                    CLogMessage::preformatted(msg);
                    emit simulatorMsfs2024->physicallyAddingRemoteModelFailed(remoteAircraft, false, false, msg);
                }
            }
            break; // SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
        }
        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
        {
            const SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData =
                static_cast<SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *>(pData);
            const DWORD requestId = pObjData->dwRequestID;

            switch (pObjData->dwRequestID)
            {

            case 0:
            {
                break;
            }
            case 1:
            {
                dataDefinitionModel = reinterpret_cast<const DataDefinitionOwnAircraftModel *>(&pObjData->dwData);
                if (dataDefinitionModel->title != NULL && dataDefinitionModel->livery != NULL)
                {
                    CAircraftModel model(dataDefinitionModel->title, dataDefinitionModel->livery,
                                         CAircraftModel::TypeOwnSimulatorModel);

                    simulatorMsfs2024->reverseLookupAndUpdateOwnAircraftModel(model.getMsfs2024ModelString());
                }
                break;
            }

            default: printf("Unhandled SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE received: %d\n", requestId); break;
            }
            break;

            break;
        }
        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
        {
            const SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = static_cast<SIMCONNECT_RECV_SIMOBJECT_DATA *>(pData);
            const DWORD requestId = pObjData->dwRequestID;
            simulatorMsfs2024->m_dispatchRequestIdLast = requestId;

            switch (requestId)
            {
            case CSimConnectDefinitions::RequestOwnAircraft:
            {
                static_assert(sizeof(DataDefinitionOwnAircraft) == 60 * sizeof(double),
                              "DataDefinitionOwnAircraft has an incorrect size.");
                const DataDefinitionOwnAircraft *ownAircaft =
                    reinterpret_cast<const DataDefinitionOwnAircraft *>(&pObjData->dwData);
                simulatorMsfs2024->updateOwnAircraftFromSimulator(*ownAircaft);
                break;
            }
            case CSimConnectDefinitions::RequestMSFSTransponder:
            {
                const DataDefinitionMSFSTransponderMode *transponderMode =
                    reinterpret_cast<const DataDefinitionMSFSTransponderMode *>(&pObjData->dwData);
                simulatorMsfs2024->updateMSFS2024TransponderMode(*transponderMode);
                break;
            }
            default:
            {
                const DWORD objectId = pObjData->dwObjectID;
                if (CSimulatorMsfs2024::isRequestForSimObjAircraft(requestId))
                {
                    const CSimConnectObject simObject = simulatorMsfs2024->getSimObjectForObjectId(objectId);
                    if (!simObject.hasValidRequestAndObjectId()) { break; }
                    const CSimConnectDefinitions::SimObjectRequest subRequest =
                        CSimulatorMsfs2024::requestToSimObjectRequest(requestId);

                    if (subRequest == CSimConnectDefinitions::SimObjectPositionData)
                    {
                        static_assert(sizeof(DataDefinitionPosData) == 5 * sizeof(double),
                                      "DataDefinitionPosData has an incorrect size.");
                        const DataDefinitionPosData *remoteAircraftSimData =
                            reinterpret_cast<const DataDefinitionPosData *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == simObject.getObjectId())
                        {
                            simulatorMsfs2024->triggerUpdateRemoteAircraftFromSimulator(simObject,
                                                                                        *remoteAircraftSimData);
                        }
                    } // position
                    else if (subRequest == CSimConnectDefinitions::SimObjectModel)
                    {
                        static_assert(sizeof(DataDefinitionRemoteAircraftModel) == sizeof(double) + 168 + 256 + 256,
                                      "DataDefinitionRemoteAircraftModel has an incorrect size.");
                        const DataDefinitionRemoteAircraftModel *remoteAircraftModel =
                            reinterpret_cast<const DataDefinitionRemoteAircraftModel *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == simObject.getObjectId())
                        {
                            simulatorMsfs2024->triggerUpdateRemoteAircraftFromSimulator(simObject,
                                                                                        *remoteAircraftModel);
                        }
                    } // model
                    else if (subRequest == CSimConnectDefinitions::SimObjectLights)
                    {
                        static_assert(sizeof(DataDefinitionRemoteAircraftLights) == 9 * sizeof(double),
                                      "DataDefinitionRemoteAircraftLights has an incorrect size.");
                        const DataDefinitionRemoteAircraftLights *remoteAircraftLights =
                            reinterpret_cast<const DataDefinitionRemoteAircraftLights *>(&pObjData->dwData);
                        // extra check, but ids should be the same
                        if (objectId == simObject.getObjectId())
                        {
                            const CCallsign callsign(simObject.getCallsign());
                            const CAircraftLights lights(remoteAircraftLights->toLights()); // as in simulator
                            simulatorMsfs2024->setCurrentLights(callsign, lights);
                            if (simObject.getLightsAsSent().isNull())
                            {
                                // allows to compare for toggle
                                simulatorMsfs2024->setLightsAsSent(callsign, lights);
                            }
                        }
                        break;
                    } // lights
                    else
                    {
                        if (CBuildConfig::isLocalDeveloperDebugBuild())
                        {
                            CLogMessage(simulatorMsfs2024).error(u"Unknown subrequest (aircraft): '%1' %2")
                                << CSimConnectDefinitions::simObjectRequestToString(subRequest)
                                << simObject.toQString();
                        }
                    }
                }
            }
            break; // default (SIMCONNECT_RECV_ID_SIMOBJECT_DATA)
            }
            break; // SIMCONNECT_RECV_ID_SIMOBJECT_DATA
        }
        case SIMCONNECT_RECV_ID_ENUMERATE_SIMOBJECT_AND_LIVERY_LIST: // 38
        {
            if (m_generic.getPropertyModelSet())
            {
                SIMCONNECT_RECV_ENUMERATE_SIMOBJECT_AND_LIVERY_LIST *msg =
                    (SIMCONNECT_RECV_ENUMERATE_SIMOBJECT_AND_LIVERY_LIST *)pData;
                switch (msg->dwRequestID)
                {
                case CSimConnectDefinitions::REQUEST_AIRPLANE:
                case CSimConnectDefinitions::REQUEST_HELICOPTER:
                case CSimConnectDefinitions::REQUEST_HOT_AIR: simulatorMsfs2024->CacheSimObjectAndLiveries(msg); break;
                }
            }

            break;
        }
        case SIMCONNECT_RECV_ID_CLIENT_DATA:
        {
            if (!simulatorMsfs2024->m_useSbOffsets) { break; }
            simulatorMsfs2024->m_sbDataReceived++;
            const SIMCONNECT_RECV_CLIENT_DATA *clientData = static_cast<SIMCONNECT_RECV_CLIENT_DATA *>(pData);
            if (clientData->dwRequestID == CSimConnectDefinitions::RequestSbData)
            {
                //! \fixme FSUIPC vs SimConnect why is offset 19 ident 2/0? In FSUIPC it is 0/1, according to
                //! documentation it is 0/1 but I receive 2/0 here. Whoever knows, add comment or fix if wrong
                DataDefinitionClientAreaSb sbData;
                std::memcpy(&sbData.data, &clientData->dwData, 128);
                simulatorMsfs2024->updateOwnAircraftFromSimulator(sbData);
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
        default: simulatorMsfs2024->m_dispatchProcEmptyCount++; break;
        } // main switch

        // performance stats
        const qint64 procTimeEnd = QDateTime::currentMSecsSinceEpoch();
        simulatorMsfs2024->m_dispatchProcTimeMs = procTimeEnd - procTimeStart;
        if (simulatorMsfs2024->m_dispatchProcTimeMs > simulatorMsfs2024->m_dispatchProcMaxTimeMs)
        {
            simulatorMsfs2024->m_dispatchProcMaxTimeMs = simulatorMsfs2024->m_dispatchProcTimeMs;
        }
    } // method
} // namespace swift::simplugin::msfs2024common
