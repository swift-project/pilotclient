/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DBUSSERVER_H
#define DBUSSERVER_H

#include <QObject>
#include <QtDBus/QDBusServer>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusConnection>
#include <QStringList>
#include <QMap>

namespace BlackCore {

	/*! 
	 *  \brief     Custom DBusServer
	 *  \details   This class implements a custom DBusServer for DBus peer connections
	 */

    class CDBusServer : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient")

	private:
		QDBusServer m_busServer; //!< QDBusServer implementation

		QMap<QString, QObject*> m_objects; //!< Mapping of all exposed objects

		QMap<QString, QDBusConnection> m_DBusConnections; //!< Mapping of all DBusConnection objects

	public:

		/*!
		 * \brief Constructor
		 * \param parent
		 */
        CDBusServer(const QString &address, QObject *parent = 0);

		/*!
		 * \brief Adds a QObject to be exposed to DBus
		 * \param name
		 * \param object
		 */
		void addObject(const QString &name, QObject *object);

		void printError();

	public slots:

		/*!
		 * \brief Called when a new DBus client has connected
		 * \param connection
		 */
		void newConnection(const QDBusConnection & connection);
	signals:
	};

}

#endif // DBUSSERVER_H
