/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DBUS_HANDLER_BASE_H
#define DBUS_HANDLER_BASE_H

#include <QObject>

namespace BlackCore {
    class CDBusServer;
}

namespace BlackMisc
{

	/*!
	 *  \brief     DBus Handler Base class
	 *  \details   This class implements the basic methods any DBus handler class needs to use. If you want
	 *             to implement your own DBus handler derive it from this one.
	 */

	class IDBusHandler : public QObject
	{
		Q_OBJECT

	protected:

        BlackCore::CDBusServer *m_dbusserver;  	//!< Our DBusServer
		QString m_dbusPath;			//!< DBus object path

		QObject *m_parent;          //!< Pointer to the parent plane manager object

	public:

		/*!
		 * \brief Default constructor
		 * \param parent
		 */
		IDBusHandler(QObject *parent = 0);

		/*!
		 * \brief Sets the DBusServer
		 * \param dbusServer
		 */
        void setDBusServer(BlackCore::CDBusServer *dbusServer);

		void setDBusObjectPath( const QString &dbusPath);
		
	signals:
		
	public slots:
		
	};

} // namespace BlackMisc

#endif // DBUS_HANDLER_BASE_H
