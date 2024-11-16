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
    IContextNetwork *IContextNetwork::create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local:
        case CCoreFacadeConfig::LocalInDBusServer: return (new CContextNetwork(mode, runtime))->registerWithDBus(server);
        case CCoreFacadeConfig::Remote: return new CContextNetworkProxy(CDBusServer::coreServiceName(connection), connection, mode, runtime);
        case CCoreFacadeConfig::NotUsed:
        default:
            return new CContextNetworkEmpty(runtime);
        }
    }

    const QList<QCommandLineOption> &IContextNetwork::getCmdLineOptions()
    {
        static const QList<QCommandLineOption> e;
        static const QList<QCommandLineOption> opts {
#ifdef SWIFT_VATSIM_SUPPORT
            QCommandLineOption({ "idAndKey", "clientIdAndKey" },
                               QCoreApplication::translate("CContextNetwork", "Client id and key pair separated by ':', e.g. <id>:<key>."), "clientIdAndKey")
#endif
        };

        // only in not officially shipped versions
        return (CBuildConfig::isLocalDeveloperDebugBuild()) ? opts : e;
    }

#ifdef SWIFT_VATSIM_SUPPORT
    bool IContextNetwork::getCmdLineClientIdAndKey(int &id, QString &key)
    {
        // init values
        id = 0;
        key = "";

        // split parser values
        if (IContextNetwork::getCmdLineOptions().isEmpty()) { return false; } // no such option, avoid warnings
        if (!sApp) { return false; }
        const QString clientIdAndKey = sApp->getParserValue("clientIdAndKey").toLower();
        if (clientIdAndKey.isEmpty() || !clientIdAndKey.contains(':')) { return false; }
        const QStringList stringList = clientIdAndKey.split(':');
        const QString clientIdAsString = stringList[0];
        bool ok = true;
        id = clientIdAsString.toInt(&ok, 0); // base 0 means C convention
        if (!ok || id == 0) { return false; }
        key = stringList[1];
        return true;
    }
#endif

} // namespace swift::core::context
