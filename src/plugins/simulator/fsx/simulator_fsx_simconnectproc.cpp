/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_fsx.h"
#include "simconnect_datadefinition.h"
#include "blacksim/fscommon/bcdconversions.h"
#include "blacksim/fsx/simconnectutilities.h"
#include "blacksim/fsx/fsxsimulatorsetup.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/project.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackSim;
using namespace BlackSim::FsCommon;
using namespace BlackSim::Fsx;

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
                    CLogMessage().info(CSimulatorFsx::getMessageCategory(), CProject::systemNameAndVersion());
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION:
                {
                    SIMCONNECT_RECV_EXCEPTION *exception = (SIMCONNECT_RECV_EXCEPTION *)pData;
                    QString ex;
                    ex.sprintf("Exception=%d  SendID=%d  Index=%d  cbData=%d",
                               static_cast<int>(exception->dwException), static_cast<int>(exception->dwSendID),
                               static_cast<int>(exception->dwIndex), static_cast<int>(cbData));
                    qDebug() << "Caught simConnect exception: " << CSimConnectUtilities::simConnectExceptionToString((SIMCONNECT_EXCEPTION)exception->dwException);
                    qDebug() << ex;
                    break;
                }
            case SIMCONNECT_RECV_ID_QUIT:
                {
                    simulatorFsx->onSimExit(); // TODO: What is the difference to sim stopped?
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT:
                {
                    SIMCONNECT_RECV_EVENT *event = static_cast<SIMCONNECT_RECV_EVENT *>(pData);

                    switch (event->uEventID)
                    {
                    case SystemEventSimStatus:
                        {
                            if (event->dwData)
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
                            simulatorFsx->m_simPaused = event->dwData ? true : false;
                            break;
                        }
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE:
                {
                    SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *>(pData);
                    if (event->uEventID == SystemEventObjectAdded)
                    {
                    }
                    else if (event->uEventID == SystemEventObjectRemoved)
                    {
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
                    simulatorFsx->setSimconnectObjectID(event->dwRequestID, event->dwObjectID);
                    break;
                }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
                {
                    SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA *) pData;
                    switch (pObjData->dwRequestID)
                    {
                    case CSimConnectDataDefinition::RequestOwnAircraft:
                        {
                            DataDefinitionOwnAircraft *ownAircaft;
                            ownAircaft = (DataDefinitionOwnAircraft *)&pObjData->dwData;
                            simulatorFsx->updateOwnAircraftFromSim(*ownAircaft);
                            break;
                        }
                    case CSimConnectDataDefinition::RequestOwnAircraftTitle:
                        {
                            DataDefinitionOwnAircraftModel *dataDefinitionModel = (DataDefinitionOwnAircraftModel *) &pObjData->dwData;
                            CAircraftModel model;
                            model.setQueriedModelString(dataDefinitionModel->title);
                            simulatorFsx->setAircraftModel(model);
                            break;
                        }
                    case CSimConnectDataDefinition::RequestSimEnvironment:
                        {
                            DataDefinitionSimEnvironment *simEnv = (DataDefinitionSimEnvironment *) &pObjData->dwData;
                            qint32 zh = simEnv->zuluTimeSeconds / 3600;
                            qint32 zm = (simEnv->zuluTimeSeconds - (zh * 3600)) / 60;
                            CTime zulu(zh, zm);
                            qint32 lh = simEnv->localTimeSeconds / 3600;
                            qint32 lm = (simEnv->localTimeSeconds - (lh * 3600)) / 60;
                            CTime local(lh, lm);
                            simulatorFsx->synchronizeTime(zulu, local);
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
                    const CCoordinateGeodetic posAircraft = simulatorFsx->getOwnAircraft().getPosition();
                    SIMCONNECT_RECV_AIRPORT_LIST *pAirportList = (SIMCONNECT_RECV_AIRPORT_LIST *) pData;
                    for (unsigned i = 0; i < pAirportList->dwArraySize; ++i)
                    {
                        SIMCONNECT_DATA_FACILITY_AIRPORT *pFacilityAirport = pAirportList->rgData + i;
                        if (!pFacilityAirport) break;
                        const QString icao(pFacilityAirport->Icao);
                        if (icao.isEmpty()) continue; // airfield without ICAO code
                        if (!CAirportIcao::isValidIcaoDesignator(icao)) continue; // tiny airfields in SIM
                        CCoordinateGeodetic pos(pFacilityAirport->Latitude, pFacilityAirport->Longitude, pFacilityAirport->Altitude);
                        CAirport airport(CAirportIcao(icao), pos);
                        CLength d = airport.calculcateDistanceAndBearingToPlane(posAircraft);
                        if (d > maxDistance) continue;
                        simulatorFsx->m_airportsInRange.replaceOrAddByIcao(airport);
                    }
                    if (simulatorFsx->m_airportsInRange.size() > 20)
                    {
                        simulatorFsx->m_airportsInRange.removeIfOutsideRange(posAircraft, maxDistance, true);
                    }
                    break;
                }
            default:
                break;
            } // main switch
        } // method
    }
}
