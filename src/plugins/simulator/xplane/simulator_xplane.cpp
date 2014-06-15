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

        BlackMisc::Aviation::CAircraft CSimulatorXPlane::getOwnAircraft() const
        {
            if (! m_service) { return {}; }
            using namespace BlackMisc;
            Aviation::CAircraftSituation situation;
            situation.setPosition({ m_service->getLatitude(), m_service->getLongitude(), 0 });
            situation.setAltitude({ m_service->getAltitudeMSL(), Aviation::CAltitude::MeanSeaLevel, PhysicalQuantities::CLengthUnit::m() });
            situation.setHeading({ m_service->getTrueHeading(), Aviation::CHeading::True, PhysicalQuantities::CAngleUnit::deg() });
            situation.setPitch({ m_service->getPitch(), PhysicalQuantities::CAngleUnit::deg() });
            situation.setBank({ m_service->getRoll(), PhysicalQuantities::CAngleUnit::deg() });
            situation.setGroundspeed({ m_service->getGroundSpeed(), PhysicalQuantities::CSpeedUnit::m_s() });
            return { {}, {}, situation };
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
            //TODO
            Q_UNUSED(aircraft);
            return false;
        }

    }
}
