/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_DBUSSERVER_H
#define BLACKCORE_DBUSSERVER_H

#include "blackmisc/valueobject.h" // for qHash overload, include before Qt stuff due GCC issue
#include <QObject>
#include <QtDBus/QDBusServer>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusConnection>
#include <QStringList>
#include <QMap>

#define BLACKCORE_DBUSERVER_SERVICENAME "org.vatsim.pilotClient"


namespace BlackCore
{

    /*!
     *  \brief     Custom DBusServer
     *  \details   This class implements a custom DBusServer for DBus peer connections, but can also be used as session or system bus
     */
    class CDBusServer : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_DBUSERVER_SERVICENAME)

    public:
        //! \brief Service name of DBus serve
        static const QString ServiceName;

        //! \brief Server mode, normally P2P, but can be changed for debugging / testing
        enum ServerMode
        {
            SERVERMODE_P2P,
            SERVERMODE_SESSIONBUS,
            SERVERMODE_SYSTEMBUS
        };

    private:
        QDBusServer m_busServer; //!< QDBusServer implementation
        ServerMode m_serverMode;
        QMap<QString, QObject *> m_objects; //!< Mapping of all exposed objects
        QMap<QString, QDBusConnection> m_DBusConnections; //!< Mapping of all DBusConnection objects

        //! \brief Check if address means a real server with P2P connection
        static bool isP2P(const QString &address);

        //! \brief Get the class info
        static const QString getClassInfo(QObject *object);

        //! \brief Register options with connection
        static const QDBusConnection::RegisterOptions &RegisterOptions()
        {
            static QDBusConnection::RegisterOptions opt = QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllSlots;
            return opt;
        }

    public:
        //! Constructor
        //! \remarks We are using address and not ServerMode, as on some systems we need to pass in some specific configuration string
        //! \sa QDBusServer
        CDBusServer(const QString &address, QObject *parent = nullptr);

        //!! Adds a QObject to be exposed to DBus
        void addObject(const QString &name, QObject *object);

        //! Last error
        QDBusError lastError() const;

        //! Connected?
        bool isConnected() const { return this->m_busServer.isConnected(); }

        //! address
        QString address() const { return this->m_busServer.address(); }

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

    private slots:

        //! \brief Called when a new DBus client has connected in P2P mode
        bool registerObjectsWithConnection(const QDBusConnection &connection);
    };
}

#endif // guard
