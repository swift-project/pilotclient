// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextnetwork.h"

#include "config/buildconfig.h"
#include "core/application.h"
#include "core/context/contextnetworkempty.h"
#include "core/context/contextnetworkimpl.h"
#include "core/context/contextnetworkproxy.h"
#include "misc/dbusserver.h"

using namespace swift::config;
using namespace swift::core;
using namespace swift::misc;

namespace swift::core::context
{
    IContextNetwork *IContextNetwork::create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode,
                                             CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local: return new CContextNetwork(mode, runtime);
        case CCoreFacadeConfig::LocalInDBusServer:
        {
            auto *context = new CContextNetwork(mode, runtime);
            context->registerWithDBus(ObjectPath(), server);
            return context;
        }
        case CCoreFacadeConfig::Remote:
            return new CContextNetworkProxy(CDBusServer::coreServiceName(connection), connection, mode, runtime);
        case CCoreFacadeConfig::NotUsed:
        default: return new CContextNetworkEmpty(runtime);
        }
    }

#ifdef SWIFT_VATSIM_SUPPORT
    bool IContextNetwork::getEnvironmentClientIdAndKey(int &id, QString &key)
    {
        // init values
        id = 0;
        key = "";

        // only allow overwrite via environment variable in dev builds
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return false; }

        const QString clientIdAsString = qEnvironmentVariable("SWIFT_VATSIM_CLIENT_ID");
        bool ok = true;
        id = clientIdAsString.toInt(&ok, 0); // base 0 means C convention
        if (!ok || id == 0) { return false; }
        key = qEnvironmentVariable("SWIFT_VATSIM_CLIENT_KEY").toLower();
        if (key.isEmpty()) { return false; }
        return true;
    }
#endif

} // namespace swift::core::context
