/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextapplicationempty.h"
#include "blackcore/context/contextapplicationimpl.h"
#include "blackcore/context/contextapplicationproxy.h"
#include "blackcore/corefacade.h"
#include "blackcore/inputmanager.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/statusmessage.h"

#include <QtDebug>
#include <algorithm>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;

namespace BlackCore
{
    namespace Context
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

        IContextApplication::IContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            CContext(mode, runtime)
        {
            if (mode == CCoreFacadeConfig::NotUsed) { return; }
            QPointer<IContextApplication> myself(this);

            connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this, [ = ](const CStatusMessage & message)
            {
                if (!myself) { return; }
                this->logMessage(message, {});
            });
            connect(CLogHandler::instance(), &CLogHandler::subscriptionAdded, this, [ = ](const CLogPattern & pattern)
            {
                if (!myself) { return; }
                this->addLogSubscription({}, pattern);
            });
            connect(CLogHandler::instance(), &CLogHandler::subscriptionRemoved, this, [ = ](const CLogPattern & pattern)
            {
                if (!myself) { return; }
                this->removeLogSubscription({}, pattern);
            });
            connect(this, &IContextApplication::logSubscriptionAdded, this, [ = ](const CIdentifier & subscriber, const CLogPattern & pattern)
            {
                if (!myself) { return; }
                m_logSubscriptions[subscriber].push_back(pattern);
            });
            connect(this, &IContextApplication::logSubscriptionRemoved, this, [ = ](const CIdentifier & subscriber, const CLogPattern & pattern)
            {
                if (!myself) { return; }
                m_logSubscriptions[subscriber].removeAll(pattern);
            });

            connect(CSettingsCache::instance(), &CSettingsCache::valuesChangedByLocal, [ = ](const CValueCachePacket & settings)
            {
                if (!myself) { return; }
                this->changeSettings(settings, {});
            });

            connect(this, &IContextApplication::settingsChanged, [](const CValueCachePacket & settings, const CIdentifier & origin)
            {
                // Intentionally don't check for round trip here
                CSettingsCache::instance()->changeValuesFromRemote(settings, origin);
            });

            bool s = connect(CInputManager::instance(), &CInputManager::hotkeyActionRegistered, [ = ](const QStringList & actions)
            {
                if (!myself) { return; }
                this->registerHotkeyActions(actions, {});
            });
            Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey action failed");
            Q_UNUSED(s);

            s = connect(this, &IContextApplication::hotkeyActionsRegistered, [ = ](const QStringList & actions, const CIdentifier & origin)
            {
                if (origin.hasApplicationProcessId()) { return; }
                CInputManager::instance()->registerRemoteActions(actions);
            });
            Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey actions failed");
            Q_UNUSED(s);

            s = connect(CInputManager::instance(), &CInputManager::remoteActionFromLocal, this, [ = ](const QString & action, bool argument)
            {
                if (!myself) { return; }
                this->callHotkeyAction(action, argument, {});
            },
            Qt::QueuedConnection);

            Q_ASSERT_X(s, Q_FUNC_INFO, "Connect remote action failed");
            Q_UNUSED(s);

            s = connect(this, &IContextApplication::remoteHotkeyAction, [ = ](const QString & action, bool argument, const CIdentifier & origin)
            {
                if (!myself) { return; }
                if (origin.isFromLocalMachine()) { return; }
                CInputManager::instance()->callFunctionsBy(action, argument);
                CLogMessage(this, CLogCategory::contextSlot()).debug() << "Calling function" << action << "from origin" << origin.getMachineName();
            });
            Q_ASSERT_X(s, Q_FUNC_INFO, "Connect remote hotkey action failed");
            Q_UNUSED(s);

            // Enable event forwarding from GUI process to core
            CInputManager::instance()->setForwarding(true);
        }

        CIdentifierList IContextApplication::subscribersOf(const CStatusMessage &message) const
        {
            CIdentifierList result;
            for (auto it = m_logSubscriptions.begin(); it != m_logSubscriptions.end(); ++it)
            {
                bool match = std::any_of(it->begin(), it->end(), [&message](const CLogPattern & pattern) { return pattern.match(message); });
                if (match) { result.push_back(it.key()); }
            }
            return result;
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

        void IContextApplication::callHotkeyAction(const QString &action, bool argument, const CIdentifier &origin)
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
} // ns

QDBusArgument &operator <<(QDBusArgument &arg, const BlackCore::Context::CLogSubscriptionHash &hash)
{
    QList<CLogSubscriptionPair> listOfPairs;
    for (auto it = hash.begin(); it != hash.end(); ++it) { listOfPairs.push_back({ it.key(), it.value() }); }
    return arg << listOfPairs;
}

const QDBusArgument &operator >>(const QDBusArgument &arg, BlackCore::Context::CLogSubscriptionHash &hash)
{
    QList<CLogSubscriptionPair> listOfPairs;
    arg >> listOfPairs;
    for (const auto &pair : as_const(listOfPairs)) { hash.insert(pair.first, pair.second); }
    return arg;
}
