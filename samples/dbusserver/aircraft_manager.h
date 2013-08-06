/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AIRCRAFTMANAGER_H
#define AIRCRAFTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>

#include "remote_aircraft.h"

/*! 
 *  \brief     Plane Manager
 *  \details   This class manages all plane objects visible to the user
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */
class CAircraftManager : public QObject
{
    Q_OBJECT

    QList<CRemoteAircraft> m_aircraftList; //!< List of all aircrafts with their callsigns

public:

	/*!
     * \brief Constructor
     * \param parent
     */
    CAircraftManager(QObject *parent = 0);
	
	/*!
     * \brief Destructor
     */
    ~CAircraftManager();

    /*!
     * \brief Returns a list of all pilots with their callsigns
     * \return pilotList
     */
    QStringList pilotList() const;

    /*!
     * \brief Returns a list of all pilots with their callsigns
     * \return pilotList
     */
    QList<CRemoteAircraft> aircraftList() const;

    /*!
     * \brief Adds new aircraft
     * \return aircraft
     */
    void addAircraft( const CRemoteAircraft &aircraft );

signals:

public slots:
};

#endif // AIRCRAFTMANAGER_H
