/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextapplicationempty.h"
#include "blackcore/context/contextapplicationimpl.h"
#include "blackcore/context/contextapplicationproxy.h"
#include "blackcore/corefacade.h"
#include "blackcore/inputmanager.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/statusmessage.h"

#include <QtDebug>
#include <algorithm>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;

namespace BlackCore::Context
{
    IContextApplication *IContextApplication::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local:
        case CCoreFacadeConfig::LocalInDBusServer:
            return (new CContextApplication(mode, parent))->registerWithDBus(server);
        case CCoreFacadeConfig::Remote:
            return new CContextApplicationProxy(CDBusServer::coreServiceName(connection), connection, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default:
            return new CContextApplicationEmpty(parent);
        }
    }

    IContextApplication::IContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime)
    {
        if (mode == CCoreFacadeConfig::NotUsed) { return; }
        QPointer<IContextApplication> myself(this);

        connect(CSettingsCache::instance(), &CSettingsCache::valuesChangedByLocal, this, [=](const CValueCachePacket &settings) {
            if (!myself) { return; }
            this->changeSettings(settings, {});
        });

        connect(this, &IContextApplication::settingsChanged, CSettingsCache::instance(), [](const CValueCachePacket &settings, const CIdentifier &origin) {
            // Intentionally don't check for round trip here
            CSettingsCache::instance()->changeValuesFromRemote(settings, origin);
        });

        Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
        bool s = connect(sApp->getInputManager(), &CInputManager::hotkeyActionRegistered, this, [=](const QStringList &actions) {
            if (!myself) { return; }
            this->registerHotkeyActions(actions, {});
        });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey action failed");
        Q_UNUSED(s);

        s = connect(this, &IContextApplication::hotkeyActionsRegistered, sApp->getInputManager(), [=](const QStringList &actions, const CIdentifier &origin) {
            if (origin.hasApplicationProcessId()) { return; }
            sApp->getInputManager()->registerRemoteActions(actions);
        });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey actions failed");
        Q_UNUSED(s);

        s = connect(
            sApp->getInputManager(), &CInputManager::remoteActionFromLocal, this, [=](const QString &action, bool argument) {
                if (!myself) { return; }
                this->callHotkeyActionRemotely(action, argument, {});
            },
            Qt::QueuedConnection);

        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect remote action failed");
        Q_UNUSED(s);

        // Enable event forwarding from GUI process to core
        sApp->getInputManager()->setForwarding(true);
    }

    void IContextApplication::changeSettings(const CValueCachePacket &settings, const CIdentifier &origin)
    {
        Q_UNUSED(settings);
        Q_UNUSED(origin);
        qFatal("Not implemented"); // avoid losing a change during context interface construction
    }

    void IContextApplication::registerHotkeyActions(const QStringList &actions, const CIdentifier &origin)
    {
        Q_UNUSED(actions);
        Q_UNUSED(origin);
        qFatal("Not implemented"); // avoid losing a change during context interface construction
    }

    void IContextApplication::callHotkeyActionRemotely(const QString &action, bool argument, const CIdentifier &origin)
    {
        Q_UNUSED(action);
        Q_UNUSED(argument);
        Q_UNUSED(origin);
    }

    bool IContextApplication::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        if (!this->getRuntime()) { return false; }
        return this->getRuntime()->parseCommandLine(commandLine, originator);
    }
} // ns
