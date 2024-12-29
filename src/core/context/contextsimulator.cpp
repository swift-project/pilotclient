// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextsimulator.h"

#include <QFlag>

#include "core/context/contextsimulatorempty.h"
#include "core/context/contextsimulatorimpl.h"
#include "core/context/contextsimulatorproxy.h"
#include "misc/dbusserver.h"
#include "misc/pq/units.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;

namespace swift::core::context
{
    const QString &IContextSimulator::InterfaceName()
    {
        static const QString s(SWIFT_CORE_CONTEXTSIMULATOR_INTERFACENAME);
        return s;
    }

    const QString &IContextSimulator::ObjectPath()
    {
        static const QString s(SWIFT_CORE_CONTEXTSIMULATOR_OBJECTPATH);
        return s;
    }

    IContextSimulator *IContextSimulator::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                                 CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local: return new CContextSimulator(mode, parent);
        case CCoreFacadeConfig::LocalInDBusServer:
        {
            auto *context = new CContextSimulator(mode, parent);
            context->registerWithDBus(server);
            return context;
        }
        case CCoreFacadeConfig::Remote:
            return new CContextSimulatorProxy(CDBusServer::coreServiceName(connection), connection, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default: return new CContextSimulatorEmpty(parent);
        }
    }

    CSimulatorInfo IContextSimulator::getSimulatorInfo() const
    {
        return this->getSimulatorPluginInfo().getSimulatorInfo();
    }

    bool IContextSimulator::isSimulatorAvailable() const
    {
        return CBuildConfig::isCompiledWithFlightSimulatorSupport() && !this->getSimulatorPluginInfo().isUnspecified();
    }

    bool IContextSimulator::isSimulatorSimulating() const
    {
        if (!isSimulatorAvailable() || !getSimulatorStatus().testFlag(ISimulator::Simulating)) { return false; }
        return true;
    }
} // namespace swift::core::context
