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
     * DBus config: http://dbus.freedesktop.org/doc/dbus-daemon.1.html
     */
    CDBusServer::CDBusServer(const QString &address, QObject *parent) :
        QObject(parent), m_busServer(CDBusServer::isQtDBusAddress(address) ? address : "tcp:host=127.0.0.1,port=45000", // "unix:tmpdir=/tmp",
                                     parent), m_serverMode(CDBusServer::SERVERMODE_P2P)
    {
        ServerMode m = CDBusServer::addressToDBusMode(address);
        switch (m)
        {
        case SERVERMODE_SESSIONBUS:
            {
                // we use a session bus connection instead of a real P2P connection
                this->m_serverMode = CDBusServer::SERVERMODE_SESSIONBUS;
                QDBusConnection con = QDBusConnection::sessionBus();
                if (!con.registerService(CDBusServer::ServiceName))
                {
                    qCritical() << con.lastError().message();
                    qFatal("Cannot register DBus service, server started? dbus-daemon.exe --session --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                // we use a system bus connection instead of a real P2P connection
                this->m_serverMode = CDBusServer::SERVERMODE_SYSTEMBUS;
                QDBusConnection con = QDBusConnection::systemBus();
                if (!con.registerService(CDBusServer::ServiceName))
                {
                    qCritical() << con.lastError().message();
                    qFatal("Cannot register DBus service, server started? dbus-daemon.exe --system --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_P2P:
        default:
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
                connect(&m_busServer, &QDBusServer::newConnection, this, &CDBusServer::registerObjectsWithConnection);
            }
            break;
        } // switch
    }

    /*
     * Check for P2P address
     */
    bool CDBusServer::isP2P(const QString &address)
    {
        return CDBusServer::addressToDBusMode(address) == SERVERMODE_P2P;
    }

    /*
     * Is Qt DBus address
     */
    bool CDBusServer::isQtDBusAddress(const QString &address)
    {
        return
            (address.startsWith("tcp:") ||
             address.startsWith("unix:")
            );
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
    bool CDBusServer::registerObjectsWithConnection(const QDBusConnection &connection)
    {
        Q_ASSERT(!this->m_objects.isEmpty());
        QDBusConnection newConnection(connection); // copy, because object will be registered on this connection
        // insert or replace connection
        m_DBusConnections.insert(newConnection.name(), newConnection);
        bool success = true;
        qDebug() << "New Connection from: " << newConnection.name();
        QMap<QString, QObject *>::ConstIterator i = m_objects.begin();
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
    }

    /*
     * Last error
     */
    QDBusError CDBusServer::lastError() const
    {
        return this->m_busServer.lastError();
    }

    /*
     * Unregister all objects
     */
    void CDBusServer::unregisterAllObjects()
    {
        if (this->m_objects.isEmpty()) return;
        foreach(QString path, this->m_objects.keys())
        {
            switch (this->m_serverMode)
            {
            case CDBusServer::SERVERMODE_SESSIONBUS:
                QDBusConnection::sessionBus().unregisterObject(path);
                break;
            case CDBusServer::SERVERMODE_SYSTEMBUS:
                QDBusConnection::systemBus().unregisterObject(path);
                break;
            case CDBusServer::SERVERMODE_P2P:
                {
                    foreach(QDBusConnection con, this->m_DBusConnections)
                    {
                        con.unregisterObject(path);
                    }
                    break;
                }
            }
        } // all paths
    }

    /*
     * p2pDBusServer
     */
    QString CDBusServer::p2pAddress(const QString &host, const QString &port)
    {
        QString h = host.isEmpty() ? "127.0.0.1" : host;
        QString p = port;
        if (port.isEmpty())
        {
            if (host.contains(":"))
            {
                QStringList parts = host.split(host);
                h = parts.at(0);
                p = parts.at(1);
            }
            else
            {
                p = "45000";
            }
        }
        QString p2p = QString("tcp:host=%1,port=%2").arg(h).arg(p);
        return p2p;
    }

    /*
     * Fix address
     */
    QString CDBusServer::fixAddressToDBusAddress(const QString &address)
    {
        if (address.isEmpty() || address == sessionDBusServer() || address == systemDBusServer()) return address;
        if (address.startsWith("tcp:") || address.startsWith("unix:")) return address;
        return p2pAddress(address);
    }

    /*
     * Convert address to mode
     */
    CDBusServer::ServerMode CDBusServer::addressToDBusMode(const QString &address)
    {
        QString a = address.toLower();
        if (a == CDBusServer::systemDBusServer())
            return SERVERMODE_SYSTEMBUS;
        else if (a == CDBusServer::sessionDBusServer())
            return SERVERMODE_SESSIONBUS;
        else
            return SERVERMODE_P2P;
    }

} // namespace BlackCore
