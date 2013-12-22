/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QDebug>

#include "dbus_server.h"

namespace BlackCore
{

    CDBusServer::CDBusServer(const QString &address, QObject *parent) :
        QObject(parent), m_busServer(address, parent)
    {
        if (!m_busServer.isConnected())
        {
            qWarning() << m_busServer.lastError().message();
        }
        else
        {
            qDebug() << "Server listening on address: " << m_busServer.address();
        }

        connect(&m_busServer, &QDBusServer::newConnection, this, &CDBusServer::newConnection);
    }

    void CDBusServer::newConnection(const QDBusConnection & connection)
    {
        QMap<QString, QObject*>::ConstIterator i = m_objects.begin();
        QDBusConnection newConnection(connection);

        m_DBusConnections.insert(newConnection.name(), newConnection);

        qDebug() << "New Connection from: " << newConnection.name();

        while (i != m_objects.end())
        {
            qDebug() << "Adding " << i.key() << "to the new connection.";
            newConnection.registerObject(i.key(), i.value());
            ++i;
        }
    }

    void CDBusServer::addObject(const QString &name, QObject *object)
    {
        m_objects.insert(name, object);
    }

    void CDBusServer::printError()
    {
        qWarning() << m_busServer.lastError().name();
    }

} // namespace BlackCore

