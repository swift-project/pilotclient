/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_settings_proxy.h"

#include "blackmisc/blackmiscfreefunctions.h"

#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Network;
using namespace BlackMisc::Hardware;

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    CContextSettingsProxy::CContextSettingsProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSettings(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextSettings::ObjectPath(), IContextSettings::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals
     */
    void CContextSettingsProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextSettings::ObjectPath(), IContextSettings::InterfaceName(),
                                    "changedSettings", this, SIGNAL(changedSettings(uint)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    /*
     * Relay to DBus
     */
    CSettingsNetwork CContextSettingsProxy::getNetworkSettings() const
    {
        return this->m_dBusInterface->callDBusRet<CSettingsNetwork>(QLatin1Literal("getNetworkSettings"));
    }

    /*
     * Relay tp DBus
     */
    CKeyboardKeyList CContextSettingsProxy::getHotkeys() const
    {
        return this->m_dBusInterface->callDBusRet<CKeyboardKeyList>(QLatin1Literal("getHotkeys"));
    }

    /*
     * Relay to DBus, but make this no slot
     */
    BlackMisc::CStatusMessageList CContextSettingsProxy::value(const QString &path, const QString &command, const BlackMisc::CVariant &value)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("value"), path, command, value);
    }

    /*
     * Write settings
     */
    BlackMisc::CStatusMessage CContextSettingsProxy::write() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("write"));
    }

    /*
     * Read settings
     */
    CStatusMessage CContextSettingsProxy::read()
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("read"));
    }

    /*
     * Reset settings
     */
    CStatusMessage CContextSettingsProxy::reset(bool write)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("reset"), write);
    }

    /*
     * File name
     */
    QString CContextSettingsProxy::getSettingsFileName() const
    {
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("getSettingsFileName"));
    }

    /*
     * As JSON string
     */
    QString CContextSettingsProxy::getSettingsAsJsonString() const
    {
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("getSettingsAsJsonString"));
    }

} // namespace
