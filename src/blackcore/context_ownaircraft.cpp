/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_ownaircraft.h"
#include "context_ownaircraft_impl.h"
#include "context_ownaircraft_proxy.h"

namespace BlackCore
{

    IContextOwnAircraft *IContextOwnAircraft::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextOwnAircraft(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextOwnAircraftProxy(BlackCore::CDBusServer::ServiceName, conn, mode, parent);
        default:
            qFatal("Always initialize an ownAircraft context");
            return nullptr;
        }
    }
} // namespace
