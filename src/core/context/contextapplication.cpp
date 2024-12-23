// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextapplication.h"

#include <algorithm>

#include <QtDebug>

#include "core/application.h"
#include "core/context/contextapplicationempty.h"
#include "core/context/contextapplicationimpl.h"
#include "core/context/contextapplicationproxy.h"
#include "core/corefacade.h"
#include "core/inputmanager.h"
#include "misc/dbus.h"
#include "misc/dbusserver.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/settingscache.h"
#include "misc/statusmessage.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc;

namespace swift::core::context
{
    IContextApplication *IContextApplication::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                                     CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local: return new CContextApplication(mode, parent);
        case CCoreFacadeConfig::LocalInDBusServer:
        {
            auto *context = new CContextApplication(mode, parent);
            context->registerWithDBus(server);
            return context;
        }
        case CCoreFacadeConfig::Remote:
            return new CContextApplicationProxy(CDBusServer::coreServiceName(connection), connection, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default: return new CContextApplicationEmpty(parent);
        }
    }

    IContextApplication::IContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
        : IContext(mode, runtime)
    {
        if (mode == CCoreFacadeConfig::NotUsed) { return; }
        QPointer<IContextApplication> myself(this);

        connect(CSettingsCache::instance(), &CSettingsCache::valuesChangedByLocal, this,
                [=](const CValueCachePacket &settings) {
                    if (!myself) { return; }
                    this->changeSettings(settings, {});
                });

        connect(this, &IContextApplication::settingsChanged, CSettingsCache::instance(),
                [](const CValueCachePacket &settings, const CIdentifier &origin) {
                    // Intentionally don't check for round trip here
                    CSettingsCache::instance()->changeValuesFromRemote(settings, origin);
                });

        Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
        bool s = connect(sApp->getInputManager(), &CInputManager::hotkeyActionRegistered, this,
                         [=](const QStringList &actions) {
                             if (!myself) { return; }
                             this->registerHotkeyActions(actions, {});
                         });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey action failed");
        Q_UNUSED(s);

        s = connect(this, &IContextApplication::hotkeyActionsRegistered, sApp->getInputManager(),
                    [=](const QStringList &actions, const CIdentifier &origin) {
                        if (origin.hasApplicationProcessId()) { return; }
                        sApp->getInputManager()->registerRemoteActions(actions);
                    });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey actions failed");
        Q_UNUSED(s);

        s = connect(
            sApp->getInputManager(), &CInputManager::remoteActionFromLocal, this,
            [=](const QString &action, bool argument) {
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
} // namespace swift::core::context
