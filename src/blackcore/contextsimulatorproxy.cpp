/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextsimulatorproxy.h"
#include "blackmisc/dbus.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QDBusConnection>
#include <QLatin1Literal>
#include <QObject>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;

namespace BlackCore
{

    CContextSimulatorProxy::CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextSimulator(mode, runtime), m_dBusInterface(nullptr)
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
                               "installedAircraftModelsChanged", this, SIGNAL(installedAircraftModelsChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "ownAircraftModelChanged", this, SIGNAL(ownAircraftModelChanged(BlackMisc::Simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "modelMatchingCompleted", this, SIGNAL(modelMatchingCompleted(BlackMisc::Simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "renderRestrictionsChanged", this, SIGNAL(renderRestrictionsChanged(bool, bool, int, BlackMisc::PhysicalQuantities::CLength, BlackMisc::PhysicalQuantities::CLength)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "simulatorPluginChanged", this, SIGNAL(simulatorPluginChanged(BlackMisc::Simulation::CSimulatorPluginInfo)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "airspaceSnapshotHandled", this, SIGNAL(airspaceSnapshotHandled()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "weatherGridReceived", this, SIGNAL(weatherGridReceived()));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    CSimulatorPluginInfoList CContextSimulatorProxy::getAvailableSimulatorPlugins() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorPluginInfoList>(QLatin1Literal("getAvailableSimulatorPlugins"));
    }

    int CContextSimulatorProxy::getSimulatorStatus() const
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1Literal("getSimulatorStatus"));
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

    BlackMisc::Simulation::CSimulatorPluginInfo CContextSimulatorProxy::getSimulatorPluginInfo() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatorPluginInfo>(QLatin1Literal("getSimulatorPluginInfo"));
    }

    CSimulatorSetup CContextSimulatorProxy::getSimulatorSetup() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatorSetup>(QLatin1Literal("getSimulatorSetup"));
    }

    bool CContextSimulatorProxy::setTimeSynchronization(bool enable, const CTime &offset)
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

    void CContextSimulatorProxy::setMaxRenderedDistance(const CLength &distance)
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

    bool CContextSimulatorProxy::isRenderingEnabled() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isRenderingEnabled"));
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

    bool CContextSimulatorProxy::startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("startSimulatorPlugin"), simulatorInfo);
    }

    void CContextSimulatorProxy::stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo)
    {
        m_dBusInterface->callDBus(QLatin1Literal("stopSimulatorPlugin"), simulatorInfo);
    }

    CPixmap CContextSimulatorProxy::iconForModel(const QString &modelString) const
    {
        return m_dBusInterface->callDBusRet<CPixmap>(QLatin1Literal("iconForModel"), modelString);
    }

    void CContextSimulatorProxy::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        m_dBusInterface->callDBus(QLatin1Literal("highlightAircraft"), aircraftToHighlight, enableHighlight, displayTime);
    }

    void CContextSimulatorProxy::requestWeatherGrid(const Weather::CWeatherGrid &weatherGrid, const CIdentifier &identifier)
    {
        m_dBusInterface->callDBus(QLatin1Literal("requestWeatherGrid"), weatherGrid, identifier);
    }

    void CContextSimulatorProxy::enableDebugMessages(bool driver, bool interpolator)
    {
        m_dBusInterface->callDBus(QLatin1Literal("enableDebugMessages"), driver, interpolator);
    }

} // namespace
