/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbusserver.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include <QProcess>
#include <QMetaClassInfo>

using namespace BlackMisc::Network;

namespace BlackMisc
{

    CDBusServer::CDBusServer(const QString &service, const QString &address, QObject *parent) : QObject(parent)
    {
        m_serverMode = modeOfAddress(address);
        switch (m_serverMode)
        {
        case SERVERMODE_SESSIONBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, coreServiceName());
                if (! connection.isConnected())
                {
                    launchDBusDaemon();
                    connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, coreServiceName());
                }

                if (! connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning("DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning("Cannot register DBus service, check server running: dbus-daemon.exe --session --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                QDBusConnection connection = QDBusConnection::systemBus();
                if (!connection.isConnected())
                {
                    launchDBusDaemon();
                    connection = QDBusConnection::systemBus();
                }

                if (!connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning("DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning("Cannot register DBus service, check server running: dbus-daemon.exe --system --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_P2P:
        default:
            {
                QString dbusAddress = isQtDBusAddress(address) ? address : "tcp:host=127.0.0.1,port=45000";
                dbusAddress = dbusAddress.toLower().trimmed().replace(' ', "");
                if (! dbusAddress.contains("bind=")) { dbusAddress = dbusAddress.append(",bind=*"); } // bind to all network interfaces

                m_busServer.reset(new QDBusServer(dbusAddress, parent));
                m_busServer->setAnonymousAuthenticationAllowed(true);

                // Note: P2P has no service name
                if (m_busServer->isConnected())
                {
                    CLogMessage(this).debug() << "Server listening on address:" << m_busServer->address();
                }
                else
                {
                    CLogMessage(this).warning("DBus P2P connection failed: %1") << lastQDBusServerError().message();
                }
                connect(m_busServer.data(), &QDBusServer::newConnection, this, &CDBusServer::ps_registerObjectsWithP2PConnection);
            }
            break;
        }
    }

    const QString &CDBusServer::coreServiceName()
    {
        static const QString sn = SWIFT_SERVICENAME;
        return sn;
    }

    void CDBusServer::launchDBusDaemon()
    {
        const QString program = QStringLiteral("dbus-daemon");
        const QStringList arguments = { QStringLiteral("--config-file=../share/dbus-1/session.conf") };
        bool success = QProcess::startDetached(program, arguments);
        if (!success) { CLogMessage(this).warning("Failed to launch dbus-daemon!"); }
    }

    bool CDBusServer::isP2PAddress(const QString &address)
    {
        return modeOfAddress(address) == SERVERMODE_P2P;
    }

    bool CDBusServer::dBusAddressToHostAndPort(QString address, QString &host, int &port)
    {
        address = address.trimmed().toLower().replace(' ', "");
        if (address.contains("host=") || address.contains("port="))
        {
            // "tcp:host=foo.com,port=123"
            QStringList parts(address.split(','));
            for (const QString &part : parts)
            {
                if (part.startsWith("host="))
                {
                    host = part.mid(part.lastIndexOf("=") + 1).trimmed();
                }
                else if (part.startsWith("port="))
                {
                    bool ok;
                    port = part.mid(part.lastIndexOf("=") + 1).trimmed().toInt(&ok);
                    if (! ok) { port = -1; }
                }
            }
            if (port < 0) { port = 45000; }
            if (host.isEmpty()) { host = "127.0.0.1"; }
            return true;
        }
        else
        {
            host = "";
            port = -1;
            return false;
        }
    }

    bool CDBusServer::isQtDBusAddress(const QString &address)
    {
        return address.startsWith("tcp:") || address.startsWith("unix:");
    }

    bool CDBusServer::isSessionOrSystemAddress(const QString &address)
    {
        return address == sessionBusAddress() || address == systemBusAddress();
    }

    QString CDBusServer::getDBusInterfaceFromClassInfo(QObject *object)
    {
        if (! object) { return ""; }
        const QMetaObject *mo = object->metaObject();
        for (int i = 0; i < mo->classInfoCount(); i++)
        {
            QMetaClassInfo ci = mo->classInfo(i);
            if (ci.name() == "D-Bus Interface") { return QString(ci.value()); }
        }
        return "";
    }

    bool CDBusServer::ps_registerObjectsWithP2PConnection(QDBusConnection connection)
    {
        Q_ASSERT(! m_objects.isEmpty());
        m_connections.insert(connection.name(), connection);
        CLogMessage(this).debug() << "New Connection from:" << connection.name();
        bool success = true;
        for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
        {
            CLogMessage(this).debug() << "Adding" << i.key() << getDBusInterfaceFromClassInfo(i.value()) << "to the new connection.";
            bool ok = connection.registerObject(i.key(), i.value(), registerOptions());
            Q_ASSERT_X(ok, "CDBusServer::newConnection", "Registration failed");
            if (! ok) { success = false; }
        }
        return success;
    }

    void CDBusServer::addObject(const QString &path, QObject *object)
    {
        if (! object) { return; }
        m_objects.insert(path, object); // will be registered when P2P connection is established

        switch (m_serverMode)
        {
        case SERVERMODE_SESSIONBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, coreServiceName());
                if (connection.registerObject(path, object, registerOptions()))
                {
                    CLogMessage(this).debug() << "Adding" << path << getDBusInterfaceFromClassInfo(object) << "to session bus.";
                }
                else
                {
                    CLogMessage(this).error("Error, no success with session bus registration");
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, coreServiceName());
                if (connection.registerObject(path, object, registerOptions()))
                {
                    CLogMessage(this).debug() << "Adding" << path << getDBusInterfaceFromClassInfo(object) << "to system bus.";
                }
                else
                {
                    CLogMessage(this).error("Error, no success with system bus registration");
                }
            }
            break;
        case SERVERMODE_P2P:
            break;
        default:
            Q_ASSERT_X(false, "CDBusServer::addObject", "Wrong server mode");
        }
    }

    QDBusError CDBusServer::lastQDBusServerError() const
    {
        if (! hasQDBusServer()) { return {}; }
        return m_busServer->lastError();
    }

    const QDBusServer *CDBusServer::qDBusServer() const
    {
        return m_busServer.data();
    }

    bool CDBusServer::hasQDBusServer() const
    {
        return ! m_busServer.isNull();
    }

    void CDBusServer::removeAllObjects()
    {
        for (const QString &path : m_objects.keys())
        {
            switch (m_serverMode)
            {
            case SERVERMODE_SESSIONBUS:
                QDBusConnection::sessionBus().unregisterObject(path);
                break;
            case SERVERMODE_SYSTEMBUS:
                QDBusConnection::systemBus().unregisterObject(path);
                break;
            case SERVERMODE_P2P:
                {
                    for(QDBusConnection connection : m_connections)
                    {
                        connection.unregisterObject(path);
                    }
                    break;
                }
            }
        }
    }

    const QDBusConnection &CDBusServer::defaultConnection()
    {
        static QDBusConnection defaultConnection("default");
        return defaultConnection;
    }

    const QString &CDBusServer::sessionBusAddress()
    {
        static QString session = "session";
        return session;
    }

    const QString &CDBusServer::systemBusAddress()
    {
        static QString system = "system";
        return system;
    }

    QString CDBusServer::p2pAddress(const QString &host, const QString &port)
    {
        QString h = host.isEmpty() ? "127.0.0.1" : host.trimmed();
        QString p = port;

        // can handle host and port separately or combined, e.g. "myhost:1234"
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
        Q_ASSERT_X(CNetworkUtils::isValidIPv4Address(p), "CDBusServer::p2pAddress", "Wrong IP in String");
        return QString("tcp:host=%1,port=%2").arg(h).arg(p);
    }

    QString CDBusServer::normalizeAddress(const QString &address)
    {
        if (address.isEmpty() || address == sessionBusAddress() || address == systemBusAddress()) { return address; }
        if (isQtDBusAddress(address)) { return address; }
        return p2pAddress(address);
    }

    CDBusServer::ServerMode CDBusServer::modeOfAddress(QString address)
    {
        address = address.toLower();
        if (address == systemBusAddress())       { return SERVERMODE_SYSTEMBUS; }
        else if (address == sessionBusAddress()) { return SERVERMODE_SESSIONBUS; }
        else                                     { return SERVERMODE_P2P; }
    }

    bool CDBusServer::isDBusAvailable(const QString &address, int port, int timeoutMs)
    {
        QString unused;
        return CNetworkUtils::canConnect(address, port, unused, timeoutMs);
    }

    bool CDBusServer::isDBusAvailable(const QString &address, int port, QString &o_message, int timeoutMs)
    {
        return CNetworkUtils::canConnect(address, port, o_message, timeoutMs);
    }

    bool CDBusServer::isDBusAvailable(const QString &dbusAddress, QString &o_message, int timeoutMs)
    {
        if (dbusAddress.isEmpty()) { o_message = "no address"; return false; }
        if (isP2PAddress(dbusAddress))
        {
            QString host;
            int port = -1;
            if (dBusAddressToHostAndPort(dbusAddress, host, port))
            {
                return isDBusAvailable(host, port, o_message, timeoutMs);
            }
            else
            {
                return false;
            }
        }
        else
        {
            QDBusConnection connection(dbusAddress == systemBusAddress() ? QDBusConnection::systemBus() : QDBusConnection::connectToBus(QDBusConnection::SessionBus, coreServiceName()));

            // todo: further checks would need to go here
            // failing session bus not detected yet

            o_message = connection.lastError().message();
            return connection.isConnected();
        }
    }

    bool CDBusServer::isDBusAvailable(const QString &dbusAddress, int timeoutMs)
    {
        QString unused;
        return isDBusAvailable(dbusAddress, unused, timeoutMs);
    }

} // namespace
