/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include "dbusserver.h"
#include <QProcess>
#include <QMetaClassInfo>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    /*
     * Constructor
     * Remark, without the default "unix:tmpdir=/tmp" any refereal to address crashes
     * see http://download.froglogic.com/public/qt5-squishcoco-report/QtBase/source_241_preprocessed.html
     * DBus config: http://dbus.freedesktop.org/doc/dbus-daemon.1.html
     */
    CDBusServer::CDBusServer(const QString &service, const QString &address, QObject *parent) : QObject(parent)
    {
        ServerMode m = CDBusServer::addressToDBusMode(address);
        switch (m)
        {
        case SERVERMODE_SESSIONBUS:
            {
                // we use a session bus connection instead of a real P2P connection
                this->m_serverMode = CDBusServer::SERVERMODE_SESSIONBUS;
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, ServiceName());
                if (!connection.isConnected())
                {
                    launchDbusDaemon();
                    connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, ServiceName());
                }

                if (!connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning("DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning("Cannot register DBus service, server started? dbus-daemon.exe --session --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                // we use a system bus connection instead of a real P2P connection
                this->m_serverMode = CDBusServer::SERVERMODE_SYSTEMBUS;
                QDBusConnection connection = QDBusConnection::systemBus();
                if (!connection.isConnected())
                {
                    launchDbusDaemon();
                    connection = QDBusConnection::systemBus();
                }

                if (!connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning("DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning("Cannot register DBus service, server started? dbus-daemon.exe --session --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_P2P:
        default:
            {
                QString dbusAddress(CDBusServer::isQtDBusAddress(address) ? address : "tcp:host=127.0.0.1,port=45000");
                dbusAddress = dbusAddress.toLower().trimmed().replace(' ', "");
                if (!dbusAddress.contains("bind=")) { dbusAddress = dbusAddress.append(",bind=*"); }
                this->m_serverMode = CDBusServer::SERVERMODE_P2P;
                this->m_busServer.reset(
                    new QDBusServer(
                        dbusAddress, // "unix:tmpdir=/tmp"
                        parent)
                );
                m_busServer->setAnonymousAuthenticationAllowed(true);

                // Note: P2P has no service name
                if (!m_busServer->isConnected())
                {
                    CLogMessage(this).warning("DBus P2P connection failed: %1") << this->lastQDBusServerError().message();
                }
                else
                {
                    CLogMessage(this).debug() << "Server listening on address: " << m_busServer->address();
                }
                connect(m_busServer.data(), &QDBusServer::newConnection, this, &CDBusServer::ps_registerObjectsWithP2PConnection);
            }
            break;
        } // switch
    }

    const QString &CDBusServer::ServiceName()
    {
        static const QString sn(BLACKCORE_RUNTIME_SERVICENAME);
        return sn;
    }

    void CDBusServer::launchDbusDaemon()
    {
        const QString program = QStringLiteral("dbus-daemon");
        const QStringList arguments = { QStringLiteral("--config-file=../share/dbus-1/session.conf") };
        bool success = QProcess::startDetached(program, arguments);
        if (!success) { CLogMessage(this).warning("Failed to launch dbus-daemon!"); }
    }

    bool CDBusServer::isP2PAddress(const QString &address)
    {
        return CDBusServer::addressToDBusMode(address) == SERVERMODE_P2P;
    }

    bool CDBusServer::splitDBusAddressIntoHostAndPort(const QString &dbusAddress, QString &host, int &port)
    {
        bool ok = false;
        QString dbus(dbusAddress.trimmed().toLower().replace(' ', ""));
        if (dbus.contains("host=") || dbus.contains("port="))
        {
            // "tcp:host=foo.com,port=123"
            QStringList parts(dbus.split(','));
            for (const QString &p : parts)
            {
                if (p.contains("host="))
                {
                    host = p.mid(p.lastIndexOf("=") + 1).trimmed();
                }
                else if (p.contains("port="))
                {
                    bool ok;
                    port = p.mid(p.lastIndexOf("=") + 1).trimmed().toInt(&ok);
                    if (!ok) { port = -1; }
                }
            }
            if (port < 0) { port = 45000; }
            if (host.isEmpty()) { host = "127.0.0.1"; }
            ok = true;
        }

        if (!ok)
        {
            host = "";
            port = -1;
        }
        return ok;
    }

    bool CDBusServer::isQtDBusAddress(const QString &address)
    {
        return
            (address.contains("tcp:") ||
             address.contains("unix:")
            );
    }

    bool CDBusServer::isSessionOrSystemAddress(const QString &address)
    {
        return address == sessionDBusServer() || address == systemDBusServer();
    }

    const QString CDBusServer::getClassInfo(QObject *object)
    {
        if (!object) return "";
        const QMetaObject *mo = object->metaObject();
        if (mo->classInfoCount() < 1) return "";
        for (int i = 0; i < mo->classInfoCount(); i++)
        {
            QMetaClassInfo ci = mo->classInfo(i);
            QString name(ci.name());
            if (name == "D-Bus Interface") { return QString(ci.value()); }
        }
        return "";
    }

    bool CDBusServer::ps_registerObjectsWithP2PConnection(const QDBusConnection &connection)
    {
        Q_ASSERT(!this->m_objects.isEmpty());
        QDBusConnection newConnection(connection); // copy, because object will be registered on this connection
        // insert or replace connection
        m_DBusConnections.insert(newConnection.name(), newConnection);
        bool success = true;
        CLogMessage(this).debug() << "New Connection from: " << newConnection.name();
        QMap<QString, QObject *>::ConstIterator i = m_objects.begin();
        while (i != m_objects.end())
        {
            CLogMessage(this).debug() << "Adding " << i.key() << CDBusServer::getClassInfo(i.value()) << "to the new connection.";
            bool ok = newConnection.registerObject(i.key(), i.value(), CDBusServer::RegisterOptions());
            Q_ASSERT_X(ok, "CDBusServer::newConnection", "Registration failed");
            if (!ok) { success = false; }
            ++i;
        }
        return success;
    }

    void CDBusServer::addObject(const QString &path, QObject *object)
    {
        if (!object) { return; }
        m_objects.insert(path, object); // For P2P: registered when P2P connection is established

        // P2P
        if (this->m_serverMode == CDBusServer::SERVERMODE_P2P)
        {
            return;
        }

        bool success = false;
        if (this->m_serverMode == CDBusServer::SERVERMODE_SESSIONBUS)
        {
            QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, ServiceName());
            success = connection.registerObject(path, object, CDBusServer::RegisterOptions());
            if (success)
            {
                CLogMessage(this).debug() << "Adding " << path << CDBusServer::getClassInfo(object) << " to session bus.";
            }
            else
            {
                CLogMessage(this).error("Error, no success with session bus registration");
            }
        }
        else if (this->m_serverMode == CDBusServer::SERVERMODE_SYSTEMBUS)
        {
            QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, ServiceName());
            success = connection.registerObject(path, object, CDBusServer::RegisterOptions());
            if (success)
            {
                CLogMessage(this).debug() << "Adding " << path << CDBusServer::getClassInfo(object) << " to system bus.";
            }
            else
            {
                CLogMessage(this).error("Error, no success with system bus registration");
            }
        }
        else
        {
            Q_ASSERT_X(false, "CDBusServer::addObject", "Wrong server mode");
        }
    }

    QDBusError CDBusServer::lastQDBusServerError() const
    {
        if (!hasQDBusServer()) { return QDBusError(); }
        return this->m_busServer->lastError();
    }

    const QDBusServer *CDBusServer::qDBusServer() const
    {
        return this->m_busServer.data();
    }

    bool CDBusServer::hasQDBusServer() const
    {
        return !this->m_busServer.isNull();
    }

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
                    foreach(QDBusConnection connection, this->m_DBusConnections)
                    {
                        connection.unregisterObject(path);
                    }
                    break;
                }
            }
        } // all paths
    }

    const QDBusConnection &CDBusServer::defaultConnection()
    {
        static QDBusConnection defaultConnection("default");
        return defaultConnection;
    }

    const QString &CDBusServer::sessionDBusServer()
    {
        static QString session("session");
        return session;
    }

    const QString &CDBusServer::systemDBusServer()
    {
        static QString system("system");
        return system;
    }

    QString CDBusServer::p2pAddress(const QString &host, const QString &port)
    {
        QString h = host.isEmpty() ? "127.0.0.1" : host.trimmed();
        QString p = port;

        // can handle host and port separately or combined
        // such as "myHost::1234"
        if (port.isEmpty())
        {
            if (h.contains(":"))
            {
                QStringList parts = h.split(":");
                // todo: Replace assert with input validation
                Q_ASSERT_X(parts.length() == 2, "p2pAdress", "Wrong IP string split");
                h = parts.at(0).trimmed();
                p = parts.at(1).trimmed();
            }
            else
            {
                p = "45000";
            }
        }

        // todo: Replace assert with input validation
        Q_ASSERT_X(CNetworkUtils::isValidIPv4Address(p), "p2pAdress", "Wrong IP in String");
        QString p2p = QString("tcp:host=%1,port=%2").arg(h).arg(p);
        return p2p;
    }

    QString CDBusServer::fixAddressToDBusAddress(const QString &address)
    {
        if (address.isEmpty() || address == sessionDBusServer() || address == systemDBusServer()) { return address; }
        if (address.startsWith("tcp:") || address.startsWith("unix:")) return address;
        return p2pAddress(address);
    }

    CDBusServer::ServerMode CDBusServer::addressToDBusMode(const QString &address)
    {
        QString a = address.toLower();
        if (a == CDBusServer::systemDBusServer())       { return SERVERMODE_SYSTEMBUS; }
        else if (a == CDBusServer::sessionDBusServer()) { return SERVERMODE_SESSIONBUS; }
        else { return SERVERMODE_P2P; }
    }

    bool CDBusServer::isDBusAvailable(const QString &address, int port, int timeoutMs)
    {
        QString m;
        return CNetworkUtils::canConnect(address, port, m, timeoutMs);
    }

    bool CDBusServer::isDBusAvailable(const QString &address, int port, QString &message, int timeoutMs)
    {
        return CNetworkUtils::canConnect(address, port, message, timeoutMs);
    }

    bool CDBusServer::isDBusAvailable(const QString &dbusAddress, QString &message, int timeoutMs)
    {
        if (dbusAddress.isEmpty()) { message = "no address"; return false; }
        if (isP2PAddress(dbusAddress))
        {
            QString host;
            int port = -1;
            if (splitDBusAddressIntoHostAndPort(dbusAddress, host, port))
            {
                return isDBusAvailable(host, port, message, timeoutMs);
            }
            else
            {
                return false;
            }
        }
        else
        {
            QDBusConnection connection(
                (dbusAddress == systemDBusServer()) ?
                QDBusConnection::systemBus() :
                QDBusConnection::connectToBus(QDBusConnection::SessionBus, ServiceName())
            );
            bool success = connection.isConnected();

            // further checks would need to go here
            // failing session bus not detected yet

            message = connection.lastError().message();
            connection.disconnectFromBus(ServiceName());
            return success;
        }
    }

    bool CDBusServer::isDBusAvailable(const QString &dbusAddress, int timeoutMs)
    {
        QString m;
        return isDBusAvailable(dbusAddress, m, timeoutMs);
    }

} // namespace
