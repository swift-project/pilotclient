/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextsimulator.h"
#include "blackcore/contextsimulatorempty.h"
#include "blackcore/contextsimulatorimpl.h"
#include "blackcore/contextsimulatorproxy.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/pq/units.h"

#include <QFlag>

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

    IContextSimulator *IContextSimulator::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local:
        case CCoreFacadeConfig::LocalInDbusServer:
            return (new CContextSimulator(mode, parent))->registerWithDBus(server);
        case CCoreFacadeConfig::Remote:
            return new CContextSimulatorProxy(BlackMisc::CDBusServer::coreServiceName(), conn, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default:
            return new CContextSimulatorEmpty(parent);
        }
    }

    ISimulator::SimulatorStatus IContextSimulator::getSimulatorStatusEnum() const
    {
        return static_cast<ISimulator::SimulatorStatus>(this->getSimulatorStatus());
    }

} // namespace
