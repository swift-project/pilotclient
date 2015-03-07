/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
            return new BlackCore::CContextOwnAircraftProxy(BlackCore::CDBusServer::ServiceName(), conn, mode, parent);
        default:
            qFatal("Always initialize an ownAircraft context");
            return nullptr;
        }
    }
} // namespace
