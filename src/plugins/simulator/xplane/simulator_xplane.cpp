/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_xplane.h"
#include "xbus_service_proxy.h"
#include <QDBusServiceWatcher>

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
            if (! m_service) { return {}; }
            using namespace BlackMisc;
            using namespace BlackMisc::PhysicalQuantities;
            Aviation::CAircraftSituation situation;
            situation.setPosition({ m_service->getLatitude(), m_service->getLongitude(), 0 });
            situation.setAltitude({ m_service->getAltitudeMSL(), Aviation::CAltitude::MeanSeaLevel, CLengthUnit::m() });
            situation.setHeading({ m_service->getTrueHeading(), Aviation::CHeading::True, CAngleUnit::deg() });
            situation.setPitch({ m_service->getPitch(), CAngleUnit::deg() });
            situation.setBank({ m_service->getRoll(), CAngleUnit::deg() });
            situation.setGroundspeed({ m_service->getGroundSpeed(), CSpeedUnit::m_s() });
            Aviation::CAircraft ac { {}, {}, situation };
            ac.setIcaoInfo(Aviation::CAircraftIcao { m_service->getAircraftIcaoCode() });
            ac.setCom1System(Aviation::CComSystem::getCom1System({ m_service->getCom1Active(), CFrequencyUnit::kHz() }, { m_service->getCom1Standby(), CFrequencyUnit::kHz() }));
            ac.setCom2System(Aviation::CComSystem::getCom2System({ m_service->getCom2Active(), CFrequencyUnit::kHz() }, { m_service->getCom2Standby(), CFrequencyUnit::kHz() }));
            ac.setTransponder(Aviation::CTransponder::getStandardTransponder(m_service->getTransponderCode(), xpdrMode(m_service->getTransponderMode(), m_service->getTransponderIdent())));
            return ac;
        }

        void CSimulatorXPlane::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            if (! m_service) { return; }
            // TODO
            Q_UNUSED(message);
        }

        BlackMisc::Network::CAircraftModel CSimulatorXPlane::getAircraftModel() const
        {
            if (! m_service) { return {}; }
            return m_service->getAircraftModelPath();
        }

        bool CSimulatorXPlane::updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft)
        {
            if (! m_service) { return false; }
            using namespace BlackMisc;
            using namespace BlackMisc::PhysicalQuantities;
            auto com1 = Aviation::CComSystem::getCom1System({ m_service->getCom1Active(), CFrequencyUnit::kHz() }, { m_service->getCom1Standby(), CFrequencyUnit::kHz() });
            auto com2 = Aviation::CComSystem::getCom2System({ m_service->getCom2Active(), CFrequencyUnit::kHz() }, { m_service->getCom2Standby(), CFrequencyUnit::kHz() });
            auto xpdr = Aviation::CTransponder::getStandardTransponder(m_service->getTransponderCode(), xpdrMode(m_service->getTransponderMode(), m_service->getTransponderIdent()));
            if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
            {
                m_service->setCom1Active(aircraft.getCom1System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0));
                m_service->setCom1Standby(aircraft.getCom1System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0));
                m_service->setCom2Active(aircraft.getCom2System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0));
                m_service->setCom2Standby(aircraft.getCom2System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0));
                m_service->setTransponderCode(aircraft.getTransponderCode());
                m_service->setTransponderMode(xpdrMode(aircraft.getTransponderMode()));
                return true;
            }
            return false;
        }

    }
}
