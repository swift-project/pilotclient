/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_fsx.h"
#include "simconnect_datadefinition.h"
#include "blacksim/fsx/simconnectutilities.h"
#include "blacksim/fsx/fsxsimulatorsetup.h"
#include "blacksim/simulatorinfo.h"
#include <QTimer>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackSim;
using namespace BlackSim::Fsx;

namespace BlackSimPlugin
{
    namespace Fsx
    {
        BlackCore::ISimulator *CSimulatorFsxFactory::create(QObject *parent)
        {
            return new Fsx::CSimulatorFsx(parent);
        }

        BlackSim::CSimulatorInfo CSimulatorFsxFactory::getSimulatorInfo() const
        {
            return CSimulatorInfo::FSX();
        }

        CSimulatorFsx::CSimulatorFsx(QObject *parent) :
            ISimulator(parent),
            m_isConnected(false),
            m_simRunning(false),
            m_hSimConnect(nullptr),
            m_nextObjID(1),
            m_simulatorInfo(CSimulatorInfo::FSX()),
            m_simconnectTimerId(-1)
        {
            CFsxSimulatorSetup setup;
            setup.init(); // this fetches important setting on local side
            this->m_simulatorInfo.setSimulatorSetup(setup.getSettings());
        }

        CSimulatorFsx::~CSimulatorFsx()
        {
            disconnectFrom();
        }

        bool CSimulatorFsx::isConnected() const
        {
            return m_isConnected;
        }

        bool CSimulatorFsx::connectTo()
        {
            if(m_isConnected)
                return true;

            if (FAILED(SimConnect_Open(&m_hSimConnect, "BlackBox", nullptr, 0, 0, 0)))
            {
                return false;
            }

            initSystemEvents();
            initDataDefinitions();
            m_simconnectTimerId = startTimer(10);
            m_isConnected = true;

            emit connectionChanged(true);
            return true;
        }

        bool CSimulatorFsx::disconnectFrom()
        {
            if (!m_isConnected)
                return true;

            emit connectionChanged(false);
            if (m_hSimConnect)
                SimConnect_Close(m_hSimConnect);


            if (m_simconnectTimerId)
                killTimer(m_simconnectTimerId);

            m_hSimConnect = nullptr;
            m_simconnectTimerId = -1;
            m_isConnected = false;

            return true;
        }

        bool CSimulatorFsx::canConnect()
        {
            if (m_isConnected)
                return true;

            if (FAILED(SimConnect_Open(&m_hSimConnect, "BlackBox", nullptr, 0, 0, 0)))
            {
                return false;
            }
            SimConnect_Close(m_hSimConnect);

            return true;
        }

        void CSimulatorFsx::addRemoteAircraft(const CCallsign &callsign, const QString &type, const CAircraftSituation &initialSituation)
        {
            Q_UNUSED(type);

            SIMCONNECT_DATA_INITPOSITION initialPosition;
            initialPosition.Latitude = initialSituation.latitude().value();
            initialPosition.Longitude = initialSituation.longitude().value();
            initialPosition.Altitude = initialSituation.getAltitude().value();
            initialPosition.Pitch = initialSituation.getPitch().value();
            initialPosition.Bank = initialSituation.getBank().value();
            initialPosition.Heading = initialSituation.getHeading().value();
            initialPosition.Airspeed = 0;
            initialPosition.OnGround = 0;

            SimConnectObject simObj;
            simObj.m_callsign = callsign;
            simObj.m_requestId = m_nextObjID;
            simObj.m_objectId = 0;
            simObj.m_interpolator.addAircraftSituation(initialSituation);
            m_simConnectObjects.insert(callsign, simObj);
            ++m_nextObjID;

            HRESULT hr = SimConnect_AICreateNonATCAircraft(m_hSimConnect, "Boeing 737-800 Paint1", callsign.toQString().left(12).toLatin1().constData(), initialPosition, simObj.m_requestId);
            Q_UNUSED(hr);
        }

        void CSimulatorFsx::addAircraftSituation(const CCallsign &callsign, const CAircraftSituation &situation)
        {
            if (!m_simConnectObjects.contains(callsign))
            {
                addRemoteAircraft(callsign, "Boeing 737-800 Paint1", situation);
                return;
            }

            SimConnectObject simObj = m_simConnectObjects.value(callsign);
            simObj.m_interpolator.addAircraftSituation(situation);
            m_simConnectObjects.insert(callsign, simObj);
        }

        void CSimulatorFsx::removeRemoteAircraft(const CCallsign &/*callsign*/)
        {
            // TODO
        }

        CSimulatorInfo CSimulatorFsx::getSimulatorInfo() const
        {
            return this->m_simulatorInfo;
        }

        void CALLBACK CSimulatorFsx::SimConnectProc(SIMCONNECT_RECV *pData, DWORD /* cbData */, void *pContext)
        {
            CSimulatorFsx *simulatorFsx = static_cast<CSimulatorFsx *>(pContext);

            switch (pData->dwID)
            {
            case SIMCONNECT_RECV_ID_EXCEPTION:
                {
                    SIMCONNECT_RECV_EXCEPTION *event = (SIMCONNECT_RECV_EXCEPTION *)pData;
                    qDebug() << "Caught simConnect exception: " << CSimConnectUtilities::simConnectExceptionToString((SIMCONNECT_EXCEPTION)event->dwException);
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
                    SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *event = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE *>(pData);
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
                    SIMCONNECT_RECV_EVENT_FRAME  *event = (SIMCONNECT_RECV_EVENT_FRAME *) pData;
                    switch(event->uEventID)
                    {
                    case EVENT_FRAME:
                        simulatorFsx->onSimFrame();
                        break;
                    }
                }
            case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
                {
                    SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *event = static_cast<SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *>(pData);
                    simulatorFsx->setSimconnectObjectID(event->dwRequestID, event->dwObjectID);
                }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
                {
                    SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA *) pData;
                    switch (pObjData->dwRequestID)
                    {
                    case CSimConnectDataDefinition::RequestOwnAircraft:
                        DataDefinitionOwnAircraft *ownAircaft;
                        ownAircaft = (DataDefinitionOwnAircraft *)&pObjData->dwData;
                        simulatorFsx->setOwnAircraft(*ownAircaft);
                        break;
                    }
                    break;
                }

            }
        }

        void CSimulatorFsx::onSimRunning()
        {
            m_simRunning = true;
            SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::RequestOwnAircraft,
                                              CSimConnectDataDefinition::DataOwnAircraft,
                                              SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
        }

        void CSimulatorFsx::onSimStopped()
        {
            m_simRunning = false;
        }

        void CSimulatorFsx::onSimFrame()
        {
            update();
        }

        void CSimulatorFsx::onSimExit()
        {

        }

        void CSimulatorFsx::setOwnAircraft(DataDefinitionOwnAircraft aircraft)
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

            CTransponder transponder("Transponder", aircraft.transponderCode, CTransponder::ModeS);

            m_ownAircraft.setSituation(aircraftSituation);
            m_ownAircraft.setCom1System(com1);
            m_ownAircraft.setCom2System(com2);
            m_ownAircraft.setTransponder(transponder);
        }

        void CSimulatorFsx::setSimconnectObjectID(DWORD requestID, DWORD objectID)
        {
            SimConnect_AIReleaseControl(m_hSimConnect, objectID, requestID);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EVENT_FREEZELAT, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EVENT_FREEZEALT, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EVENT_FREEZEATT, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

            DataDefinitionGearHandlePosition gearHandle;
            gearHandle.gearHandlePosition = 1;

            SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::DataDefinitionGearHandlePosition, objectID, SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0, sizeof(gearHandle), &gearHandle);

            SimConnectObject simObject;
            foreach(simObject, m_simConnectObjects)
            {
                if (simObject.m_requestId == static_cast<int>(requestID))
                {
                    simObject.m_objectId = objectID;
                    break;
                }
            }
            m_simConnectObjects.insert(simObject.m_callsign, simObject);

        }

        void CSimulatorFsx::timerEvent(QTimerEvent * /* event */)
        {
            dispatch();
        }

        void CSimulatorFsx::dispatch()
        {
            SimConnect_CallDispatch(m_hSimConnect, SimConnectProc, this);
        }

        HRESULT CSimulatorFsx::initSystemEvents()
        {
            HRESULT hr = S_OK;
            // System events
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_SIM_STATUS, "Sim");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_OBJECT_ADDED, "ObjectAdded");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_OBJECT_REMOVED, "ObjectRemoved");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EVENT_FRAME, "Frame");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EVENT_FREEZELAT, "FREEZE_LATITUDE_LONGITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EVENT_FREEZEALT, "FREEZE_ALTITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EVENT_FREEZEATT, "FREEZE_ATTITUDE_SET");

            return hr;
        }

        HRESULT CSimulatorFsx::initDataDefinitions()
        {
            return CSimConnectDataDefinition::initDataDefinitions(m_hSimConnect);
        }

        void CSimulatorFsx::update()
        {
            foreach(SimConnectObject simObj, m_simConnectObjects)
            {
                if (simObj.m_interpolator.hasEnoughAircraftSituations())
                {

                    SIMCONNECT_DATA_INITPOSITION position;
                    CAircraftSituation situation = simObj.m_interpolator.getCurrentSituation();
                    position.Latitude = situation.latitude().value();
                    position.Longitude = situation.longitude().value();
                    position.Altitude = situation.getAltitude().value(CLengthUnit::ft());
                    position.Pitch = situation.getPitch().value();
                    position.Bank = situation.getBank().value();
                    position.Heading = situation.getHeading().value(CAngleUnit::deg());
                    position.Airspeed = situation.getGroundSpeed().value(CSpeedUnit::kts());
                    position.OnGround = position.Airspeed < 30 ? 1 : 0;

                    DataDefinitionRemoteAircraftSituation ddAircraftSituation;
                    ddAircraftSituation.position = position;

                    DataDefinitionGearHandlePosition gearHandle;
                    gearHandle.gearHandlePosition = 1;

                    if (simObj.m_objectId != 0)
                    {
                        SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::DataDefinitionRemoteAircraftSituation, simObj.m_objectId, SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0, sizeof(ddAircraftSituation), &ddAircraftSituation);

                        // With the following SimConnect call all aircrafts loose their red tag. No idea why though.
                        SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::DataDefinitionGearHandlePosition, simObj.m_objectId, SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0, sizeof(gearHandle), &gearHandle);
                    }
                }
            }
        }
    }
}
