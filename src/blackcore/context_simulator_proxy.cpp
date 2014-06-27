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
using namespace BlackSim;

namespace BlackCore
{

    CContextSimulatorProxy::CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    void CContextSimulatorProxy::relaySignals(const QString &/*serviceName*/, QDBusConnection &/*connection*/)
    { }

    CSimulatorInfoList CContextSimulatorProxy::getAvailableSimulatorPlugins() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorInfoList>(QLatin1Literal("getAvailableSimulatorPlugins"));
    }

    bool CContextSimulatorProxy::isConnected() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    bool CContextSimulatorProxy::canConnect()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("canConnect"));
    }

    bool CContextSimulatorProxy::connectTo()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("connectTo"));
    }

    void CContextSimulatorProxy::asyncConnectTo()
    {
        m_dBusInterface->callDBus(QLatin1Literal("asyncConnectTo"));
    }

    bool CContextSimulatorProxy::disconnectFrom()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("disconnectFrom"));
    }

    BlackMisc::Network::CAircraftModel CContextSimulatorProxy::getOwnAircraftModel() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Network::CAircraftModel>(QLatin1Literal("getOwnAircraftModel"));
    }

    BlackSim::CSimulatorInfo CContextSimulatorProxy::getSimulatorInfo() const
    {
        return m_dBusInterface->callDBusRet<BlackSim::CSimulatorInfo>(QLatin1Literal("getSimulatorInfo"));
    }

    bool CContextSimulatorProxy::loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("loadSimulatorPlugin"), simulatorInfo);
    }

    bool CContextSimulatorProxy::loadSimulatorPluginFromSettings()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("loadSimulatorPluginFromSettings"));
    }

    void CContextSimulatorProxy::unloadSimulatorPlugin()
    {
        m_dBusInterface->callDBus(QLatin1Literal("unloadSimulatorPlugin"));
    }

    void CContextSimulatorProxy::settingsChanged(uint type)
    {
        m_dBusInterface->callDBus(QLatin1Literal("settingsChanged"), type);
    }

} // namespace BlackCore
