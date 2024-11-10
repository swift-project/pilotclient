// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextownaircraftproxy.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/dbus.h"
#include "misc/dbusserver.h"
#include "misc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QObject>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;

namespace swift::core::context
{
    CContextOwnAircraftProxy::CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr)
    {
        m_dBusInterface = new swift::misc::CGenericDBusInterface(
            serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    void CContextOwnAircraftProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                    "changedAircraftCockpit", this, SIGNAL(changedAircraftCockpit(swift::misc::simulation::CSimulatedAircraft, swift::misc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                               "changedSelcal", this, SIGNAL(changedSelcal(swift::misc::aviation::CSelcal, swift::misc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                               "changedCallsign", this, SIGNAL(changedCallsign(swift::misc::aviation::CCallsign)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                               "changedAircraftIcaoCodes", this, SIGNAL(changedAircraftIcaoCodes(swift::misc::aviation::CAircraftIcaoCode, swift::misc::aviation::CAirlineIcaoCode)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                               "changedPilot", this, SIGNAL(changedPilot(swift::misc::network::CUser)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                               "movedAircraft", this, SIGNAL(movedAircraft(swift::misc::physical_quantities::CLength)));
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

    swift::misc::simulation::CSimulatedAircraft CContextOwnAircraftProxy::getOwnAircraft() const
    {
        return m_dBusInterface->callDBusRet<swift::misc::simulation::CSimulatedAircraft>(QLatin1String("getOwnAircraft"));
    }

    CComSystem CContextOwnAircraftProxy::getOwnComSystem(CComSystem::ComUnit unit) const
    {
        return m_dBusInterface->callDBusRet<swift::misc::aviation::CComSystem>(QLatin1String("getOwnComSystem"), unit);
    }

    CTransponder CContextOwnAircraftProxy::getOwnTransponder() const
    {
        return m_dBusInterface->callDBusRet<swift::misc::aviation::CTransponder>(QLatin1String("getOwnTransponder"));
    }

    CAircraftSituation CContextOwnAircraftProxy::getOwnAircraftSituation() const
    {
        return m_dBusInterface->callDBusRet<swift::misc::aviation::CAircraftSituation>(QLatin1String("getOwnAircraftSituation"));
    }

    bool CContextOwnAircraftProxy::updateCockpit(const swift::misc::aviation::CComSystem &com1, const swift::misc::aviation::CComSystem &com2, const swift::misc::aviation::CTransponder &transponder, const CIdentifier &originator)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateCockpit"), com1, com2, transponder, originator);
    }

    bool CContextOwnAircraftProxy::updateTransponderMode(const CTransponder::TransponderMode &transponderMode, const CIdentifier &originator)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateTransponderMode"), transponderMode, originator);
    }

    bool CContextOwnAircraftProxy::updateActiveComFrequency(const physical_quantities::CFrequency &frequency, swift::misc::aviation::CComSystem::ComUnit comUnit, const CIdentifier &originator)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateActiveComFrequency"), frequency, comUnit, originator);
    }

    bool CContextOwnAircraftProxy::updateOwnAircraftPilot(const swift::misc::network::CUser &pilot)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnAircraftPilot"), pilot);
    }

    bool CContextOwnAircraftProxy::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateSelcal"), selcal, originator);
    }

    bool CContextOwnAircraftProxy::updateOwnPosition(const swift::misc::geo::CCoordinateGeodetic &position, const swift::misc::aviation::CAltitude &altitude, const CAltitude &pressureAltitude)
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
