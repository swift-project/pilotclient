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
#include "simconnectdatadefinition.h"
#include "blackmisc/simulation/fscommon/bcdconversions.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/logmessage.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::Fsx;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        void CALLBACK CSimulatorFsxCommon::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
        {
            CSimulatorFsxCommon *simulatorFsx = static_cast<CSimulatorFsxCommon *>(pContext);
            switch (pData->dwID)
            {
            case SIMCONNECT_RECV_ID_OPEN:
                {
                    SIMCONNECT_RECV_OPEN *event = (SIMCONNECT_RECV_OPEN *)pData;
                    simulatorFsx->m_simulatorVersion = QString("%1.%2.%3.%4").arg(event->dwApplicationVersionMajor).arg(event->dwApplicationVersionMinor).arg(event->dwApplicationBuildMajor).arg(event->dwApplicationBuildMinor);
                    simulatorFsx->m_simConnectVersion = QString("%1.%2.%3.%4").arg(event->dwSimConnectVersionMajor).arg(event->dwSimConnectVersionMinor).arg(event->dwSimConnectBuildMajor).arg(event->dwSimConnectBuildMinor);
                    simulatorFsx->m_simulatorName = QString(event->szApplicationName);
                    simulatorFsx->m_simulatorDetails = QString("Name: '%1' Version: %2 SimConnect: %3").arg(simulatorFsx->m_simulatorName, simulatorFsx->m_simulatorVersion, simulatorFsx->m_simConnectVersion);
                    CLogMessage(static_cast<CSimulatorFsxCommon *>(nullptr)).info("Connect to FSX/P3D: '%1'") << simulatorFsx->m_simulatorDetails;
                    simulatorFsx->setSimConnected();
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION:
                {
                    if (!simulatorFsx->stillDisplayReceiveExceptions()) { break; }
                    SIMCONNECT_RECV_EXCEPTION *exception = (SIMCONNECT_RECV_EXCEPTION *)pData;
                    const DWORD exceptionId = exception->dwException;
                    const DWORD sendId = exception->dwSendID;
                    const DWORD index = exception->dwIndex;
                    const DWORD data = cbData;
                    const QString exStr(CSimConnectUtilities::simConnectExceptionToString((SIMCONNECT_EXCEPTION)exception->dwException));
                    QString ex;
                    ex.sprintf("Exception=%lu | SendID=%lu | Index=%lu | cbData=%lu", exceptionId, sendId, index, data);
                    switch (exceptionId)
                    {
                    case SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE: break;
                    case SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID: break;
                    default: break;
                    }
                    CLogMessage(simulatorFsx).warning("Caught FSX simConnect exception: %1 %2") << exStr << ex;
                    break;
                }
            case SIMCONNECT_RECV_ID_QUIT:
                {
                    simulatorFsx->onSimExit();
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT:
                {
                    SIMCONNECT_RECV_EVENT *event = static_cast<SIMCONNECT_RECV_EVENT *>(pData);
                    switch (event->uEventID)
                    {
                    case SystemEventSimStatus:
                        {
                            const bool running = event->dwData ? true : false;
                            if (running)
                            {
                                simulatorFsx->onSimRunning();
                            }
                            else
                            {
                                simulatorFsx->onSimStopped();
                            }
                            break;
                        }
                    case SystemEventPause:
                        {
                            const bool p = event->dwData ? true : false;
                            if (simulatorFsx->m_simPaused != p)
                            {
                                simulatorFsx->m_simPaused = p;
                                simulatorFsx->emitSimulatorCombinedStatus();
                            }
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE:
                {
                    const SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *>(pData);
                    const DWORD objectId = event->dwData;
                    const SIMCONNECT_SIMOBJECT_TYPE objectType = event->eObjType;
                    if (objectType != SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT && objectType != SIMCONNECT_SIMOBJECT_TYPE_HELICOPTER)
                    {
                        break;
                    }

                    // such an object is not necessarily one of ours
                    // for instance, I always see object "5" when I start the simulator
                    if (!simulatorFsx->getSimConnectObjects().isKnownSimObjectId(objectId)) break;
                    switch (event->uEventID)
                    {
                    case SystemEventObjectAdded:
                        // added in SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
                        break;
                    case SystemEventObjectRemoved:
                        simulatorFsx->simulatorReportedObjectRemoved(objectId);
                        break;
                    default:
                        break;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_FRAME:
                {
                    SIMCONNECT_RECV_EVENT_FRAME *event = (SIMCONNECT_RECV_EVENT_FRAME *) pData;
                    switch (event->uEventID)
                    {
                    case SystemEventFrame:
                        // doing interpolation
                        simulatorFsx->onSimFrame();
                        break;
                    default:
                        break;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
                {
                    SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *event = static_cast<SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *>(pData);
                    const DWORD requestId = event->dwRequestID;
                    const DWORD objectId = event->dwObjectID;
                    bool success = simulatorFsx->setSimConnectObjectId(requestId, objectId);
                    if (!success) { break; } // not an request ID of ours

                    success = simulatorFsx->simulatorReportedObjectAdded(objectId); // trigger follow up actions
                    if (!success)
                    {
                        const CSimulatedAircraft remoteAircraft(simulatorFsx->getSimConnectObjects().getSimObjectForObjectId(objectId).getAircraft());
                        const CStatusMessage msg = CStatusMessage(simulatorFsx).error("Cannot add object %1, cs: '%2' model: '%3'") << objectId << remoteAircraft.getCallsignAsString() << remoteAircraft.getModelString();
                        CLogMessage::preformatted(msg);
                        emit simulatorFsx->physicallyAddingRemoteModelFailed(remoteAircraft, msg);
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
                {
                    // SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *)pData;
                    break;
                }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
                {
                    SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA *) pData;
                    const DWORD requestId = pObjData->dwRequestID;
                    switch (requestId)
                    {
                    case CSimConnectDefinitions::RequestOwnAircraft:
                        {
                            static_assert(sizeof(DataDefinitionOwnAircraft) == 30 * sizeof(double), "DataDefinitionOwnAircraft has an incorrect size.");
                            const DataDefinitionOwnAircraft *ownAircaft = (DataDefinitionOwnAircraft *)&pObjData->dwData;
                            simulatorFsx->updateOwnAircraftFromSimulator(*ownAircaft);
                            break;
                        }
                    case CSimConnectDefinitions::RequestOwnAircraftTitle:
                        {
                            const DataDefinitionOwnAircraftModel *dataDefinitionModel = (DataDefinitionOwnAircraftModel *) &pObjData->dwData;
                            CAircraftModel model;
                            model.setModelString(dataDefinitionModel->title);
                            model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                            simulatorFsx->reverseLookupAndUpdateOwnAircraftModel(model);
                            break;
                        }
                    case CSimConnectDefinitions::RequestSimEnvironment:
                        {
                            const DataDefinitionSimEnvironment *simEnv = (DataDefinitionSimEnvironment *) &pObjData->dwData;
                            if (simulatorFsx->isTimeSynchronized())
                            {
                                const int zh = simEnv->zuluTimeSeconds / 3600;
                                const int zm = (simEnv->zuluTimeSeconds - (zh * 3600)) / 60;
                                const CTime zulu(zh, zm);
                                const int lh = simEnv->localTimeSeconds / 3600;
                                const int lm = (simEnv->localTimeSeconds - (lh * 3600)) / 60;
                                const CTime local(lh, lm);
                                simulatorFsx->synchronizeTime(zulu, local);
                            }
                            break;
                        }
                    default:
                        {
                            const DWORD objectId = pObjData->dwObjectID;
                            if (isRequestForSimData(requestId))
                            {
                                static_assert(sizeof(DataDefinitionRemoteAircraftSimData) == 5 * sizeof(double), "DataDefinitionRemoteAircraftSimData has an incorrect size.");
                                const CSimConnectObject simObj = simulatorFsx->getSimConnectObjects().getSimObjectForObjectId(objectId);
                                if (!simObj.hasValidRequestAndObjectId()) break;
                                const DataDefinitionRemoteAircraftSimData *remoteAircraftSimData = (DataDefinitionRemoteAircraftSimData *)&pObjData->dwData;
                                // extra check, but ids should be the same
                                if (objectId == simObj.getObjectId())
                                {
                                    simulatorFsx->updateRemoteAircraftFromSimulator(simObj, *remoteAircraftSimData);
                                }
                            }
                            else if (isRequestForLights(requestId))
                            {
                                static_assert(sizeof(DataDefinitionRemoteAircraftLights) == 8 * sizeof(double), "DataDefinitionRemoteAircraftLights has an incorrect size.");
                                const CSimConnectObject simObj = simulatorFsx->getSimConnectObjects().getSimObjectForObjectId(objectId);
                                if (!simObj.hasValidRequestAndObjectId()) break;
                                const DataDefinitionRemoteAircraftLights *remoteAircraftLights = (DataDefinitionRemoteAircraftLights *)&pObjData->dwData;
                                // extra check, but ids should be the same
                                if (objectId == simObj.getObjectId())
                                {
                                    const CCallsign callsign(simObj.getCallsign());
                                    const CAircraftLights lights = remoteAircraftLights->toLights(); // as in simulator
                                    simulatorFsx->setCurrentLights(callsign, lights);
                                    if (simObj.getLightsAsSent().isNull())
                                    {
                                        // allows to compare for toggle
                                        simulatorFsx->setLightsAsSent(callsign, lights);
                                    }
                                }
                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_AIRPORT_LIST:
                {
                    static const CLength maxDistance(200.0, CLengthUnit::NM());
                    const CCoordinateGeodetic posAircraft(simulatorFsx->getOwnAircraftPosition());
                    SIMCONNECT_RECV_AIRPORT_LIST *pAirportList = (SIMCONNECT_RECV_AIRPORT_LIST *) pData;
                    for (unsigned i = 0; i < pAirportList->dwArraySize; ++i)
                    {
                        SIMCONNECT_DATA_FACILITY_AIRPORT *pFacilityAirport = pAirportList->rgData + i;
                        if (!pFacilityAirport) { break; }
                        const QString icao(pFacilityAirport->Icao);
                        if (icao.isEmpty()) { continue; } // airfield without ICAO code
                        if (!CAirportIcaoCode::isValidIcaoDesignator(icao)) { continue; } // tiny airfields/strips in simulator
                        if (CAirportIcaoCode::containsNumbers(icao)) { continue; } // tiny airfields/strips in simulator
                        const CCoordinateGeodetic pos(pFacilityAirport->Latitude, pFacilityAirport->Longitude, pFacilityAirport->Altitude);
                        CAirport airport(CAirportIcaoCode(icao), pos);
                        const CLength d = airport.calculcateAndUpdateRelativeDistanceAndBearing(posAircraft);
                        if (d > maxDistance) { continue; }
                        airport.updateMissingParts(simulatorFsx->getWebServiceAirport(icao));
                        simulatorFsx->m_airportsInRangeFromSimulator.replaceOrAddByIcao(airport);
                    }

                    if (simulatorFsx->m_airportsInRangeFromSimulator.size() > simulatorFsx->maxAirportsInRange())
                    {
                        simulatorFsx->m_airportsInRangeFromSimulator.sortByDistanceToOwnAircraft();
                        simulatorFsx->m_airportsInRangeFromSimulator.truncate(simulatorFsx->maxAirportsInRange());
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_CLIENT_DATA:
                {
                    if (!simulatorFsx->m_useSbOffsets) { break; }
                    SIMCONNECT_RECV_CLIENT_DATA *clientData = (SIMCONNECT_RECV_CLIENT_DATA *)pData;
                    if (simulatorFsx->m_useSbOffsets && clientData->dwRequestID == CSimConnectDefinitions::RequestSbData)
                    {
                        //! \fixme FSUIPC vs SimConnect why is offset 19 ident 2/0? In FSUIPC it is 0/1, according to documentation it is 0/1 but I receive 2/0 here. Whoever knows, add comment or fix if wrong
                        DataDefinitionClientAreaSb *sbData = (DataDefinitionClientAreaSb *) &clientData->dwData;
                        simulatorFsx->updateOwnAircraftFromSimulator(*sbData);
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_FILENAME:
                {
                    SIMCONNECT_RECV_EVENT_FILENAME *event = static_cast<SIMCONNECT_RECV_EVENT_FILENAME *>(pData);
                    switch (event->uEventID)
                    {
                    case SystemEventFlightLoaded: break;
                    default: break;
                    }
                    break;
                }
            default:
                break;
            } // main switch
        } // method
    } // namespace
} // namespace
