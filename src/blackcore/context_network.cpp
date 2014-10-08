/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context_network.h"
#include "context_network_impl.h"
#include "context_network_proxy.h"
#include "context_network_empty.h"

namespace BlackCore
{

    IContextNetwork *IContextNetwork::create(CRuntime *runtime, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextNetwork(mode, runtime))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextNetworkProxy(BlackCore::CDBusServer::ServiceName, conn, mode, runtime);
        case CRuntimeConfig::NotUsed:
        default:
            return new BlackCore::CContextNetworkEmpty(true, runtime);
        }
    }

} // namesapce
