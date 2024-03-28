// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextsimulatorempty.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/context/contextsimulatorproxy.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/pq/units.h"

#include <QFlag>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore::Context
{
    const QString &IContextSimulator::InterfaceName()
    {
        static const QString s(BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME);
        return s;
    }

    const QString &IContextSimulator::ObjectPath()
    {
        static const QString s(BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH);
        return s;
    }

    IContextSimulator *IContextSimulator::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local:
        case CCoreFacadeConfig::LocalInDBusServer:
            return (new CContextSimulator(mode, parent))->registerWithDBus(server);
        case CCoreFacadeConfig::Remote:
            return new CContextSimulatorProxy(CDBusServer::coreServiceName(connection), connection, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default:
            return new CContextSimulatorEmpty(parent);
        }
    }

    ISimulator::SimulatorStatus IContextSimulator::getSimulatorStatusEnum() const
    {
        return static_cast<ISimulator::SimulatorStatus>(this->getSimulatorStatus());
    }

    CSimulatorInfo IContextSimulator::getSimulatorInfo() const
    {
        return this->getSimulatorPluginInfo().getSimulatorInfo();
    }

    bool IContextSimulator::updateCurrentSettings(const Simulation::Settings::CSimulatorSettings &settings)
    {
        const CSimulatorInfo sim = this->getSimulatorInfo();
        if (!sim.isSingleSimulator()) { return false; }
        return this->setSimulatorSettings(settings, sim);
    }

    bool IContextSimulator::updateCurrentSettingComIntegration(bool comIntegration)
    {
        Simulation::Settings::CSimulatorSettings settings = this->getSimulatorSettings();
        settings.setComIntegrated(comIntegration);
        return this->updateCurrentSettings(settings);
    }

    bool IContextSimulator::isSimulatorAvailable() const
    {
        return CBuildConfig::isCompiledWithFlightSimulatorSupport() && !this->getSimulatorPluginInfo().isUnspecified();
    }

    bool IContextSimulator::isSimulatorSimulating() const
    {
        if (!isSimulatorAvailable() || !getSimulatorStatusEnum().testFlag(ISimulator::Simulating)) { return false; }
        return true;
    }
} // namespace
