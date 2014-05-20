/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_ownaircraft_proxy.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    CContextOwnAircraftProxy::CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void CContextOwnAircraftProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
//        bool s; = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
//                                    "connectionStatusChanged", this, SIGNAL(connectionStatusChanged(uint, uint,  const QString &)));
//        Q_ASSERT(s);
//        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
//                               "changedAtcStationsBooked", this, SIGNAL(changedAtcStationsBooked()));
//        Q_ASSERT(s);
//        Q_UNUSED(s);
    }

    BlackMisc::Aviation::CAircraft CContextOwnAircraftProxy::getOwnAircraft() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraft>(QLatin1Literal("getOwnAircraft"));
    }

    BlackMisc::CStatusMessageList CContextOwnAircraftProxy::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("setOwnAircraft"), aircraft);
    }

    void CContextOwnAircraftProxy::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnPosition"), position, altitude);
    }

    void CContextOwnAircraftProxy::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnSituation"), situation);
    }

    void CContextOwnAircraftProxy::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnCockpit"), com1, com2, transponder);
    }
} // namespace
