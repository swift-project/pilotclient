/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_settings_interface.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    IContextSettings::IContextSettings(const QString &serviceName, QDBusConnection &connection, QObject *parent) : QObject(parent), m_dBusInterface(0)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextSettings::ServicePath(), IContextSettings::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals
     */
    void IContextSettings::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        connection.connect(serviceName, IContextSettings::ServicePath(), IContextSettings::InterfaceName(),
                           "changedNetworkSettings", this, SIGNAL(changedNetworkSettings()));
    }

    /*
     * Relay to DBus
     */
    BlackMisc::Settings::CSettingsNetwork IContextSettings::getNetworkSettings() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Settings::CSettingsNetwork>(QLatin1Literal("getNetworkSettings"));
    }

    /*
     * Relay tp DBus
     */
    BlackMisc::Hardware::CKeyboardKeyList IContextSettings::getHotkeys() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Hardware::CKeyboardKeyList>(QLatin1Literal("getHotkeys"));
    }

    /*
     * Relay to DBus, but make this no slot
     */
    BlackMisc::CStatusMessageList IContextSettings::value(const QString &path, const QString &command, const QVariant &value)
    {
        int type = value.userType() - BlackMisc::firstBlackMetaType();
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("value"), path, command, QDBusVariant(value), type);
    }

    /*
     * DBus version of value
     */
    BlackMisc::CStatusMessageList IContextSettings::value(const QString &path, const QString &command, QDBusVariant value, int unifiedBlackMetaType)
    {
        QVariant qv = value.variant();
        if (qv.canConvert<QDBusArgument>())
        {
            // convert from QDBusArgument
            int type = BlackMisc::firstBlackMetaType() + unifiedBlackMetaType; // unify
            qv = BlackMisc::fixQVariantFromDbusArgument(qv, type);
        }
        // when called locally, this will call the virtual method
        // of the concrete implementation in context_settings
        return this->value(path, command, qv);
    }

} // namespace
