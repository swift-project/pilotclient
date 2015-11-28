/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "contextnetwork.h"
#include "contextnetworkimpl.h"
#include "contextnetworkproxy.h"
#include "contextnetworkempty.h"

namespace BlackCore
{

    IContextNetwork *IContextNetwork::create(CRuntime *runtime, CRuntimeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextNetwork(mode, runtime))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new CContextNetworkProxy(BlackMisc::CDBusServer::ServiceName(), connection, mode, runtime);
        case CRuntimeConfig::NotUsed:
        default:
            return new CContextNetworkEmpty(runtime);
        }
    }

} // namesapce
