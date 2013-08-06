/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FSDCLIENTHANDLER_H
#define FSDCLIENTHANDLER_H

#include <QObject>

#include <BlackMisc/dbus_handler.h>

class CFsdClient;

/*! 
 *  \brief     FSD client wrapper for DBus
 *  \details   This class handles the DBus communication for the FSD client
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */
class CFsdClientHandler : public BlackMisc::IDBusHandler
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient.FsdClient")
public:

	/*!
     * \brief Default constructor
	 * \param parent
     */
    CFsdClientHandler(QObject *parent);
	
	/*!
     * \brief Destructor
     */
    ~CFsdClientHandler() {}

	/*!
     * \brief Starts connection to FSD server
     * \param host
     */
    Q_INVOKABLE void connectTo(const QString &host);
    
signals:
    void connectedTo( const QString &host);
    
public slots:
    
};

#endif // FSDCLIENTHANDLER_H
