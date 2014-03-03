/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_fsx.h"
#include "simconnect_datadefinition.h"
#include "simconnect_exception.h"
#include <QTimer>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackCore
{
    namespace FSX
    {
        CSimulatorFSX::CSimulatorFSX(QObject *parent) :
            ISimulator(parent),
            m_isConnected(false),
            m_simRunning(false),
            m_hSimConnect(nullptr)
        {
            QTimer::singleShot(5000, this, SLOT(checkConnection()));
        }

        bool CSimulatorFSX::isConnected() const
        {
            return m_isConnected;
        }

        void CALLBACK CSimulatorFSX::SimConnectProc(SIMCONNECT_RECV* pData, DWORD /* cbData */, void *pContext)
        {
            CSimulatorFSX *simulatorFsx = static_cast<CSimulatorFSX*>(pContext);

            switch(pData->dwID)
            {
                case SIMCONNECT_RECV_ID_EXCEPTION:
                {
                    SIMCONNECT_RECV_EXCEPTION *event = (SIMCONNECT_RECV_EXCEPTION*)pData;
                    CSimConnectException::handleException((SIMCONNECT_EXCEPTION)event->dwException);
                    break;
                }
                case SIMCONNECT_RECV_ID_QUIT:
                {
                    simulatorFsx->onSimExit();
                    break;
                }
                case SIMCONNECT_RECV_ID_EVENT:
                {
                    SIMCONNECT_RECV_EVENT *event = static_cast<SIMCONNECT_RECV_EVENT*>(pData);

                    switch(event->uEventID)
                    {
                    case EVENT_SIM_STATUS:
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
                    SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE*>(pData);
                    if (event->uEventID == EVENT_OBJECT_ADDED)
                    {
                    }
                    else if (event->uEventID == EVENT_OBJECT_REMOVED)
                    {
                    }

                    break;
                }
                case SIMCONNECT_RECV_ID_EVENT_FRAME:
                {
                    simulatorFsx->onSimFrame();
                    break;
                }
                case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
                {
                    SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *event = static_cast<SIMCONNECT_RECV_ASSIGNED_OBJECT_ID*>(pData);
                    simulatorFsx->setSimconnectObjectID(event->dwRequestID, event->dwObjectID);
                }
                case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
                {
                    SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*) pData;
                    switch(pObjData->dwRequestID)
                    {
                    case CSimConnectDataDefinition::RequestOwnAircraft:
                        OwnAircraft *ownAircaft;
                        ownAircaft = (OwnAircraft*)&pObjData->dwData;
                        simulatorFsx->setOwnAircraft(*ownAircaft);
                        break;
                    }
                    break;
                }

            }
        }

        void CSimulatorFSX::onSimRunning()
        {
            m_simRunning = true;
            SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::RequestOwnAircraft,
                                              CSimConnectDataDefinition::DataOwnAircraft,
                                              SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        }

        void CSimulatorFSX::onSimStopped()
        {
            m_simRunning = false;
        }

        void CSimulatorFSX::onSimFrame()
        {
        }

        void CSimulatorFSX::onSimExit()
        {

        }

        void CSimulatorFSX::setOwnAircraft(OwnAircraft aircraft)
        {
            BlackMisc::Geo::CCoordinateGeodetic position;
            position.setLatitude(CLatitude(aircraft.latitude, CAngleUnit::deg()));
            position.setLongitude(CLongitude(aircraft.longitude, CAngleUnit::deg()));

            BlackMisc::Aviation::CAircraftSituation aircraftSituation;
            aircraftSituation.setPosition(position);
            aircraftSituation.setPitch(CAngle(aircraft.pitch, CAngleUnit::deg()));
            aircraftSituation.setBank(CAngle(aircraft.bank, CAngleUnit::deg()));
            aircraftSituation.setHeading(CHeading(aircraft.trueHeading, CHeading::True, CAngleUnit::deg()));
            aircraftSituation.setGroundspeed(CSpeed(aircraft.velocity, CSpeedUnit::kts()));
            aircraftSituation.setAltitude(CAltitude(aircraft.altitude, CAltitude::MeanSeaLevel, CLengthUnit::ft()));

            CComSystem com1;
            com1.setFrequencyActive(CFrequency(aircraft.com1ActiveMHz, CFrequencyUnit::MHz()));
            com1.setFrequencyStandby(CFrequency(aircraft.com1StandbyMHz, CFrequencyUnit::MHz()));

            CComSystem com2;
            com2.setFrequencyActive(CFrequency(aircraft.com2ActiveMHz, CFrequencyUnit::MHz()));
            com2.setFrequencyStandby(CFrequency(aircraft.com2StandbyMHz, CFrequencyUnit::MHz()));

            CTransponder transponder("Transponder", aircraft.transponderCode, CTransponder::ModeC);

            m_ownAircraft.setSituation(aircraftSituation);
            m_ownAircraft.setCom1System(com1);
            m_ownAircraft.setCom2System(com2);
            m_ownAircraft.setTransponder(transponder);
        }

        void CSimulatorFSX::setSimconnectObjectID(DWORD /* requestID */, DWORD /* objectID */)
        {

        }

        void CSimulatorFSX::timerEvent(QTimerEvent* /* event */)
        {
            dispatch();
        }

        void CSimulatorFSX::checkConnection()
        {
            if (FAILED(SimConnect_Open(&m_hSimConnect, "BlackBox", nullptr, 0, 0, 0)))
            {
                QTimer::singleShot(5000, this, SLOT(checkConnection()));
                return;
            }

            initSystemEvents();
            initDataDefinitions();
            startTimer(50);
            m_isConnected = true;

            emit connectionChanged(true);
        }

        void CSimulatorFSX::dispatch()
        {
            SimConnect_CallDispatch(m_hSimConnect, SimConnectProc, this);
        }

        HRESULT CSimulatorFSX::initSystemEvents()
        {
            HRESULT hr = S_OK;
            // System events
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_SIM_STATUS, "Sim");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_OBJECT_ADDED, "ObjectAdded");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_OBJECT_REMOVED, "ObjectRemoved");

            return hr;
        }

        HRESULT CSimulatorFSX::initDataDefinitions()
        {
            return CSimConnectDataDefinition::initDataDefinitions(m_hSimConnect);
        }
    }
}
