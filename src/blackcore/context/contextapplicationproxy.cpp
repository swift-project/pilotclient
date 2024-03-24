// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/application.h"
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

namespace BlackCore::Context
{
    CContextApplicationProxy::CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextApplication(mode, runtime)
    {
        m_dBusInterface = new CGenericDBusInterface(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);

        connect(this, &CContextApplicationProxy::remoteHotkeyAction, this, &CContextApplicationProxy::processRemoteHotkeyActionCall);

        m_pingTimer.setObjectName(serviceName + "::m_pingTimer");
        connect(&m_pingTimer, &QTimer::timeout, this, &CContextApplicationProxy::reRegisterApplications);
        m_pingTimer.start(PingIdentifiersMs);
    }

    void CContextApplicationProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        // signals originating from impl side
        bool s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                    "settingsChanged", this, SIGNAL(settingsChanged(BlackMisc::CValueCachePacket, BlackMisc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "registrationChanged", this, SIGNAL(registrationChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "hotkeyActionsRegistered", this, SIGNAL(hotkeyActionsRegistered(QStringList, BlackMisc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "remoteHotkeyAction", this, SIGNAL(remoteHotkeyAction(QString, bool, BlackMisc::CIdentifier)));
        Q_ASSERT(s);
        Q_UNUSED(s);
        this->relayBaseClassSignals(serviceName, connection, IContextApplication::ObjectPath(), IContextApplication::InterfaceName());
    }

    void CContextApplicationProxy::changeSettings(const CValueCachePacket &settings, const CIdentifier &origin)
    {
        m_dBusInterface->callDBus(QLatin1String("changeSettings"), settings, origin);
    }

    BlackMisc::CValueCachePacket CContextApplicationProxy::getAllSettings() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::CValueCachePacket>(QLatin1String("getAllSettings"));
    }

    QStringList CContextApplicationProxy::getUnsavedSettingsKeys() const
    {
        return m_dBusInterface->callDBusRet<QStringList>(QLatin1String("getUnsavedSettingsKeys"));
    }

    CSettingsDictionary CContextApplicationProxy::getUnsavedSettingsKeysDescribed() const
    {
        CSettingsDictionary result = m_dBusInterface->callDBusRet<CSettingsDictionary>(QLatin1String("getUnsavedSettingsKeysDescribed"));
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
        CSettingsCache::instance()->changeValuesFromRemote(this->getAllSettings(), CIdentifier::null());
    }

    BlackMisc::CStatusMessage CContextApplicationProxy::saveSettings(const QString &keyPrefix)
    {
        return m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("saveSettings"), keyPrefix);
    }

    BlackMisc::CStatusMessage CContextApplicationProxy::saveSettingsByKey(const QStringList &keys)
    {
        return m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("saveSettingsByKey"), keys);
    }

    BlackMisc::CStatusMessage CContextApplicationProxy::loadSettings()
    {
        return m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("loadSettings"));
    }

    void CContextApplicationProxy::registerHotkeyActions(const QStringList &actions, const CIdentifier &origin)
    {
        m_dBusInterface->callDBus(QLatin1String("registerHotkeyActions"), actions, origin);
    }

    void CContextApplicationProxy::callHotkeyActionRemotely(const QString &action, bool argument, const CIdentifier &origin)
    {
        m_dBusInterface->callDBus(QLatin1String("callHotkeyActionRemotely"), action, argument, origin);
    }

    CIdentifier CContextApplicationProxy::registerApplication(const CIdentifier &application)
    {
        m_proxyPingIdentifiers.insert(application);
        if (m_pingTimer.isActive()) { m_pingTimer.start(); } // restart, no need to ping again
        return m_dBusInterface->callDBusRet<BlackMisc::CIdentifier>(QLatin1String("registerApplication"), application);
    }

    void CContextApplicationProxy::unregisterApplication(const CIdentifier &application)
    {
        m_proxyPingIdentifiers.remove(application);
        m_dBusInterface->callDBus(QLatin1String("unregisterApplication"), application);
    }

    BlackMisc::CIdentifierList CContextApplicationProxy::getRegisteredApplications() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::CIdentifierList>(QLatin1String("getRegisteredApplications"));
    }

    CIdentifier CContextApplicationProxy::getApplicationIdentifier() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::CIdentifier>(QLatin1String("getApplicationIdentifier"));
    }

    QString CContextApplicationProxy::dotCommandsHtmlHelp() const
    {
        return m_dBusInterface->callDBusRet<QString>(QLatin1String("dotCommandsHtmlHelp"));
    }

    void CContextApplicationProxy::reRegisterApplications()
    {
        if (!m_dBusInterface) { return; }
        if (m_proxyPingIdentifiers.isEmpty()) { return; }
        const CIdentifierSet identifiers = m_proxyPingIdentifiers; // copy so member can be modified
        for (const CIdentifier &identifier : identifiers)
        {
            this->registerApplication(identifier);
        }
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

    void CContextApplicationProxy::processRemoteHotkeyActionCall(const QString &action, bool argument, const CIdentifier &origin)
    {
        if (!sApp || origin.isFromLocalMachine()) { return; }
        sApp->getInputManager()->callFunctionsBy(action, argument);
        CLogMessage(this, CLogCategories::contextSlot()).debug() << "Calling function" << action << "from origin" << origin.getMachineName();
    }
} // namespace
