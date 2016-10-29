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
#include "simconnectdatadefinition.h"
#include "blackmisc/simulation/fscommon/bcdconversions.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/fsx/fsxsimulatorsetup.h"
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
    namespace Fsx
    {
        void CALLBACK CSimulatorFsx::SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext)
        {
            CSimulatorFsx *simulatorFsx = static_cast<CSimulatorFsx *>(pContext);
            switch (pData->dwID)
            {
            case SIMCONNECT_RECV_ID_OPEN:
                {
                    SIMCONNECT_RECV_OPEN *event = (SIMCONNECT_RECV_OPEN *)pData;
                    simulatorFsx->simulatorDetails = QString("Open: AppName=\"%1\"  AppVersion=%2.%3.%4.%5  SimConnectVersion=%6.%7.%8.%9")
                                                     .arg(event->szApplicationName)
                                                     .arg(event->dwApplicationVersionMajor).arg(event->dwApplicationVersionMinor).arg(event->dwApplicationBuildMajor).arg(event->dwApplicationBuildMinor)
                                                     .arg(event->dwSimConnectVersionMajor).arg(event->dwSimConnectVersionMinor).arg(event->dwSimConnectBuildMajor).arg(event->dwSimConnectBuildMinor);
                    CLogMessage(static_cast<CSimulatorFsx *>(nullptr)).info("Connect to FSX: %1") << sApp->swiftVersionString();
                    simulatorFsx->setSimConnected();
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION:
                {
                    SIMCONNECT_RECV_EXCEPTION *exception = (SIMCONNECT_RECV_EXCEPTION *)pData;
                    QString ex;
                    const int exceptionId = static_cast<int>(exception->dwException);
                    const int sendId = static_cast<int>(exception->dwSendID);
                    const int index = static_cast<int>(exception->dwIndex);
                    const int data = static_cast<int>(cbData);
                    ex.sprintf("Exception=%d  SendID=%d  Index=%d  cbData=%d", exceptionId, sendId, index, data);
                    if (exceptionId == SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE)
                    {
                        // means we have problems with an AI aircraft
                        //! \todo find out how to obtain the id here so I can get callsign
                        CCallsign cs = simulatorFsx->getCallsignForObjectId(data);
                        if (!cs.isEmpty())
                        {
                            ex += " callsign: ";
                            ex += cs.toQString();
                        }
                    }
                    CLogMessage(simulatorFsx).error("Caught FSX simConnect exception: %1 %2")
                            << CSimConnectUtilities::simConnectExceptionToString((SIMCONNECT_EXCEPTION)exception->dwException)
                            << ex;
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
                            bool running = event->dwData ? true : false;
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
                            bool p = event->dwData ? true : false;
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
                    SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *>(pData);
                    switch (event->uEventID)
                    {
                    case SystemEventObjectAdded:
                        break;
                    case SystemEventObjectRemoved:
                        break;
                    default:
                        break;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_FRAME:
                {
                    SIMCONNECT_RECV_EVENT_FRAME  *event = (SIMCONNECT_RECV_EVENT_FRAME *) pData;
                    switch (event->uEventID)
                    {
                    case SystemEventFrame:
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
                    const DWORD requestID = event->dwRequestID;
                    const DWORD objectID = event->dwObjectID;
                    const bool success = simulatorFsx->aiAircraftWasAddedInSimulator(requestID, objectID);
                    if (!success)
                    {
                        CLogMessage(simulatorFsx).warning("Cannot find CSimConnectObject for request %1") << requestID;
                        const CSimulatedAircraft remoteAircraft(simulatorFsx->getSimObjectForObjectId(objectID).getAircraft());
                        const QString msg("Object id for request " + QString::number(requestID) + " not avialable");
                        emit simulatorFsx->physicallyAddingRemoteModelFailed(remoteAircraft, CStatusMessage(simulatorFsx, CStatusMessage::SeverityError, msg));
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
                {
                    SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA *) pData;
                    switch (pObjData->dwRequestID)
                    {
                    case CSimConnectDefinitions::RequestOwnAircraft:
                        {
                            static_assert(sizeof(DataDefinitionOwnAircraft) == 27 * sizeof(double), "DataDefinitionOwnAircraft has an incorrect size.");
                            DataDefinitionOwnAircraft *ownAircaft;
                            ownAircaft = (DataDefinitionOwnAircraft *)&pObjData->dwData;
                            simulatorFsx->updateOwnAircraftFromSimulator(*ownAircaft);
                            break;
                        }
                    case CSimConnectDefinitions::RequestOwnAircraftTitle:
                        {
                            DataDefinitionOwnAircraftModel *dataDefinitionModel = (DataDefinitionOwnAircraftModel *) &pObjData->dwData;
                            CAircraftModel model;
                            model.setModelString(dataDefinitionModel->title);
                            model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                            simulatorFsx->reverseLookupAndUpdateOwnAircraftModel(model);
                            break;
                        }
                    case CSimConnectDefinitions::RequestSimEnvironment:
                        {
                            DataDefinitionSimEnvironment *simEnv = (DataDefinitionSimEnvironment *) &pObjData->dwData;
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
                        break;
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_AIRPORT_LIST:
                {
                    const CLength maxDistance(200.0, CLengthUnit::NM());
                    const CCoordinateGeodetic posAircraft(simulatorFsx->getOwnAircraftPosition());
                    SIMCONNECT_RECV_AIRPORT_LIST *pAirportList = (SIMCONNECT_RECV_AIRPORT_LIST *) pData;
                    for (unsigned i = 0; i < pAirportList->dwArraySize; ++i)
                    {
                        SIMCONNECT_DATA_FACILITY_AIRPORT *pFacilityAirport = pAirportList->rgData + i;
                        if (!pFacilityAirport) { break; }
                        const QString icao(pFacilityAirport->Icao);
                        if (icao.isEmpty()) { continue; } // airfield without ICAO code
                        if (!CAirportIcaoCode::isValidIcaoDesignator(icao)) { continue; } // tiny airfields in SIM
                        const CCoordinateGeodetic pos(pFacilityAirport->Latitude, pFacilityAirport->Longitude, pFacilityAirport->Altitude);
                        CAirport airport(CAirportIcaoCode(icao), pos);
                        const CLength d = airport.calculcateAndUpdateRelativeDistanceAndBearing(posAircraft);
                        if (d > maxDistance) { continue; }
                        simulatorFsx->m_airportsInRange.replaceOrAddByIcao(airport);
                    }
                    if (simulatorFsx->m_airportsInRange.size() > 20)
                    {
                        simulatorFsx->m_airportsInRange.removeIfOutsideRange(posAircraft, maxDistance, true);
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_CLIENT_DATA:
                {
                    if (!simulatorFsx->m_useSbOffsets) { break; }
                    SIMCONNECT_RECV_CLIENT_DATA *clientData = (SIMCONNECT_RECV_CLIENT_DATA *)pData;
                    if (simulatorFsx->m_useSbOffsets && clientData->dwRequestID == CSimConnectDefinitions::RequestSbData)
                    {
                        //! \todo why is offset 19 ident 2/0 ?
                        //! In FSUIPC it is 0/1, according to documentation it is 0/1 but I receive 2/0 here
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
                    case SystemEventFlightLoaded:
                        break;
                    default:
                        break;
                    }
                    break;
                }
            default:
                break;

            } // main switch
        } // method
    } // namespace
} // namespace
