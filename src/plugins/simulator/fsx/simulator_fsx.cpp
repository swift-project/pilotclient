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

#include <QTimer>
#include <QtConcurrent>

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
            m_simconnectTimerId(-1),
            m_skipCockpitUpdateCycles(0),
            m_fsuipc(new CFsuipc())
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

        bool CSimulatorFsx::isFsuipcConnected() const
        {
            return m_fsuipc->isConnected();
        }


        bool CSimulatorFsx::connectTo()
        {
            if (m_isConnected) return true;

            if (FAILED(SimConnect_Open(&m_hSimConnect, BlackMisc::CProject::systemNameAndVersionChar(), nullptr, 0, 0, 0)))
            {
                emit statusChanged(ConnectionFailed);
                return false;
            }
            else
            {
                this->m_fsuipc->connect(); // connect FSUIPC too
            }

            initEvents();
            initDataDefinitions();
            m_simconnectTimerId = startTimer(10);
            m_isConnected = true;

            emit statusChanged(Connected);
            return true;
        }

        void CSimulatorFsx::asyncConnectTo()
        {
            connect(&m_watcherConnect, SIGNAL(finished()), this, SLOT(connectToFinished()));

            // simplified connect, timers and signals not in different thread
            auto asyncConnectFunc = [&]() -> bool
            {
                if (FAILED(SimConnect_Open(&m_hSimConnect, BlackMisc::CProject::systemNameAndVersionChar(), nullptr, 0, 0, 0))) return false;
                this->m_fsuipc->connect(); // FSUIPC too
                return true;
            };

            QFuture<bool> result = QtConcurrent::run(asyncConnectFunc);
            m_watcherConnect.setFuture(result);
        }

        bool CSimulatorFsx::disconnectFrom()
        {
            if (!m_isConnected)
                return true;

            emit statusChanged(Disconnected);
            if (m_hSimConnect)
            {
                SimConnect_Close(m_hSimConnect);
                this->m_fsuipc->disconnect();
            }

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

            if (FAILED(SimConnect_Open(&m_hSimConnect, BlackMisc::CProject::systemNameAndVersionChar(), nullptr, 0, 0, 0)))
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

            CSimConnectObject simObj;
            simObj.setCallsign(callsign);
            simObj.setRequestId(m_nextObjID);
            simObj.setObjectId(0);
            simObj.getInterpolator()->addAircraftSituation(initialSituation);
            m_simConnectObjects.insert(callsign, simObj);
            ++m_nextObjID;

            HRESULT hr = SimConnect_AICreateNonATCAircraft(m_hSimConnect, "Boeing 737-800 Paint1", callsign.toQString().left(12).toLatin1().constData(), initialPosition, simObj.getRequestId());
            Q_UNUSED(hr);
        }

        void CSimulatorFsx::addAircraftSituation(const CCallsign &callsign, const CAircraftSituation &situation)
        {
            if (!m_simConnectObjects.contains(callsign))
            {
                addRemoteAircraft(callsign, "Boeing 737-800 Paint1", situation);
                return;
            }

            CSimConnectObject simObj = m_simConnectObjects.value(callsign);
            simObj.getInterpolator()->addAircraftSituation(situation);
            m_simConnectObjects.insert(callsign, simObj);
        }

        void CSimulatorFsx::removeRemoteAircraft(const CCallsign &callsign)
        {
            removeRemoteAircraft(m_simConnectObjects.value(callsign));
        }

        CSimulatorInfo CSimulatorFsx::getSimulatorInfo() const
        {
            return this->m_simulatorInfo;
        }

        void CSimulatorFsx::setAircraftModel(const BlackMisc::Network::CAircraftModel &model)
        {
            if (m_aircraftModel != model)
            {
                m_aircraftModel = model;
                emit aircraftModelChanged(model);
            }
        }

        bool CSimulatorFsx::updateOwnSimulatorCockpit(const CAircraft &ownAircraft)
        {
            CComSystem newCom1 = ownAircraft.getCom1System();
            CComSystem newCom2 = ownAircraft.getCom2System();
            CTransponder newTransponder = ownAircraft.getTransponder();

            bool changed = false;
            if (newCom1 != this->m_ownAircraft.getCom1System())
            {
                if (newCom1.getFrequencyActive() != this->m_ownAircraft.getCom1System().getFrequencyActive())
                    SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom1Active,
                                                   CBcdConversions::comFrequencyToBcdHz(newCom1.getFrequencyActive()), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                if (newCom1.getFrequencyStandby() != this->m_ownAircraft.getCom1System().getFrequencyStandby())
                    SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom1Standby,
                                                   CBcdConversions::comFrequencyToBcdHz(newCom1.getFrequencyStandby()), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

                this->m_ownAircraft.setCom1System(newCom1);
                changed = true;
            }

            if (newCom2 != this->m_ownAircraft.getCom2System())
            {
                if (newCom2.getFrequencyActive() != this->m_ownAircraft.getCom2System().getFrequencyActive())
                    SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom2Active,
                                                   CBcdConversions::comFrequencyToBcdHz(newCom2.getFrequencyActive()), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                if (newCom2.getFrequencyStandby() != this->m_ownAircraft.getCom2System().getFrequencyStandby())
                    SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetCom2Standby,
                                                   CBcdConversions::comFrequencyToBcdHz(newCom2.getFrequencyStandby()), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

                this->m_ownAircraft.setCom2System(newCom2);
                changed = true;
            }

            if (newTransponder != this->m_ownAircraft.getTransponder())
            {
                if (newTransponder.getTransponderCode() != this->m_ownAircraft.getTransponder().getTransponderCode())
                {
                    SimConnect_TransmitClientEvent(m_hSimConnect, 0, EventSetTransponderCode,
                                                   CBcdConversions::transponderCodeToBcd(newTransponder), SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                    changed = true;
                }
                this->m_ownAircraft.setTransponder(newTransponder);
            }

            // avoid changes of cockpit back to old values due to an outdated read back value
            if (changed) m_skipCockpitUpdateCycles = SkipUpdateCyclesForCockpit;

            // bye
            return changed;
        }

        void CSimulatorFsx::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            QByteArray m = message.getMessage().toLocal8Bit().constData();
            m.append('\0');

            SIMCONNECT_TEXT_TYPE type = SIMCONNECT_TEXT_TYPE_PRINT_BLACK;
            switch (message.getSeverity())
            {
            case CStatusMessage::SeverityInfo:
                type = SIMCONNECT_TEXT_TYPE_PRINT_GREEN;
                break;
            case CStatusMessage::SeverityWarning:
                type = SIMCONNECT_TEXT_TYPE_PRINT_YELLOW;
                break;
            case CStatusMessage::SeverityError:
                type = SIMCONNECT_TEXT_TYPE_PRINT_RED;
                break;
            }
            HRESULT hr = SimConnect_Text(
                             m_hSimConnect, type, 7.5, EventTextMessage, m.size(),
                             m.data()
                         );
            Q_UNUSED(hr);
        }

        CAirportList CSimulatorFsx::getAirportsInRange() const
        {
            return this->m_airportsInRange;
        }

        void CSimulatorFsx::setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset)
        {
            Q_UNUSED(enable);
            Q_UNUSED(offset);
        }

        void CSimulatorFsx::onSimRunning()
        {
            if (m_simRunning) return;
            m_simRunning = true;
            SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::RequestOwnAircraft,
                                              CSimConnectDataDefinition::DataOwnAircraft,
                                              SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);


            SimConnect_RequestDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::RequestOwnAircraftTitle,
                                              CSimConnectDataDefinition::DataOwnAircraftTitle,
                                              SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND,
                                              SIMCONNECT_DATA_REQUEST_FLAG_CHANGED);

            emit simulatorStarted();
        }

        void CSimulatorFsx::onSimStopped()
        {
            if (!m_simRunning) return;
            m_simRunning = false;
            emit simulatorStopped();
        }

        void CSimulatorFsx::onSimFrame()
        {
            update();
        }

        void CSimulatorFsx::onSimExit()
        {
            this->onSimStopped();
        }

        void CSimulatorFsx::updateOwnAircraftFromSim(DataDefinitionOwnAircraft simulatorOwnAircraft)
        {
            BlackMisc::Geo::CCoordinateGeodetic position;
            position.setLatitude(CLatitude(simulatorOwnAircraft.latitude, CAngleUnit::deg()));
            position.setLongitude(CLongitude(simulatorOwnAircraft.longitude, CAngleUnit::deg()));

            BlackMisc::Aviation::CAircraftSituation aircraftSituation;
            aircraftSituation.setPosition(position);
            aircraftSituation.setPitch(CAngle(simulatorOwnAircraft.pitch, CAngleUnit::deg()));
            aircraftSituation.setBank(CAngle(simulatorOwnAircraft.bank, CAngleUnit::deg()));
            aircraftSituation.setHeading(CHeading(simulatorOwnAircraft.trueHeading, CHeading::True, CAngleUnit::deg()));
            aircraftSituation.setGroundspeed(CSpeed(simulatorOwnAircraft.velocity, CSpeedUnit::kts()));
            aircraftSituation.setAltitude(CAltitude(simulatorOwnAircraft.altitude, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
            m_ownAircraft.setSituation(aircraftSituation);

            CComSystem com1 = m_ownAircraft.getCom1System(); // set defaults
            CComSystem com2 = m_ownAircraft.getCom2System();
            CTransponder transponder = m_ownAircraft.getTransponder();

            // When I change cockpit values in the sim (from GUI to simulator, not originating from simulator)
            // it takes a little while before these values are set in the simulator.
            // To avoid jitters, I wait some update cylces to stabilize the values
            if (m_skipCockpitUpdateCycles < 1)
            {
                com1.setFrequencyActive(CFrequency(simulatorOwnAircraft.com1ActiveMHz, CFrequencyUnit::MHz()));
                com1.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com1StandbyMHz, CFrequencyUnit::MHz()));
                m_ownAircraft.setCom1System(com1);

                com2.setFrequencyActive(CFrequency(simulatorOwnAircraft.com2ActiveMHz, CFrequencyUnit::MHz()));
                com2.setFrequencyStandby(CFrequency(simulatorOwnAircraft.com2StandbyMHz, CFrequencyUnit::MHz()));
                m_ownAircraft.setCom2System(com2);

                transponder.setTransponderCode(simulatorOwnAircraft.transponderCode);
                m_ownAircraft.setTransponder(transponder);
            }
            else
                --m_skipCockpitUpdateCycles;
        }

        void CSimulatorFsx::setSimconnectObjectID(DWORD requestID, DWORD objectID)
        {
            // First check, if this request id belongs to us
            auto it = std::find_if(m_simConnectObjects.begin(), m_simConnectObjects.end(),
            [requestID](const CSimConnectObject & obj) { return obj.getRequestId() == static_cast<int>(requestID); });

            if (it == m_simConnectObjects.end())
                return;

            it->setObjectId(objectID);
            SimConnect_AIReleaseControl(m_hSimConnect, objectID, requestID);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EventFreezeLat, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EventFreezeAlt, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
            SimConnect_TransmitClientEvent(m_hSimConnect, objectID, EventFreezeAtt, 1,
                                           SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

            DataDefinitionGearHandlePosition gearHandle;
            gearHandle.gearHandlePosition = 1;

            SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::DataDefinitionGearHandlePosition, objectID, SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0, sizeof(gearHandle), &gearHandle);
        }

        void CSimulatorFsx::timerEvent(QTimerEvent * /* event */)
        {
            dispatch();
        }

        void CSimulatorFsx::dispatch()
        {
            SimConnect_CallDispatch(m_hSimConnect, SimConnectProc, this);
            if (this->m_fsuipc) this->m_fsuipc->process();
        }

        void CSimulatorFsx::connectToFinished()
        {
            if (m_watcherConnect.result())
            {
                initEvents();
                initDataDefinitions();
                m_simconnectTimerId = startTimer(50);
                m_isConnected = true;

                emit statusChanged(Connected);
            }
            else
                emit statusChanged(ConnectionFailed);
        }

        void CSimulatorFsx::removeRemoteAircraft(const CSimConnectObject &simObject)
        {
            SimConnect_AIRemoveObject(m_hSimConnect, simObject.getObjectId(), simObject.getRequestId());
            m_simConnectObjects.remove(simObject.getCallsign());
        }

        HRESULT CSimulatorFsx::initEvents()
        {
            HRESULT hr = S_OK;
            // System events
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EventSimStatus, "Sim");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EventObjectAdded, "ObjectAdded");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EventObjectRemoved, "ObjectRemoved");
            hr += SimConnect_SubscribeToSystemEvent(m_hSimConnect, EventFrame, "Frame");

            // Mapped events
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeLat, "FREEZE_LATITUDE_LONGITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeAlt, "FREEZE_ALTITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventFreezeAtt, "FREEZE_ATTITUDE_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom1Active, "COM_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom1Standby, "COM_STBY_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom2Active, "COM2_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetCom2Standby, "COM2_STBY_RADIO_SET");
            hr += SimConnect_MapClientEventToSimEvent(m_hSimConnect, EventSetTransponderCode, "XPNDR_SET");

            // facility
            hr = SimConnect_SubscribeToFacilities(m_hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT, m_nextObjID++);
            return hr;
        }

        HRESULT CSimulatorFsx::initDataDefinitions()
        {
            return CSimConnectDataDefinition::initDataDefinitions(m_hSimConnect);
        }

        void CSimulatorFsx::update()
        {
            foreach(CSimConnectObject simObj, m_simConnectObjects)
            {
                if (simObj.getInterpolator()->getTimeOfLastReceivedSituation().secsTo(QDateTime::currentDateTimeUtc()) > 15)
                {
                    removeRemoteAircraft(simObj);
                    continue;
                }

                if (simObj.getInterpolator()->hasEnoughAircraftSituations())
                {
                    SIMCONNECT_DATA_INITPOSITION position;
                    CAircraftSituation situation = simObj.getInterpolator()->getCurrentSituation();
                    position.Latitude = situation.latitude().value();
                    position.Longitude = situation.longitude().value();
                    position.Altitude = situation.getAltitude().value(CLengthUnit::ft());
                    position.Pitch = situation.getPitch().value();
                    position.Bank = situation.getBank().value();
                    position.Heading = situation.getHeading().value(CAngleUnit::deg());
                    position.Airspeed = situation.getGroundSpeed().value(CSpeedUnit::kts());

                    // TODO: epic fail for helicopters and VTOPs!
                    position.OnGround = position.Airspeed < 30 ? 1 : 0;

                    DataDefinitionRemoteAircraftSituation ddAircraftSituation;
                    ddAircraftSituation.position = position;

                    DataDefinitionGearHandlePosition gearHandle;
                    gearHandle.gearHandlePosition = position.Altitude < 1000 ? 1 : 0;

                    if (simObj.getObjectId() != 0)
                    {
                        SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::DataDefinitionRemoteAircraftSituation, simObj.getObjectId(), SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0, sizeof(ddAircraftSituation), &ddAircraftSituation);

                        // With the following SimConnect call all aircrafts loose their red tag. No idea why though.
                        SimConnect_SetDataOnSimObject(m_hSimConnect, CSimConnectDataDefinition::DataDefinitionGearHandlePosition, simObj.getObjectId(), SIMCONNECT_DATA_SET_FLAG_DEFAULT, 0, sizeof(DataDefinitionGearHandlePosition), &gearHandle);
                    }
                }
            }
        }
    }
}
