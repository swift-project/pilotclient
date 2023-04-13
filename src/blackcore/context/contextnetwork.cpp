/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextnetworkempty.h"
#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextnetworkproxy.h"
#include "blackcore/application.h"
#include "blackmisc/dbusserver.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackCore;
using namespace BlackMisc;

namespace BlackCore::Context
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
            QCommandLineOption({ "idAndKey", "clientIdAndKey" },
                               QCoreApplication::translate("CContextNetwork", "Client id and key pair separated by ':', e.g. <id>:<key>."), "clientIdAndKey")
        };

        // only in not officially shipped versions
        return (CBuildConfig::isLocalDeveloperDebugBuild()) ? opts : e;
    }

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

} // namesapce
