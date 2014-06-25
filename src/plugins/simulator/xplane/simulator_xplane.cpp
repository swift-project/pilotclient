/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_xplane.h"
#include "xbus_service_proxy.h"
#include "xbus_traffic_proxy.h"
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
            m_watcher->addWatchedService(CXBusTrafficProxy::InterfaceName());
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
            return m_service && m_traffic;
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
            m_traffic = new CXBusTrafficProxy(m_conn, this);
            if (m_service->isValid() && m_traffic->isValid() && m_traffic->initialize())
            {
                connect(m_service, &CXBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::emitAircraftModelChanged);
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
            if (m_traffic)
            {
                m_traffic->cleanup();
            }
            emit statusChanged(ISimulator::Disconnected);
            m_conn = QDBusConnection { "default" };
            m_watcher->setConnection(m_conn);
            delete m_service;
            delete m_traffic;
            m_service = nullptr;
            m_traffic = nullptr;
            return true;
        }

        void CSimulatorXPlane::serviceRegistered(const QString &serviceName)
        {
            if (serviceName == CXBusServiceProxy::InterfaceName())
            {
                delete m_service;
                m_service = new CXBusServiceProxy(m_conn, this);
                connect(m_service, &CXBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::emitAircraftModelChanged);
            }
            else if (serviceName == CXBusTrafficProxy::InterfaceName())
            {
                delete m_traffic;
                m_traffic = new CXBusTrafficProxy(m_conn, this);
            }
            if (m_service && m_traffic)
            {
                emit statusChanged(ISimulator::Connected);
            }
        }

        void CSimulatorXPlane::serviceUnregistered()
        {
            delete m_service;
            delete m_traffic;
            m_service = nullptr;
            m_traffic = nullptr;
            emit statusChanged(ISimulator::Disconnected);
        }

        void CSimulatorXPlane::emitAircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao)
        {
            emit aircraftModelChanged({ path, true });
            Q_UNUSED(filename);
            Q_UNUSED(livery);
            Q_UNUSED(icao);
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
            return { m_xplaneData.aircraftModelPath, true };
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

        void CSimulatorXPlane::addRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign, const QString &type,
            const BlackMisc::Aviation::CAircraftSituation &initialSituation)
        {
            if (! isConnected()) { return; }
            m_traffic->addPlane(callsign.asString(), type, "YYY", "YYY"); // TODO livery
            addAircraftSituation(callsign, initialSituation);
        }

        void CSimulatorXPlane::addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign,
            const BlackMisc::Aviation::CAircraftSituation &situ)
        {
            if (! isConnected()) { return; }
            using namespace BlackMisc::PhysicalQuantities;
            m_traffic->setPlanePosition(callsign.asString(),
                situ.latitude().value(CAngleUnit::deg()),
                situ.longitude().value(CAngleUnit::deg()),
                situ.getAltitude().value(CLengthUnit::ft()),
                situ.getPitch().value(CAngleUnit::deg()),
                situ.getBank().value(CAngleUnit::deg()),
                situ.getHeading().value(CAngleUnit::deg()));
            m_traffic->setPlaneSurfaces(callsign.asString(), true, 0, 0, 0, 0, 0, 0, 0, 0, 0, true, true, true, true, 0); // TODO landing gear, lights, control surfaces
            m_traffic->setPlaneTransponder(callsign.asString(), 2000, true, false); // TODO transponder
        }

        void CSimulatorXPlane::removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign)
        {
            if (! isConnected()) { return; }
            m_traffic->removePlane(callsign.asString());
        }

    }
}
