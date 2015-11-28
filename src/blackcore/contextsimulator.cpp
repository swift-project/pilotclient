/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "contextsimulator.h"
#include "contextsimulatorimpl.h"
#include "contextsimulatorproxy.h"
#include "contextsimulatorempty.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
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

    const BlackMisc::PhysicalQuantities::CTime &IContextSimulator::HighlightTime()
    {
        static const CTime t(10.0, CTimeUnit::s());
        return t;
    }

    IContextSimulator *IContextSimulator::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextSimulator(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new CContextSimulatorProxy(BlackMisc::CDBusServer::ServiceName(), conn, mode, parent);
        case CRuntimeConfig::NotUsed:
        default:
            return new CContextSimulatorEmpty(parent);
        }
    }

    ISimulator::SimulatorStatus IContextSimulator::getSimulatorStatusEnum() const
    {
        return static_cast<ISimulator::SimulatorStatus>(this->getSimulatorStatus());
    }

} // namespace
