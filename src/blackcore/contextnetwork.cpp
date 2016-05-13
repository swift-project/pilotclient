/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextnetwork.h"
#include "blackcore/contextnetworkempty.h"
#include "blackcore/contextnetworkimpl.h"
#include "blackcore/contextnetworkproxy.h"
#include "blackmisc/dbusserver.h"

namespace BlackCore
{

    IContextNetwork *IContextNetwork::create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local:
        case CCoreFacadeConfig::LocalInDbusServer:
            return (new CContextNetwork(mode, runtime))->registerWithDBus(server);
        case CCoreFacadeConfig::Remote:
            return new CContextNetworkProxy(BlackMisc::CDBusServer::coreServiceName(), connection, mode, runtime);
        case CCoreFacadeConfig::NotUsed:
        default:
            return new CContextNetworkEmpty(runtime);
        }
    }

} // namesapce
