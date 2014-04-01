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
        //! \brief Constructor
        CDBusServer(const QString &address, QObject *parent = nullptr);

        //!! \brief Adds a QObject to be exposed to DBus
        void addObject(const QString &name, QObject *object);

        //! \brief Last error
        QDBusError lastError() const;

        //! \brief Connected?
        bool isConnected() const
        {
            return this->m_busServer.isConnected();
        }

        //! \brief address
        QString address() const
        {
            return this->m_busServer.address();
        }

        //! \brief Connection by name
        const QDBusConnection getDbusConnection(const QString &connectionName) const
        {
            return this->m_DBusConnections.value(connectionName, CDBusServer::defaultConnection());
        }

        //! \brief Get DBbus connections
        const QList<QDBusConnection> getDbusConnections() const
        {
            // http://stackoverflow.com/questions/1124340/any-ideas-why-qhash-and-qmap-return-const-t-instead-of-const-t
            return this->m_DBusConnections.values();
        }

        //! \brief Default connection
        static const QDBusConnection &defaultConnection()
        {
            static QDBusConnection defaultConnection("default");
            return defaultConnection;
        }

    private slots:

        //! \brief Called when a new DBus client has connected
        bool newConnection(const QDBusConnection &connection);
    };
}

#endif // guard
