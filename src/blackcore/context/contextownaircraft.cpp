/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftempty.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextownaircraftproxy.h"
#include "blackmisc/dbusserver.h"

namespace BlackCore
{
    namespace Context
    {
        IContextOwnAircraft *IContextOwnAircraft::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &conn)
        {
            switch (mode)
            {
            case CCoreFacadeConfig::Local:
            case CCoreFacadeConfig::LocalInDbusServer:
                return (new CContextOwnAircraft(mode, parent))->registerWithDBus(server);
            case CCoreFacadeConfig::Remote:
                return new CContextOwnAircraftProxy(BlackMisc::CDBusServer::coreServiceName(), conn, mode, parent);
            case CCoreFacadeConfig::NotUsed:
            default:
                return new CContextOwnAircraftEmpty(parent);
            }
        }
    } // namespace
} // namespace
