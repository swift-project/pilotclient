/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network.h"
#include "context_network_impl.h"
#include "context_network_proxy.h"

namespace BlackCore
{

    IContextNetwork *IContextNetwork::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextNetwork(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextNetworkProxy(BlackCore::CDBusServer::ServiceName, conn, mode, parent);
        default:
            return nullptr; // network not mandatory
        }
    }

}
