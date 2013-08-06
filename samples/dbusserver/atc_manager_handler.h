/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ATCMANAGERHANDLER_H
#define ATCMANAGERHANDLER_H

#include <QObject>
#include <QStringList>
#include "blackmisc/dbus_handler.h"

class CAtcManager;

/*! 
 *  \brief     ATC Manager Wrapper for DBus
 *  \details   This class handles the DBus communication for the ATC manager
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */
class CAtcManagerHandler : public BlackMisc::IDBusHandler
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient.AtcManager")
    Q_PROPERTY( QStringList atcList READ atcList)

	/**
	 * Return QStringList of controller callsigns
	 * This needs to be private, since you should not call this
	 * directly in your application. Use the parents method instead.
     */
    QStringList atcList() const;

public:

	/*!
     * \brief Default constructor
	 * \param parent
     */
    CAtcManagerHandler(QObject *parent);
	
	/*!
     * \brief Destructor
     */
    ~CAtcManagerHandler();
};

#endif // ATCMANAGERHANDLER_H
