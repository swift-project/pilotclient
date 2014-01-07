/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_application_interface.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    IContextApplication::IContextApplication(const QString &serviceName, QDBusConnection &connection, QObject *parent) : QObject(parent), m_dBusInterface(0)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(serviceName , IContextApplication::ServicePath(), IContextApplication::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals
     */
    void IContextApplication::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        connection.connect(serviceName, IContextApplication::ServicePath(), IContextApplication::InterfaceName(),
                           "statusMessage", this, SIGNAL(statusMessage(BlackMisc::CStatusMessage)));
        connection.connect(serviceName, IContextApplication::ServicePath(), IContextApplication::InterfaceName(),
                           "widgetGuiStarting", this, SIGNAL(widgetGuiStarting()));
        connection.connect(serviceName, IContextApplication::ServicePath(), IContextApplication::InterfaceName(),
                           "widgetGuiTerminating", this, SIGNAL(widgetGuiTerminating()));
    }

    /*
     * Ping, is DBus alive?
     */
    qint64 IContextApplication::ping(qint64 token) const
    {
        qint64 t = this->m_dBusInterface->callDBusRet<qint64>(QLatin1Literal("ping"), token);
        return t;
    }

} // namespace
