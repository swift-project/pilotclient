/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplicationproxy.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/loghandler.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QObject>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackCore
{
    namespace Context
    {
        CContextApplicationProxy::CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextApplication(mode, runtime)
        {
            this->m_dBusInterface = new CGenericDBusInterface(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(), connection, this);
            this->relaySignals(serviceName, connection);

            connect(this, &IContextApplication::messageLogged, this, [](const CStatusMessage & message, const CIdentifier & origin)
            {
                if (!origin.isFromSameProcess())
                {
                    CLogHandler::instance()->logRemoteMessage(message);
                }
            });
        }

        void CContextApplicationProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
        {
            // signals originating from impl side
            bool s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                        "messageLogged", this, SIGNAL(messageLogged(BlackMisc::CStatusMessage, BlackMisc::CIdentifier)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "logSubscriptionAdded", this, SIGNAL(logSubscriptionAdded(BlackMisc::CIdentifier, BlackMisc::CLogPattern)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "logSubscriptionRemoved", this, SIGNAL(logSubscriptionRemoved(BlackMisc::CIdentifier, BlackMisc::CLogPattern)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "settingsChanged", this, SIGNAL(settingsChanged(BlackMisc::CValueCachePacket, BlackMisc::CIdentifier)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "registrationChanged", this, SIGNAL(registrationChanged()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "fakedSetComVoiceRoom", this, SIGNAL(fakedSetComVoiceRoom(BlackMisc::Audio::CVoiceRoomList)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "hotkeyActionsRegistered", this, SIGNAL(hotkeyActionsRegistered(QStringList, BlackMisc::CIdentifier)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                   "requestDisplayOnConsole", this, SIGNAL(requestDisplayOnConsole(QString)));
            Q_UNUSED(s);
            this->relayBaseClassSignals(serviceName, connection, IContextApplication::ObjectPath(), IContextApplication::InterfaceName());
        }

        void CContextApplicationProxy::logMessage(const CStatusMessage &message, const CIdentifier &origin)
        {
            if (subscribersOf(message).containsAnyNotIn(CIdentifierList({ origin, {} })))
            {
                this->m_dBusInterface->callDBus(QLatin1String("logMessage"), message, origin);
            }
        }

        void CContextApplicationProxy::addLogSubscription(const CIdentifier &subscriber, const CLogPattern &pattern)
        {
            this->m_dBusInterface->callDBus(QLatin1String("addLogSubscription"), subscriber, pattern);
        }

        void CContextApplicationProxy::removeLogSubscription(const CIdentifier &subscriber, const CLogPattern &pattern)
        {
            this->m_dBusInterface->callDBus(QLatin1String("removeLogSubscription"), subscriber, pattern);
        }

        CLogSubscriptionHash CContextApplicationProxy::getAllLogSubscriptions() const
        {
            return this->m_dBusInterface->callDBusRet<CLogSubscriptionHash>(QLatin1String("getAllLogSubscriptions"));
        }

        void CContextApplicationProxy::synchronizeLogSubscriptions()
        {
            // note this proxy method does not call synchronizeLogSubscriptions in core
            m_logSubscriptions = getAllLogSubscriptions();
            for (const auto &pattern : CLogHandler::instance()->getAllSubscriptions()) { this->addLogSubscription({}, pattern); }
        }

        void CContextApplicationProxy::changeSettings(const CValueCachePacket &settings, const CIdentifier &origin)
        {
            this->m_dBusInterface->callDBus(QLatin1String("changeSettings"), settings, origin);
        }

        BlackMisc::CValueCachePacket CContextApplicationProxy::getAllSettings() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CValueCachePacket>(QLatin1String("getAllSettings"));
        }

        QStringList CContextApplicationProxy::getUnsavedSettingsKeys() const
        {
            return this->m_dBusInterface->callDBusRet<QStringList>(QLatin1String("getUnsavedSettingsKeys"));
        }

        CSettingsDictionary CContextApplicationProxy::getUnsavedSettingsKeysDescribed() const
        {
            CSettingsDictionary result = this->m_dBusInterface->callDBusRet<CSettingsDictionary>(QLatin1String("getUnsavedSettingsKeysDescribed"));
            for (auto it = result.begin(); it != result.end(); ++it)
            {
                // consolidate with local names to fill any gaps in remote names
                if (it.value().isEmpty()) { it.value() = CSettingsCache::instance()->getHumanReadableName(it.key()); }
            }
            return result;
        }

        void CContextApplicationProxy::synchronizeLocalSettings()
        {
            // note this proxy method does not call synchronizeLocalSettings in core
            CSettingsCache::instance()->changeValuesFromRemote(this->getAllSettings(), CIdentifier::anonymous());
        }

        BlackMisc::CStatusMessage CContextApplicationProxy::saveSettings(const QString &keyPrefix)
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("saveSettings"), keyPrefix);
        }

        BlackMisc::CStatusMessage CContextApplicationProxy::saveSettingsByKey(const QStringList &keys)
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("saveSettingsByKey"), keys);
        }

        BlackMisc::CStatusMessage CContextApplicationProxy::loadSettings()
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("loadSettings"));
        }

        void CContextApplicationProxy::registerHotkeyActions(const QStringList &actions, const CIdentifier &origin)
        {
            this->m_dBusInterface->callDBus(QLatin1String("registerHotkeyActions"), actions, origin);
        }

        void CContextApplicationProxy::callHotkeyAction(const QString &action, bool argument, const CIdentifier &origin)
        {
            this->m_dBusInterface->callDBus(QLatin1String("callHotkeyAction"), action, argument, origin);
        }

        BlackMisc::CIdentifier CContextApplicationProxy::registerApplication(const CIdentifier &application)
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CIdentifier>(QLatin1String("registerApplication"), application);
        }

        void CContextApplicationProxy::unregisterApplication(const CIdentifier &application)
        {
            this->m_dBusInterface->callDBus(QLatin1String("unregisterApplication"), application);
        }

        BlackMisc::CIdentifierList CContextApplicationProxy::getRegisteredApplications() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CIdentifierList>(QLatin1String("getRegisteredApplications"));
        }

        bool CContextApplicationProxy::writeToFile(const QString &fileName, const QString &content)
        {
            if (fileName.isEmpty()) { return false; }
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("writeToFile"), fileName, content);
        }

        QString CContextApplicationProxy::readFromFile(const QString &fileName) const
        {
            if (fileName.isEmpty()) { return ""; }
            return this->m_dBusInterface->callDBusRet<QString>(QLatin1String("readFromFile"), fileName);
        }

        bool CContextApplicationProxy::removeFile(const QString &fileName)
        {
            if (fileName.isEmpty()) { return false; }
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("removeFile"), fileName);
        }

        bool CContextApplicationProxy::existsFile(const QString &fileName) const
        {
            if (fileName.isEmpty()) { return false; }
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("existsFile"), fileName);
        }

        QString CContextApplicationProxy::dotCommandsHtmlHelp() const
        {
            return this->m_dBusInterface->callDBusRet<QString>(QLatin1String("dotCommandsHtmlHelp"));
        }

        bool CContextApplicationProxy::isContextResponsive(const QString &dBusAddress, QString &msg, int timeoutMs)
        {
            const bool connected = CDBusServer::isDBusAvailable(dBusAddress, msg, timeoutMs);
            if (!connected) { return false; }

            static const QString dBusName("contexttest");
            QDBusConnection connection = CDBusServer::connectToDBus(dBusAddress, dBusName);
            CContextApplicationProxy proxy(BlackMisc::CDBusServer::coreServiceName(), connection, CCoreFacadeConfig::Remote, nullptr);
            const CIdentifier id("swift proxy test");
            const CIdentifier pingId = proxy.registerApplication(id);
            const bool ok = (id == pingId);
            if (ok)
            {
                proxy.unregisterApplication(id);
            }
            else
            {
                msg = "Mismatch in proxy ping, context not ready.";
            }
            CDBusServer::disconnectFromDBus(connection, dBusAddress);
            return ok;
        }
    } // namespace
} // namespace
