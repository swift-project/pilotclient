/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
        // Application Options:
        // -h, –host=HOSTNAME Hostname or IP of the remote host
        // -p, –port-ssh=PORT-SSH SSH port on the remote host
        // -u, –username=USERNAME SSH username on the remote host
        // -w, –password=PASSWORD SSH password on the remote host
        // -m, –method=DBUS_TRANSPORT_METHOD The D-Bus transport method to use (TCP, UNIX, abstract-UNIX)
        // -b, –bind=HOSTNAME The bind-address to listen for D-Bus client connections on
        // -d, –bus-address=BUS_ADDRESS The DBus session bus address of the remote D-Bus daemon
        // -t, –port-tcp=PORT-TCP The TCP port to listen for DBus client connections on
        // -v, –verbose=VERBOSE Set verbosity level (3, 2, 1, 0, -1)=(packet,protocol,functions,important,none)

        static const QString desc("Mode: %1 Address: '%2' Service: '%3'");
        m_serverMode = CDBusServer::modeOfAddress(address);
        this->setObjectName(desc.arg(CDBusServer::modeToString(m_serverMode), address, service.isEmpty() ? "-" : service));
        switch (m_serverMode)
        {
        case SERVERMODE_SESSIONBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, coreServiceName());
                connection.unregisterService(service); // allow reconnecting by removing still registered service
                if (! connection.isConnected() || ! connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning(u"DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning(u"Cannot register DBus service, check server running: dbus-daemon.exe --session --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, coreServiceName());
                connection.unregisterService(service); // allow reconnecting by removing still registered service
                if (! connection.isConnected() || ! connection.registerService(service))
                {
                    // registration fails can either mean something wrong with DBus or service already exists
                    CLogMessage(this).warning(u"DBus registration: %1") << connection.lastError().message();
                    CLogMessage(this).warning(u"Cannot register DBus service, check server running: dbus-daemon.exe --system --address=tcp:host=192.168.0.133,port=45000");
                }
            }
            break;
        case SERVERMODE_P2P:
        default:
            {
                QString dbusAddress = isQtDBusAddress(address) ? address : "tcp:host=127.0.0.1,port=45000";
                dbusAddress = dbusAddress.toLower().trimmed().replace(' ', "");
                if (! dbusAddress.contains("bind=")) { dbusAddress = dbusAddress.append(",bind=*"); } // bind to all network interfaces

                m_busServer.reset(new QDBusServer(dbusAddress, this));
                m_busServer->setObjectName("QDBusServer: " + this->objectName());
                m_busServer->setAnonymousAuthenticationAllowed(true);

                // Note: P2P has no service name
                if (m_busServer->isConnected())
                {
                    CLogMessage(this).info(u"DBus P2P Server listening on address: '%1'") << m_busServer->address();
                }
                else
                {
                    CLogMessage(this).warning(u"DBus P2P connection failed: %1") << lastQDBusServerError().message();
                }
                connect(m_busServer.data(), &QDBusServer::newConnection, this, &CDBusServer::registerObjectsWithP2PConnection);
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

    const QString &CDBusServer::coreServiceName(const QDBusConnection &connection)
    {
        static const QString empty;
        return CDBusServer::isP2PConnection(connection) ? empty : CDBusServer::coreServiceName();
    }

    const QStringList &CDBusServer::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::dbus() });
        return cats;
    }

    bool CDBusServer::isP2PAddress(const QString &address)
    {
        return modeOfAddress(address) == SERVERMODE_P2P;
    }

    bool CDBusServer::isP2PConnection(const QDBusConnection &connection)
    {
        if (CDBusServer::isQtDefaultConnection(connection)) { return false; }
        return connection.name().contains(p2pConnectionName());
    }

    bool CDBusServer::dBusAddressToHostAndPort(const QString &address, QString &host, int &port)
    {
        const QString canonicalAddress = address.trimmed().toLower().replace(' ', "");
        if (canonicalAddress.contains("host=") || canonicalAddress.contains("port="))
        {
            // "tcp:host=foo.com,port=123"
            const QStringList parts(canonicalAddress.split(','));
            for (const QString &part : parts)
            {
                // "host=" or "tcp:host="
                if (part.contains("host=", Qt::CaseInsensitive))
                {
                    const QString h = part.mid(part.lastIndexOf("=") + 1).trimmed();
                    host = h;
                }
                else if (part.contains("port=", Qt::CaseInsensitive))
                {
                    const QString p = part.mid(part.lastIndexOf("=") + 1).trimmed();
                    bool ok;
                    port = p.toInt(&ok);
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

    bool CDBusServer::dBusAddressToHostAndPort(const QString &dbusAddress, QString &o_host, QString &o_port)
    {
        int port;
        const bool s = dBusAddressToHostAndPort(dbusAddress, o_host, port);
        o_port = QString::number(port);
        return s;
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
        if (! object) { return {}; }
        const QMetaObject *mo = object->metaObject();
        for (int i = 0; i < mo->classInfoCount(); i++)
        {
            const QMetaClassInfo ci = mo->classInfo(i);
            const QString name = QString(ci.name()).toLower();
            if (name == "d-bus interface") { return QString(ci.value()); }
        }
        return {};
    }

    QDBusConnection::RegisterOptions CDBusServer::registerOptions()
    {
        return QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllSlots;
        // return QDBusConnection::ExportAllContents;
    }

    bool CDBusServer::registerObjectsWithP2PConnection(QDBusConnection connection)
    {
        Q_ASSERT(! m_objects.isEmpty());
        m_connections.insert(connection.name(), connection);
        CLogMessage(this).info(u"New Connection from: '%1'") << connection.name();
        bool success = true;
        for (auto i = m_objects.cbegin(); i != m_objects.cend(); ++i)
        {
            const QString key(i.key());
            const bool ok = connection.registerObject(key, i.value(), registerOptions());
            if (ok)
            {
                CLogMessage(this).info(u"Adding '%1' to the new connection '%2'") << key << this->getDBusInterfaceFromClassInfo(i.value());
            }
            else
            {
                CLogMessage(this).info(u"Adding '%1' failed, connection '%2', error '%3'") << key << this->getDBusInterfaceFromClassInfo(i.value()) << connection.lastError().message();
                success = false;
            }
        }
        return success;
    }

    void CDBusServer::addObject(const QString &path, QObject *object)
    {
        if (! object) { return; }
        m_objects.insert(path, object); // will be registered when P2P connection is established

        QObject::connect(object, &QObject::destroyed, this, [this, path] { m_objects.remove(path); });

        switch (m_serverMode)
        {
        case SERVERMODE_SESSIONBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, coreServiceName());
                if (connection.registerObject(path, object, registerOptions()))
                {
                    CLogMessage(this).info(u"Adding '%1' '%2' to session DBus") << path << getDBusInterfaceFromClassInfo(object);
                }
                else
                {
                    CLogMessage(this).error(u"Error adding '%1' '%2' to session DBus: '%3'") << path << getDBusInterfaceFromClassInfo(object) << connection.lastError().message();
                }
            }
            break;
        case SERVERMODE_SYSTEMBUS:
            {
                QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, coreServiceName());
                if (connection.registerObject(path, object, registerOptions()))
                {
                    CLogMessage(this).info(u"Adding '%1' '%2' to system DBus") << path << getDBusInterfaceFromClassInfo(object);
                }
                else
                {
                    CLogMessage(this).error(u"Error adding '%1' '%2' to system DBus: '%3'") << path << getDBusInterfaceFromClassInfo(object) << connection.lastError().message();
                }
            }
            break;
        case SERVERMODE_P2P:
            {
                for (QDBusConnection connection : std::as_const(m_connections))
                {
                    if (connection.registerObject(path, object, registerOptions()))
                    {
                        CLogMessage(this).info(u"Adding '%1' '%2' to P2P DBus '%3'") << path << getDBusInterfaceFromClassInfo(object) << connection.name();
                    }
                    else
                    {
                        CLogMessage(this).error(u"Error adding '%1' '%2' to P2P DBus '%3': '%4'") << path << getDBusInterfaceFromClassInfo(object) << connection.name() << connection.lastError().message();
                    }
                }
            }
            break;
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong server mode");
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
        return !m_busServer.isNull();
    }

    void CDBusServer::removeAllObjects()
    {
        for (const QString &path : makeKeysRange(std::as_const(m_objects)))
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
                for (QDBusConnection connection : std::as_const(m_connections))
                {
                    connection.unregisterObject(path);
                }
                break;
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
            // cppcheck-suppress ignoredReturnValue
            p.toShort(&ok);
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
        return QStringLiteral("tcp:host=%1,port=%2").arg(h, p);
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

    const QString &CDBusServer::modeToString(CDBusServer::ServerMode mode)
    {
        static const QString p2p = "P2P";
        static const QString session = "session";
        static const QString system = "system";

        switch (mode)
        {
        case SERVERMODE_P2P: return p2p;
        case SERVERMODE_SYSTEMBUS: return system;
        case SERVERMODE_SESSIONBUS:
        default: break;
        }
        return session;
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
