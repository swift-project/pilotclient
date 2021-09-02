/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextownaircraftproxy.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QObject>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Context
    {
        CContextOwnAircraftProxy::CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr)
        {
            m_dBusInterface = new BlackMisc::CGenericDBusInterface(
                serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                connection, this);
            this->relaySignals(serviceName, connection);
        }

        void CContextOwnAircraftProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
        {
            bool s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                        "changedAircraftCockpit", this, SIGNAL(changedAircraftCockpit(BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::CIdentifier)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "changedSelcal", this, SIGNAL(changedSelcal(BlackMisc::Aviation::CSelcal, BlackMisc::CIdentifier)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "changedCallsign", this, SIGNAL(changedCallsign(BlackMisc::Aviation::CCallsign)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "changedAircraftIcaoCodes", this, SIGNAL(changedAircraftIcaoCodes(BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAirlineIcaoCode)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "changedPilot", this, SIGNAL(changedPilot(BlackMisc::Network::CUser)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "movedAircraft", this, SIGNAL(movedAircraft(BlackMisc::PhysicalQuantities::CLength)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "isTakingOff", this, SIGNAL(isTakingOff()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                   "isTouchingDown", this, SIGNAL(isTouchingDown()));
            Q_ASSERT(s);

            this->relayBaseClassSignals(serviceName, connection, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName());
            Q_UNUSED(s)
        }

        void CContextOwnAircraftProxy::unitTestRelaySignals()
        {
            // connect signals, asserts when failures
            QDBusConnection con = QDBusConnection::sessionBus();
            CContextOwnAircraftProxy c(CDBusServer::coreServiceName(), con, CCoreFacadeConfig::Remote, nullptr);
            Q_UNUSED(c)
        }

        BlackMisc::Simulation::CSimulatedAircraft CContextOwnAircraftProxy::getOwnAircraft() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraft>(QLatin1String("getOwnAircraft"));
        }

        CComSystem CContextOwnAircraftProxy::getOwnComSystem(CComSystem::ComUnit unit) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CComSystem>(QLatin1String("getOwnComSystem"), unit);
        }

        CTransponder CContextOwnAircraftProxy::getOwnTransponder() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CTransponder>(QLatin1String("getOwnTransponder"));
        }

        CAircraftSituation CContextOwnAircraftProxy::getOwnAircraftSituation() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraftSituation>(QLatin1String("getOwnAircraftSituation"));
        }

        bool CContextOwnAircraftProxy::updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateCockpit"), com1, com2, transponder, originator);
        }

        bool CContextOwnAircraftProxy::updateTransponderMode(const CTransponder::TransponderMode &transponderMode, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateTransponderMode"), transponderMode, originator);
        }

        bool CContextOwnAircraftProxy::updateActiveComFrequency(const PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateActiveComFrequency"), frequency, comUnit, originator);
        }

        bool CContextOwnAircraftProxy::updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnAircraftPilot"), pilot);
        }

        bool CContextOwnAircraftProxy::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateSelcal"), selcal, originator);
        }

        bool CContextOwnAircraftProxy::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const CAltitude &pressureAltitude)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnPosition"), position, altitude, pressureAltitude);
        }

        bool CContextOwnAircraftProxy::updateOwnCallsign(const CCallsign &callsign)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnCallsign"), callsign);
        }

        bool CContextOwnAircraftProxy::updateOwnIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnIcaoCodes"), aircraftIcaoCode, airlineIcaoCode);
        }

        void CContextOwnAircraftProxy::toggleTransponderMode()
        {
            m_dBusInterface->callDBus(QLatin1String("toggleTransponderMode"));
        }

        bool CContextOwnAircraftProxy::setTransponderMode(CTransponder::TransponderMode mode)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("setTransponderMode"), mode);
        }

        bool CContextOwnAircraftProxy::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("parseCommandLine"), commandLine, originator);
        }
    } // namespace
} // namespace
