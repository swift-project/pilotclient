/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DBUSSERVER_H
#define BLACKCORE_DBUSSERVER_H

#include "blackmisc/valueobject.h" // for qHash overload, include before Qt stuff due GCC issue
#include <QObject>
#include <QtDBus/QDBusServer>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusConnection>
#include <QStringList>
#include <QMap>

//! Service name of DBus service
#define BLACKCORE_RUNTIME_SERVICENAME "org.swift.pilotclient"

namespace BlackCore
{

    /*!
     *  Custom DBusServer
     *  \details This class implements a custom DBusServer for DBus peer connections, but can also be used
     *           with session or system bus. For session / system bus this class represents no real server,
     *           but more a wrapper for \sa QDBusConnection and the registered objects
     */
    class CDBusServer : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_RUNTIME_SERVICENAME)

    public:
        //! Service name of DBus server
        static const QString &ServiceName();

        //! Server mode, normally P2P, but can be changed for debugging / testing
        enum ServerMode
        {
            SERVERMODE_P2P,
            SERVERMODE_SESSIONBUS,
            SERVERMODE_SYSTEMBUS
        };

        //! Construct a server for the BlackCore runtime
        //! \remarks We are using address and not ServerMode, as on some systems we need to pass in some specific configuration string
        //! \sa QDBusServer
        CDBusServer(const QString &address, QObject *parent = nullptr) : CDBusServer(CDBusServer::ServiceName(), address, parent) {}

        //! Construct a server for some arbitrary service
        CDBusServer(const QString &service, const QString &address, QObject *parent = nullptr);

        //!! Adds a QObject to be exposed to DBus
        void addObject(const QString &name, QObject *object);

        //! Last error
        QDBusError lastQDBusServerError() const;

        //! DBus server (if avaialable)
        const QDBusServer *qDBusServer() const;

        //! With (P2P) DBus server
        bool hasQDBusServer() const;

        //! Unregister all objects
        void unregisterAllObjects();

        //! Default connection
        static const QDBusConnection &defaultConnection()
        {
            static QDBusConnection defaultConnection("default");
            return defaultConnection;
        }

        //! Denotes a session DBus server
        static const QString &sessionDBusServer()
        {
            static QString session("session");
            return session;
        }

        //! Denotes a session DBus server
        static const QString &systemDBusServer()
        {
            static QString system("system");
            return system;
        }

        //! Denotes a P2P DBus server, e.g. "tcp:host=192.168.3.3,port=45000"
        //! \remarks it is valid to pass only one string as host:port
        static QString p2pAddress(const QString &host = "127.0.0.1", const QString &port = "");

        //! Turn something like 127.0.0.1:45000 into "tcp:host=127.0.0.1,port=45000"
        static QString fixAddressToDBusAddress(const QString &address);

        //! address to DBus server mode
        static ServerMode addressToDBusMode(const QString &address);

        //! Qt DBus address, e.g. "unix:tmpdir=/tmp", "tcp:host=127.0.0.1,port=45000"
        static bool isQtDBusAddress(const QString &address);

    private:
        ServerMode m_serverMode = SERVERMODE_P2P;
        QScopedPointer<QDBusServer> m_busServer; //!< QDBusServer implementation
        QMap<QString, QObject *> m_objects;      //!< Mapping of all exposed objects, for P2P registration when connection establishes, also to later unregister objects
        QMap<QString, QDBusConnection> m_DBusConnections; //!< Mapping of all DBusConnection objects

        //! Check if address means a real server with P2P connection
        static bool isP2P(const QString &address);

        //! Get the class info
        static const QString getClassInfo(QObject *object);

        //! Register options with connection
        static const QDBusConnection::RegisterOptions &RegisterOptions()
        {
            static QDBusConnection::RegisterOptions opt = QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllSlots;
            return opt;
        }

    private slots:

        //! Called when a new DBus client has connected in P2P mode
        bool ps_registerObjectsWithP2PConnection(const QDBusConnection &connection);
    };
}

#endif // guard
