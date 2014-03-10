/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QDebug>
#include <QMetaClassInfo>

#include "dbus_server.h"

namespace BlackCore
{

    const QString CDBusServer::ServiceName = QString(BLACKCORE_DBUSERVER_SERVICENAME);

    /*
     * Constructor
     * Remark, without the default "unix:tmpdir=/tmp" any refereal to address crashes
     * see http://download.froglogic.com/public/qt5-squishcoco-report/QtBase/source_241_preprocessed.html
     */
    CDBusServer::CDBusServer(const QString &address, QObject *parent) :
        QObject(parent), m_busServer(CDBusServer::isP2P(address) ? address : "unix:tmpdir=/tmp" , parent), m_serverMode(CDBusServer::SERVERMODE_P2P)
    {
        if (address.isEmpty() || address.toLower() == "session")
        {
            // we use a session bus connection instead of a real P2P connection
            this->m_serverMode = CDBusServer::SERVERMODE_SESSIONBUS;
            QDBusConnection con = QDBusConnection::sessionBus();

            if (!con.registerService(CDBusServer::ServiceName))
            {
                qFatal("Cannot register DBus service");
            }
            this->newConnection(con);
        }
        else if (address.toLower() == "system")
        {
            // we use a system bus connection instead of a real P2P connection
            this->m_serverMode = CDBusServer::SERVERMODE_SYSTEMBUS;
            QDBusConnection con = QDBusConnection::systemBus();
            if (!con.registerService(CDBusServer::ServiceName))
            {
                qFatal("Cannot register DBus service");
            }
            this->newConnection(con);
        }
        else
        {
            this->m_serverMode = CDBusServer::SERVERMODE_P2P;
            // Note: P2P has no service name
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
    }

    /*
     * Check for P2P address
     */
    bool CDBusServer::isP2P(const QString &address)
    {
        return !(address.isEmpty() || address.toLower() == "session" || address.toLower() == "system");
    }

    /*
     * Class info
     */
    const QString CDBusServer::getClassInfo(QObject *object)
    {
        if (!object) return "";
        const QMetaObject *mo = object->metaObject();
        if (mo->classInfoCount() < 1) return "";
        for (int i = 0; i < mo->classInfoCount(); i++)
        {
            QMetaClassInfo ci = mo->classInfo(i);
            QString name(ci.name());
            if (name == "D-Bus Interface") return QString(ci.value());
        }
        return "";
    }

    /*
     * Connection established
     */
    bool CDBusServer::newConnection(const QDBusConnection &connection)
    {
        QMap<QString, QObject *>::ConstIterator i = m_objects.begin();
        QDBusConnection newConnection(connection);
        m_DBusConnections.insert(newConnection.name(), newConnection);
        bool success = true;
        qDebug() << "New Connection from: " << newConnection.name();

        while (i != m_objects.end())
        {
            qDebug() << "Adding " << i.key() << CDBusServer::getClassInfo(i.value()) << "to the new connection.";
            bool ok = newConnection.registerObject(i.key(), i.value(), CDBusServer::RegisterOptions());
            Q_ASSERT_X(ok, "CDBusServer::newConnection", "Registration failed");
            if (!ok) success = false;
            ++i;
        }

        return success;
    }

    /*
     * Add the objects
     */
    void CDBusServer::addObject(const QString &path, QObject *object)
    {
        if (!object) return;
        m_objects.insert(path, object); // this will be added when connection is established

        if (this->m_serverMode == CDBusServer::SERVERMODE_P2P) return;

        bool success = false;
        if (this->m_serverMode == CDBusServer::SERVERMODE_SESSIONBUS)
        {
            success = QDBusConnection::sessionBus().registerObject(path, object, CDBusServer::RegisterOptions());
            qDebug() << "Adding " << path << CDBusServer::getClassInfo(object) << "to the session bus.";
            if (!success) qDebug() << "Error, no success with registration" << this->lastError().message();
        }
        else if (this->m_serverMode == CDBusServer::SERVERMODE_SYSTEMBUS)
        {
            success = QDBusConnection::systemBus().registerObject(path, object, CDBusServer::RegisterOptions());
            qDebug() << "Adding " << path << CDBusServer::getClassInfo(object) << "to the system bus.";
            if (!success) qDebug() << "Error, no success with registration" << this->lastError().message();
        }
        else
        {
            Q_ASSERT_X(false, "CDBusServer::addObject", "Wrong server mode");
        }
        // Q_ASSERT_X(success, "CDBusServer::addObject", "Registration failed");
    }

    /*
     * Last error
     */
    QDBusError CDBusServer::lastError() const
    {
        return this->m_busServer.lastError();
    }

} // namespace BlackCore

