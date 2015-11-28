/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextapplication.h"
#include "blackcore/contextapplicationimpl.h"
#include "blackcore/contextapplicationproxy.h"
#include "blackcore/contextapplicationempty.h"
#include "blackcore/inputmanager.h"
#include "blackcore/settingscache.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/logmessage.h"
#include <QCoreApplication>
#include <QThread>

using namespace BlackCore;
using namespace BlackMisc;

namespace BlackCore
{

    IContextApplication *IContextApplication::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextApplication(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextApplicationProxy(BlackMisc::CDBusServer::ServiceName(), connection, mode, parent);
        case CRuntimeConfig::NotUsed:
        default:
            return new CContextApplicationEmpty(parent);
        }
    }

    IContextApplication::IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        CContext(mode, runtime)
    {
        if (mode == CRuntimeConfig::NotUsed) { return; }
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this, [this](const CStatusMessage & message)
        {
            this->logMessage(message, {});
        });
        connect(CLogHandler::instance(), &CLogHandler::subscriptionAdded, this, [this](const CLogPattern & pattern)
        {
            this->addLogSubscription({}, pattern);
        });
        connect(CLogHandler::instance(), &CLogHandler::subscriptionRemoved, this, [this](const CLogPattern & pattern)
        {
            this->removeLogSubscription({}, pattern);
        });
        connect(this, &IContextApplication::logSubscriptionAdded, this, [this](const CIdentifier & subscriber, const CLogPattern & pattern)
        {
            this->m_logSubscriptions[subscriber].push_back(pattern);
        });
        connect(this, &IContextApplication::logSubscriptionRemoved, this, [this](const CIdentifier & subscriber, const CLogPattern & pattern)
        {
            this->m_logSubscriptions[subscriber].removeAll(pattern);
        });

        connect(CSettingsCache::instance(), &CSettingsCache::valuesChangedByLocal, [this](const CValueCachePacket & settings)
        {
            this->changeSettings(settings, {});
        });

        connect(this, &IContextApplication::settingsChanged, [](const CValueCachePacket & settings, const CIdentifier & origin)
        {
            // Intentionally don't check for round trip here
            CSettingsCache::instance()->changeValuesFromRemote(settings, origin);
        });

        bool s = connect(CInputManager::instance(), &CInputManager::hotkeyActionRegistered, [this](const QStringList & actions)
        {
            this->registerHotkeyActions(actions, {});
        });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey action failed");
        Q_UNUSED(s);

        s = connect(this, &IContextApplication::hotkeyActionsRegistered, [this](const QStringList & actions, const CIdentifier & origin)
        {
            if (origin.isFromSameProcess()) { return; }
            CInputManager::instance()->registerRemoteActions(actions);
        });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect hotkey actions failed");
        Q_UNUSED(s);

        s = connect(CInputManager::instance(), &CInputManager::remoteActionFromLocal, [this](const QString & action, bool argument)
        {
            this->callHotkeyAction(action, argument, {});
        });
        Q_ASSERT_X(s, Q_FUNC_INFO, "Connect remote action failed");
        Q_UNUSED(s);

        s = connect(this, &IContextApplication::remoteHotkeyAction, [this](const QString & action, bool argument, const CIdentifier & origin)
        {
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

    void IContextApplication::registerHotkeyActions(const QStringList &actions, const BlackMisc::CIdentifier &origin)
    {
        Q_UNUSED(actions);
        Q_UNUSED(origin);
        qFatal("Not implemented"); // avoid losing a change during context interface construction
    }

    void IContextApplication::callHotkeyAction(const QString &action, bool argument, const BlackMisc::CIdentifier &origin)
    {
        Q_UNUSED(action);
        Q_UNUSED(argument);
        Q_UNUSED(origin);
    }

    void IContextApplication::changeSettings(uint typeValue)
    {
        Q_UNUSED(typeValue);
    }

} // namespace

QDBusArgument &operator <<(QDBusArgument &arg, const BlackCore::CLogSubscriptionHash &hash)
{
    QList<CLogSubscriptionPair> listOfPairs;
    for (auto it = hash.begin(); it != hash.end(); ++it) { listOfPairs.push_back({ it.key(), it.value() }); }
    return arg << listOfPairs;
}

const QDBusArgument &operator >>(const QDBusArgument &arg, BlackCore::CLogSubscriptionHash &hash)
{
    QList<CLogSubscriptionPair> listOfPairs;
    arg >> listOfPairs;
    for (const auto &pair : listOfPairs) { hash.insert(pair.first, pair.second); }
    return arg;
}
