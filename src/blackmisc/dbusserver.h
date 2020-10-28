/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DBUSSERVER_H
#define BLACKMISC_DBUSSERVER_H

#include "blackmisc/logcategories.h"
#include "blackmisc/blackmiscexport.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QFlags>
#include <QMap>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QDBusServer;

//! Service name of DBus service
#define SWIFT_SERVICENAME "org.swift-project"

namespace BlackMisc
{
    /*!
     * Custom DBusServer
     * \details This class implements a custom DBusServer for DBus peer connections, but can also be used
     *          with session or system bus. For session/system bus this class represents no real server,
     *          but more a wrapper for QDBusConnection and object registration.
     */
    class BLACKMISC_EXPORT CDBusServer : public QObject
    {
        Q_OBJECT

    public:
        //! Default service name
        static const QString &coreServiceName();

        //! Service name, empty if is P2P connection
        static const QString &coreServiceName(const QDBusConnection &connection);

        //! Log categories
        static const QStringList &getLogCategories();

        //! Server mode
        enum ServerMode
        {
            SERVERMODE_P2P,
            SERVERMODE_SESSIONBUS,
            SERVERMODE_SYSTEMBUS
        };

        //! Construct a server for the core service
        CDBusServer(const QString &address, QObject *parent = nullptr) : CDBusServer(coreServiceName(), address, parent) {}

        //! Construct a server for some arbitrary service
        CDBusServer(const QString &service, const QString &address, QObject *parent = nullptr);

        //! Destructor
        virtual ~CDBusServer() override;

        //! Add a QObject to be exposed via DBus
        void addObject(const QString &name, QObject *object);

        //! Last error
        QDBusError lastQDBusServerError() const;

        //! DBus server (if using P2P)
        const QDBusServer *qDBusServer() const;

        //! True if using P2P
        bool hasQDBusServer() const;

        //! Remove all objects added with addObject
        void removeAllObjects();

        //! Default connection
        static const QDBusConnection &defaultConnection();

        //! Address denoting a session bus server
        static const QString &sessionBusAddress();

        //! Address denoting a system bus server
        static const QString &systemBusAddress();

        //! Connect to DBus
        static QDBusConnection connectToDBus(const QString &dbusAddress, const QString &name = {});

        //! Disconnect from Bus/Peer to peer
        static void disconnectFromDBus(const QDBusConnection &connection, const QString &dBusAddress);

        //! Address denoting a P2P server at the given host and port.
        //! \remarks Port number may be embedding in the host string after a colon.
        //! \return p2p address like "tcp:host=foo.bar.com,port=1234"
        static QString p2pAddress(const QString &host, const QString &port = "");

        //! P2P connection name
        static const QString &p2pConnectionName();

        //! Turn something like 127.0.0.1:45000 into "tcp:host=127.0.0.1,port=45000"
        //! \note Handles also "session" and "system" as valid address while CDBusServer::p2pAddress is for
        //! P2P addresses only.
        static QString normalizeAddress(const QString &address);

        //! Return the server mode of the given address
        static ServerMode modeOfAddress(QString address);

        //! Mode to string
        static const QString &modeToString(ServerMode mode);

        //! True if a valid Qt DBus address, e.g. "unix:tmpdir=/tmp", "tcp:host=127.0.0.1,port=45000"
        static bool isQtDBusAddress(const QString &address);

        //! True if address is session or system bus address
        //! \remark actually swift does NOT support system DBus anymore
        static bool isSessionOrSystemAddress(const QString &address);

        //! False if address is session or system bus address
        static bool isP2PAddress(const QString &address);

        //! False if address is session or system bus connection
        static bool isP2PConnection(const QDBusConnection &connection);

        //! Extract host and port from a DBus address
        static bool dBusAddressToHostAndPort(const QString &dbusAddress, QString &o_host, int &o_port);

        //! Extract host and port from a DBus address
        static bool dBusAddressToHostAndPort(const QString &dbusAddress, QString &o_host, QString &o_port);

        //! Is the given connection one of the default connections?
        static bool isQtDefaultConnection(const QDBusConnection &connection);

        //! Is there a DBus server running at the given address?
        //! \note This is an incomplete test showing too optimistic results for session DBus
        //! @{
        static bool isDBusAvailable(const QString &host, int port, int timeoutMs = 1500);
        static bool isDBusAvailable(const QString &host, int port, QString &message, int timeoutMs = 1500);
        static bool isDBusAvailable(const QString &dbusAddress, QString &message, int timeoutMs = 1500);
        static bool isDBusAvailable(const QString &dbusAddress, int timeoutMs = 1500);
        //! @}

    private:
        ServerMode m_serverMode = SERVERMODE_P2P;
        QScopedPointer<QDBusServer> m_busServer;
        QMap<QString, QObject *> m_objects;
        QMap<QString, QDBusConnection> m_connections;

        //! Interface name/value to class name
        static QString getDBusInterfaceFromClassInfo(QObject *object);

        //! Register options with connection
        static QDBusConnection::RegisterOptions registerOptions();

        //! Called when a new DBus client has connected in P2P mode
        bool registerObjectsWithP2PConnection(QDBusConnection connection);
    };
}

#endif // guard
