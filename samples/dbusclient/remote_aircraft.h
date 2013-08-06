/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef REMOTE_AIRCRAFT_H
#define REMOTE_AIRCRAFT_H

#include <QtDBus>


/*!
 *  \brief     Remove Aircraft
 *  \details   This class represents a aircraft from another user in the network
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */
class CRemoteAircraft
{
    QString m_callsign; //!< Aircrafts callsign

    double m_heading; //!< Aircrafts heading.

    double m_groundSpeed; //!< Aircrafts groundspeed in knots

    QString m_wakeTurbulence; //!< wake turbulence classification

public:

    /*!
     * \brief Constructor
     */
    CRemoteAircraft();

    /*!
     * \brief Copy constructor
     * \param other
     */
    CRemoteAircraft(const CRemoteAircraft &other);

    /*!
     * \brief Assignment operator
     * \param other
     */
    CRemoteAircraft &operator=(const CRemoteAircraft &other);

    /*!
     * \brief Destructor
     */
    ~CRemoteAircraft() {}

    friend QDBusArgument &operator<<(QDBusArgument &argument, const CRemoteAircraft &remoteAircraft);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, CRemoteAircraft &remoteAircraft);

    /*!
     * \brief Aircrafts Callsign
     */
    QString getCallsign() const { return m_callsign; }

    /*!
     * \brief Aircrafts heading
     */
    double getHeading() const { return m_heading; }

    /*!
     * \brief Aircrafts ground speed
     */
    double getGroundSpeed() const { return m_groundSpeed; }

    /*!
     * \brief Aircrafts wake turbulence classification
     */
    QString getWakeTurbulence() const { return m_wakeTurbulence; }

    static void registerMetaType();

};

typedef QList<CRemoteAircraft> CRemoteAircraftList;

Q_DECLARE_METATYPE(CRemoteAircraft)
Q_DECLARE_METATYPE(CRemoteAircraftList)

#endif // REMOTE_AIRCRAFT_H
