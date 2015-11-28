/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "contextownaircraft.h"
#include "contextownaircraftimpl.h"
#include "contextownaircraftproxy.h"
#include "contextownaircraftempty.h"

namespace BlackCore
{
    IContextOwnAircraft *IContextOwnAircraft::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextOwnAircraft(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new CContextOwnAircraftProxy(BlackMisc::CDBusServer::ServiceName(), conn, mode, parent);
        case CRuntimeConfig::NotUsed:
        default:
            return new CContextOwnAircraftEmpty(parent);
        }
    }
} // namespace
