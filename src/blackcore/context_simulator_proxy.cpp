/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_simulator_proxy.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
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

    void CContextSimulatorProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                                    "simulatorStatusChanged", this, SIGNAL(simulatorStatusChanged(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "simulatorStatusChanged", this, SIGNAL(simulatorStatusChanged(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "installedAircraftModelsChanged", this, SIGNAL(installedAircraftModelsChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "ownAircraftModelChanged", this, SIGNAL(ownAircraftModelChanged(BlackMisc::Simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "modelMatchingCompleted", this, SIGNAL(modelMatchingCompleted(BlackMisc::Simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "restrictedRenderingChanged", this, SIGNAL(restrictedRenderingChanged(bool)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    CSimulatorInfoList CContextSimulatorProxy::getAvailableSimulatorPlugins() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorInfoList>(QLatin1Literal("getAvailableSimulatorPlugins"));
    }

    bool CContextSimulatorProxy::isConnected() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    bool CContextSimulatorProxy::canConnect() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("canConnect"));
    }

    bool CContextSimulatorProxy::connectToSimulator()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("connectToSimulator"));
    }

    void CContextSimulatorProxy::asyncConnectToSimulator()
    {
        m_dBusInterface->callDBus(QLatin1Literal("asyncConnectToSimulator"));
    }

    bool CContextSimulatorProxy::disconnectFromSimulator()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("disconnectFromSimulator"));
    }

    CAirportList CContextSimulatorProxy::getAirportsInRange() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAirportList>(QLatin1Literal("getAirportsInRange"));
    }

    CAircraftModelList CContextSimulatorProxy::getInstalledModels() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CAircraftModelList>(QLatin1Literal("getInstalledModels"));
    }

    CAircraftModelList CContextSimulatorProxy::getInstalledModelsStartingWith(const QString modelString) const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CAircraftModelList>(QLatin1Literal("getInstalledModelsStartingWith"), modelString);
    }

    int CContextSimulatorProxy::getInstalledModelsCount() const
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1Literal("getInstalledModelsCount"));
    }

    void CContextSimulatorProxy::reloadInstalledModels()
    {
        m_dBusInterface->callDBus(QLatin1Literal("reloadInstalledModels"));
    }

    CAircraftIcao CContextSimulatorProxy::getIcaoForModelString(const QString &modelString) const
    {
        return m_dBusInterface->callDBusRet<CAircraftIcao>(QLatin1Literal("getIcaoForModelString"), modelString);
    }

    BlackSim::CSimulatorInfo CContextSimulatorProxy::getSimulatorInfo() const
    {
        return m_dBusInterface->callDBusRet<BlackSim::CSimulatorInfo>(QLatin1Literal("getSimulatorInfo"));
    }

    bool CContextSimulatorProxy::setTimeSynchronization(bool enable, CTime offset)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("setTimeSynchronization"), enable, offset);
    }

    bool CContextSimulatorProxy::isTimeSynchronized() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isTimeSynchronized"));
    }

    void CContextSimulatorProxy::setMaxRenderedAircraft(int number)
    {
        m_dBusInterface->callDBus(QLatin1Literal("setMaxRenderedAircraft"), number);
    }

    void CContextSimulatorProxy::setMaxRenderedDistance(CLength &distance)
    {
        m_dBusInterface->callDBus(QLatin1Literal("setMaxRenderedDistance"), distance);
    }

    void CContextSimulatorProxy::deleteAllRenderingRestrictions()
    {
        m_dBusInterface->callDBus(QLatin1Literal("deleteAllRenderingRestrictions"));
    }

    bool CContextSimulatorProxy::isRenderingRestricted() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isRenderingRestricted"));
    }

    CLength CContextSimulatorProxy::getMaxRenderedDistance() const
    {
        return m_dBusInterface->callDBusRet<CLength>(QLatin1Literal("getMaxRenderedDistance"));
    }

    CLength CContextSimulatorProxy::getRenderedDistanceBoundary() const
    {
        return m_dBusInterface->callDBusRet<CLength>(QLatin1Literal("getRenderedDistanceBoundary"));
    }

    QString CContextSimulatorProxy::getRenderRestrictionText() const
    {
        return m_dBusInterface->callDBusRet<QString>(QLatin1Literal("getRenderRestrictionText"));
    }

    int CContextSimulatorProxy::getMaxRenderedAircraft() const
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1Literal("getMaxRenderedAircraft"));
    }

    CTime CContextSimulatorProxy::getTimeSynchronizationOffset() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::PhysicalQuantities::CTime>(QLatin1Literal("getTimeSynchronizationOffset"));
    }

    bool CContextSimulatorProxy::loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("loadSimulatorPlugin"), simulatorInfo);
    }

    bool CContextSimulatorProxy::loadSimulatorPluginFromSettings()
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("loadSimulatorPluginFromSettings"));
    }
    
    void CContextSimulatorProxy::listenForSimulator(const CSimulatorInfo &simulatorInfo)
    {
        m_dBusInterface->callDBus(QLatin1Literal("listenForSimulator"), simulatorInfo);
    }
    
    void CContextSimulatorProxy::listenForSimulatorFromSettings()
    {
        m_dBusInterface->callDBus(QLatin1Literal("listenForSimulatorFromSettings"));
    }

    void CContextSimulatorProxy::unloadSimulatorPlugin()
    {
        m_dBusInterface->callDBus(QLatin1Literal("unloadSimulatorPlugin"));
    }

    void CContextSimulatorProxy::settingsChanged(uint type)
    {
        m_dBusInterface->callDBus(QLatin1Literal("settingsChanged"), type);
    }

    CPixmap CContextSimulatorProxy::iconForModel(const QString &modelString) const
    {
        return m_dBusInterface->callDBusRet<CPixmap>(QLatin1Literal("iconForModel"), modelString);
    }

    void CContextSimulatorProxy::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        m_dBusInterface->callDBus(QLatin1Literal("highlightAircraft"), aircraftToHighlight, enableHighlight, displayTime);
    }

    void CContextSimulatorProxy::enableDebugMessages(bool driver, bool interpolator)
    {
        m_dBusInterface->callDBus(QLatin1Literal("enableDebugMessages"), driver, interpolator);
    }

    bool CContextSimulatorProxy::isPaused() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isPaused"));
    }

    bool CContextSimulatorProxy::isSimulating() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isRunning"));
    }

} // namespace
