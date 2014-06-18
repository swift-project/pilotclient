/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_xplane.h"
#include "xbus_service_proxy.h"
#include <QDBusServiceWatcher>
#include <QTimer>

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlane::CSimulatorXPlane(QObject *parent) : BlackCore::ISimulator(parent)
        {
            m_watcher = new QDBusServiceWatcher(this);
            m_watcher->setWatchMode(QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration);
            m_watcher->addWatchedService(CXBusServiceProxy::InterfaceName());
            connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, &CSimulatorXPlane::serviceRegistered);
            connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, &CSimulatorXPlane::serviceUnregistered);

            m_fastTimer = new QTimer(this);
            m_slowTimer = new QTimer(this);
            connect(m_fastTimer, &QTimer::timeout, this, &CSimulatorXPlane::fastTimerTimeout);
            connect(m_slowTimer, &QTimer::timeout, this, &CSimulatorXPlane::slowTimerTimeout);
            m_fastTimer->start(100);
            m_slowTimer->start(1000);

            resetData();
        }

        void CSimulatorXPlane::fastTimerTimeout()
        {
            if (isConnected())
            {
                m_service->getLatitudeAsync(&m_xplaneData.latitude);
                m_service->getLongitudeAsync(&m_xplaneData.longitude);
                m_service->getAltitudeMSLAsync(&m_xplaneData.altitude);
                m_service->getGroundSpeedAsync(&m_xplaneData.groundspeed);
                m_service->getPitchAsync(&m_xplaneData.pitch);
                m_service->getRollAsync(&m_xplaneData.roll);
                m_service->getTrueHeadingAsync(&m_xplaneData.trueHeading);
                m_service->getCom1ActiveAsync(&m_xplaneData.com1Active);
                m_service->getCom1StandbyAsync(&m_xplaneData.com1Standby);
                m_service->getCom2ActiveAsync(&m_xplaneData.com2Active);
                m_service->getCom2StandbyAsync(&m_xplaneData.com2Standby);
                m_service->getTransponderCodeAsync(&m_xplaneData.xpdrCode);
                m_service->getTransponderModeAsync(&m_xplaneData.xpdrMode);
                m_service->getTransponderIdentAsync(&m_xplaneData.xpdrIdent);
            }
        }

        void CSimulatorXPlane::slowTimerTimeout()
        {
            if (isConnected())
            {
                m_service->getAircraftModelPathAsync(&m_xplaneData.aircraftModelPath);
                m_service->getAircraftIcaoCodeAsync(&m_xplaneData.aircraftIcaoCode);
            }
        }

        bool CSimulatorXPlane::isConnected() const
        {
            return m_service;
        }

        bool CSimulatorXPlane::canConnect()
        {
            if (isConnected()) { return true; }
            auto conn = QDBusConnection::sessionBus(); // TODO make this configurable
            auto dummy = new CXBusServiceProxy(conn, this, true);
            bool ok = dummy->isValid();
            delete dummy;
            return ok;
        }

        bool CSimulatorXPlane::connectTo()
        {
            if (isConnected()) { return true; }
            m_conn = QDBusConnection::sessionBus(); // TODO make this configurable
            m_service = new CXBusServiceProxy(m_conn, this);
            if (m_service->isValid())
            {
                connect(m_service, &CXBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::aircraftModelChanged);
                m_watcher->setConnection(m_conn);
                emit statusChanged(ISimulator::Connected);
                return true;
            }
            else
            {
                disconnectFrom();
                return false;
            }
        }

        void CSimulatorXPlane::asyncConnectTo()
        {
            // TODO
            connectTo();
        }

        bool CSimulatorXPlane::disconnectFrom()
        {
            emit statusChanged(ISimulator::Disconnected);
            m_conn = QDBusConnection { "default" };
            m_watcher->setConnection(m_conn);
            delete m_service;
            m_service = nullptr;
            return true;
        }

        void CSimulatorXPlane::serviceRegistered()
        {
            delete m_service;
            m_service = new CXBusServiceProxy(m_conn, this);
            connect(m_service, &CXBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::aircraftModelChanged);
            emit statusChanged(ISimulator::Connected);
        }

        void CSimulatorXPlane::serviceUnregistered()
        {
            delete m_service;
            m_service = nullptr;
            emit statusChanged(ISimulator::Disconnected);
        }

        // convert xplane squawk mode to swift squawk mode
        BlackMisc::Aviation::CTransponder::TransponderMode xpdrMode(int xplaneMode, bool ident)
        {
            if (ident) { return BlackMisc::Aviation::CTransponder::StateIdent; }
            if (xplaneMode == 0 || xplaneMode == 1) { return BlackMisc::Aviation::CTransponder::StateStandby; }
            return BlackMisc::Aviation::CTransponder::ModeC;
        }
        // convert swift squawk mode to xplane squawk mode
        int xpdrMode(BlackMisc::Aviation::CTransponder::TransponderMode mode)
        {
            return mode == BlackMisc::Aviation::CTransponder::StateStandby ? 1 : 2;
        }

        BlackMisc::Aviation::CAircraft CSimulatorXPlane::getOwnAircraft() const
        {
            if (! isConnected()) { return {}; }
            using namespace BlackMisc;
            using namespace BlackMisc::PhysicalQuantities;
            Aviation::CAircraftSituation situation;
            situation.setPosition({ m_xplaneData.latitude, m_xplaneData.longitude, 0 });
            situation.setAltitude({ m_xplaneData.altitude, Aviation::CAltitude::MeanSeaLevel, CLengthUnit::m() });
            situation.setHeading({ m_xplaneData.trueHeading, Aviation::CHeading::True, CAngleUnit::deg() });
            situation.setPitch({ m_xplaneData.pitch, CAngleUnit::deg() });
            situation.setBank({ m_xplaneData.roll, CAngleUnit::deg() });
            situation.setGroundspeed({ m_xplaneData.groundspeed, CSpeedUnit::m_s() });
            Aviation::CAircraft ac { {}, {}, situation };
            ac.setIcaoInfo(Aviation::CAircraftIcao { m_xplaneData.aircraftIcaoCode });
            ac.setCom1System(Aviation::CComSystem::getCom1System({ m_xplaneData.com1Active, CFrequencyUnit::kHz() }, { m_xplaneData.com1Standby, CFrequencyUnit::kHz() }));
            ac.setCom2System(Aviation::CComSystem::getCom2System({ m_xplaneData.com2Active, CFrequencyUnit::kHz() }, { m_xplaneData.com2Standby, CFrequencyUnit::kHz() }));
            ac.setTransponder(Aviation::CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent)));
            return ac;
        }

        void CSimulatorXPlane::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            if (! isConnected()) { return; }
            // TODO
            Q_UNUSED(message);
        }

        BlackMisc::Network::CAircraftModel CSimulatorXPlane::getAircraftModel() const
        {
            if (! isConnected()) { return {}; }
            return m_xplaneData.aircraftModelPath;
        }

        bool CSimulatorXPlane::updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft)
        {
            if (! isConnected()) { return false; }
            using namespace BlackMisc;
            using namespace BlackMisc::PhysicalQuantities;
            auto com1 = Aviation::CComSystem::getCom1System({ m_xplaneData.com1Active, CFrequencyUnit::kHz() }, { m_xplaneData.com1Standby, CFrequencyUnit::kHz() });
            auto com2 = Aviation::CComSystem::getCom2System({ m_xplaneData.com2Active, CFrequencyUnit::kHz() }, { m_xplaneData.com2Standby, CFrequencyUnit::kHz() });
            auto xpdr = Aviation::CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
            if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
            {
                m_xplaneData.com1Active = aircraft.getCom1System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com1Standby = aircraft.getCom1System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com2Active = aircraft.getCom2System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com2Standby = aircraft.getCom2System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.xpdrCode = aircraft.getTransponderCode();
                m_xplaneData.xpdrMode = xpdrMode(aircraft.getTransponderMode());
                m_service->setCom1Active(m_xplaneData.com1Active);
                m_service->setCom1Standby(m_xplaneData.com1Standby);
                m_service->setCom2Active(m_xplaneData.com2Active);
                m_service->setCom2Standby(m_xplaneData.com2Standby);
                m_service->setTransponderCode(m_xplaneData.xpdrCode);
                m_service->setTransponderMode(m_xplaneData.xpdrMode);

                m_service->cancelAllPendingAsyncCalls(); // in case there is already a reply with some old data incoming
                return true;
            }
            return false;
        }

    }
}
