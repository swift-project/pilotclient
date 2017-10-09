/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/dbusserver.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/processctrl.h"
#include "blackmisc/statusmessage.h"

#include <QDBusServer>
#include <QMetaClassInfo>
#include <QMetaObject>
#include <QStringList>
#include <QThread>
#include <QtGlobal>

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
                if (! connection.isConnected() || ! connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning("DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning("Cannot register DBus service, check server running: dbus-daemon.exe --session --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, coreServiceName());
                if (! connection.isConnected() || ! connection.registerService(service))
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

    CDBusServer::~CDBusServer()
    {
        this->removeAllObjects();
        QDBusConnection::disconnectFromBus(coreServiceName());
    }

    const QString &CDBusServer::coreServiceName()
    {
        static const QString sn = SWIFT_SERVICENAME;
        return sn;
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
            const QStringList parts(address.split(','));
            for (const QString &part : parts)
            {
                if (part.startsWith("host="))
                {
                    host = part.midRef(part.lastIndexOf("=") + 1).trimmed().toString();
                }
                else if (part.startsWith("port="))
                {
                    bool ok;
                    port = part.midRef(part.lastIndexOf("=") + 1).trimmed().toInt(&ok);
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

    bool CDBusServer::isQtDefaultConnection(const QDBusConnection &connection)
    {
        return connection.name() == QDBusConnection::sessionBus().name() ||
               connection.name() == QDBusConnection::systemBus().name();
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
            QString name(ci.name());
            if (name == "D-Bus Interface") { return QString(ci.value()); }
        }
        return "";
    }

    bool CDBusServer::ps_registerObjectsWithP2PConnection(QDBusConnection connection)
    {
        Q_ASSERT(! m_objects.isEmpty());
        m_connections.insert(connection.name(), connection);
        CLogMessage(this).debug() << "New Connection from:" << connection.name();
        bool success = true;
        for (auto i = m_objects.cbegin(); i != m_objects.cend(); ++i)
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
            {
                for (QDBusConnection connection : as_const(m_connections))
                {
                    if (connection.registerObject(path, object, registerOptions()))
                    {
                        CLogMessage(this).debug() << "Adding" << path << getDBusInterfaceFromClassInfo(object) << "to" << connection.name();
                    }
                    else
                    {
                        CLogMessage(this).error("Error, no success with %1 registration") << connection.name();
                    }
                }
            }
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
        if (m_objects.isEmpty()) { return; }
        for (const QString &path : makeKeysRange(as_const(m_objects)))
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
                    for (QDBusConnection connection : as_const(m_connections))
                    {
                        connection.unregisterObject(path);
                    }
                    break;
                }
            }
        }
        m_objects.clear();
    }

    const QDBusConnection &CDBusServer::defaultConnection()
    {
        static const QDBusConnection defaultConnection("default");
        return defaultConnection;
    }

    const QString &CDBusServer::sessionBusAddress()
    {
        static const QString session("session");
        return session;
    }

    const QString &CDBusServer::systemBusAddress()
    {
        static const QString system("system");
        return system;
    }

    QDBusConnection CDBusServer::connectToDBus(const QString &dBusAddress, const QString &name)
    {
        if (dBusAddress == sessionBusAddress())
        {
            if (name.isEmpty()) return QDBusConnection::sessionBus();
            return QDBusConnection::connectToBus(QDBusConnection::SessionBus, name);
        }
        else if (dBusAddress == systemBusAddress())
        {
            if (name.isEmpty()) return QDBusConnection::systemBus();
            return QDBusConnection::connectToBus(QDBusConnection::SystemBus, name);
        }
        else if (isP2PAddress(dBusAddress))
        {
            return QDBusConnection::connectToPeer(dBusAddress,
                                                  name.isEmpty() ? CDBusServer::p2pConnectionName() : name);
        }
        return QDBusConnection("invalid");
    }

    void CDBusServer::disconnectFromDBus(const QDBusConnection &connection, const QString &dBusAddress)
    {
        if (CDBusServer::isQtDefaultConnection(connection)) return; // do not touch the default connections
        if (CDBusServer::isP2PAddress(dBusAddress))
        {
            QDBusConnection::disconnectFromPeer(connection.name());
        }
        else
        {
            QDBusConnection::disconnectFromBus(connection.name());
        }
    }

    QString CDBusServer::p2pAddress(const QString &host, const QString &port)
    {
        QString h = host.trimmed().toLower().remove(' ');
        if (h.isEmpty()) { h = "127.0.0.1"; }

        // check port
        bool ok = false;
        QString p = port.toLower().trimmed();
        if (!p.isEmpty())
        {
            p.toInt(&ok);
            if (!ok)
            {
                p = ""; // was not a number
            }
        }

        // can handle host and port separately or combined, e.g. "myhost:1234"
        if (port.isEmpty())
        {
            if (h.startsWith("tcp:") && h.contains("host=") && h.contains("port="))
            {
                // looks we already got a full string
                return h;
            }

            // 192.168.5.3:9300 style
            if (h.contains(":"))
            {
                const QStringList parts = h.split(":");
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
        return QString("tcp:host=%1,port=%2").arg(h, p);
    }

    const QString &CDBusServer::p2pConnectionName()
    {
        static const QString n("p2pConnection");
        return n;
    }

    QString CDBusServer::normalizeAddress(const QString &address)
    {
        const QString lc(address.toLower().trimmed());

        if (lc.isEmpty()) { return sessionBusAddress(); }
        if (lc == sessionBusAddress() || lc == systemBusAddress()) { return lc; }

        // some aliases
        if (lc.startsWith("sys")) { return systemBusAddress(); }
        if (lc.startsWith("ses")) { return sessionBusAddress(); }

        // Qt / P2P
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

    bool CDBusServer::isDBusAvailable(const QString &address, int port, QString &message, int timeoutMs)
    {
        return CNetworkUtils::canConnect(address, port, message, timeoutMs);
    }

    bool CDBusServer::isDBusAvailable(const QString &dBusAddress, QString &message, int timeoutMs)
    {
        if (dBusAddress.isEmpty()) { message = "No address."; return false; }
        if (isP2PAddress(dBusAddress))
        {
            QString host;
            int port = -1;
            return CDBusServer::dBusAddressToHostAndPort(dBusAddress, host, port) ?
                   CDBusServer::isDBusAvailable(host, port, message, timeoutMs) :
                   false;
        }
        else
        {
            QDBusConnection connection = CDBusServer::connectToDBus(dBusAddress);
            const bool isConnected = connection.isConnected();
            message = connection.lastError().message();
            CDBusServer::disconnectFromDBus(connection, dBusAddress);
            return isConnected;
        }
    }

    bool CDBusServer::isDBusAvailable(const QString &dbusAddress, int timeoutMs)
    {
        QString unused;
        return CDBusServer::isDBusAvailable(dbusAddress, unused, timeoutMs);
    }
} // namespace
