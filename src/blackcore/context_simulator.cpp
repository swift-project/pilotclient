/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator.h"
#include "context_simulator_impl.h"
#include "context_simulator_proxy.h"

namespace BlackCore
{

    IContextSimulator *IContextSimulator::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextSimulator(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextSimulatorProxy(BlackCore::CDBusServer::ServiceName, conn, mode, parent);
        default:
            return nullptr; // simulator not mandatory
        }
    }
} // namespace
