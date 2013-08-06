/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PLANEMANAGERHANDLER_H
#define PLANEMANAGERHANDLER_H

#include <QObject>
#include <QStringList>

#include "blackmisc/dbus_handler.h"
#include "remote_aircraft.h"

class CAircraftManager;

/*! 
 *  \brief     Plane Manager wrapper for DBus
 *  \details   This class handles the DBus communication for the PlaneManager
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */
class CAircraftManagerHandler : public BlackMisc::IDBusHandler
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient.PlaneManager")
    Q_PROPERTY( QList<CRemoteAircraft> aircraftList READ aircraftList)
	
    QList<CRemoteAircraft> aircraftList() const;

public:

	/*!
     * \brief Default constructor
	 * \param parent
     */
    CAircraftManagerHandler(QObject *parent);
	
	/*!
     * \brief Destructor
     */
    ~CAircraftManagerHandler() {}
    
signals:
    
public slots:

private:
    
};

#endif // PLANEMANAGERHANDLER_H
