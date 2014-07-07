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
        void CALLBACK CSimulatorFsx::SimConnectProc(SIMCONNECT_RECV *pData, DWORD /* cbData */, void *pContext)
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
                    simulatorFsx->displayStatusMessage(CStatusMessage::getInfoMessage(CProject::systemNameAndVersion()));
                    break;
                }
            case SIMCONNECT_RECV_ID_EXCEPTION:
                {
                    SIMCONNECT_RECV_EXCEPTION *event = (SIMCONNECT_RECV_EXCEPTION *)pData;
                    qDebug() << "Caught simConnect exception: " << CSimConnectUtilities::simConnectExceptionToString((SIMCONNECT_EXCEPTION)event->dwException);
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
                    case EventSimStatus:
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
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE:
                {
                    SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *>(pData);
                    if (event->uEventID == EventObjectAdded)
                    {
                    }
                    else if (event->uEventID == EventObjectRemoved)
                    {
                    }
                    break;
                }
            case SIMCONNECT_RECV_ID_EVENT_FRAME:
                {
                    SIMCONNECT_RECV_EVENT_FRAME  *event = (SIMCONNECT_RECV_EVENT_FRAME *) pData;
                    switch (event->uEventID)
                    {
                    case EventFrame:
                        simulatorFsx->onSimFrame();
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
                        DataDefinitionOwnAircraft *ownAircaft;
                        ownAircaft = (DataDefinitionOwnAircraft *)&pObjData->dwData;
                        simulatorFsx->updateOwnAircraftFromSim(*ownAircaft);
                        break;
                    case CSimConnectDataDefinition::RequestOwnAircraftTitle:
                        DataDefinitionOwnAircraftModel *dataDefinitionModel = (DataDefinitionOwnAircraftModel *) &pObjData->dwData;
                        CAircraftModel model;
                        model.setQueriedModelString(dataDefinitionModel->title);
                        simulatorFsx->setAircraftModel(model);
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
            } // switch
        } // method
    }
}
