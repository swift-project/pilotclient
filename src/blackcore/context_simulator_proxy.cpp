/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_simulator_proxy.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackCore
{

    // Constructor for DBus
    CContextSimulatorProxy::CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, QObject *parent) : IContextSimulator(parent), m_dBusInterface(0)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    // Workaround for signals, not working without, but why?
    void CContextSimulatorProxy::relaySignals(const QString &/*serviceName*/, QDBusConnection &/*connection*/)
    {
    }

    bool CContextSimulatorProxy::isConnected() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    BlackMisc::Aviation::CAircraft CContextSimulatorProxy::ownAircraft() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraft>(QLatin1Literal("ownAircraft"));
    }

} // namespace BlackCore
