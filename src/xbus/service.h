/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_XBUS_SERVICE_H
#define BLACKSIM_XBUS_SERVICE_H

//! \file

#include "datarefs.h"
#include <QObject>

#define XBUS_SERVICE_INTERFACENAME "net.vatsim.PilotClient.XBus"
#define XBUS_SERVICE_OBJECTPATH "/XBus"

namespace XBus
{

    /*!
     * XBus service object which is accessible through DBus
     */
    class CService : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", XBUS_SERVICE_INTERFACENAME)

    public:
        //! Constructor
        CService(QObject *parent);

        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(XBUS_SERVICE_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(XBUS_SERVICE_OBJECTPATH);
            return s;
        }

        //! Called by XPluginReceiveMessage when the model changes.
        void onAircraftModelChanged();

    signals:
        //! Emitted when the model or livery changes.
        void aircraftModelChanged(const QString &path, const QString &filename, const QString &livery);

    public slots:
        //! Get major version number
        int getXPlaneVersionMajor() const;

        //! Get minor version number
        int getXPlaneVersionMinor() const;

        //! Get root of X-Plane install path
        QString getXPlaneInstallationPath() const;

        //! Get full path to X-Plane preferences file
        QString getXPlanePreferencesPath() const;

        //! Get aircraft latitude in degrees
        double getLatitude() const { return m_latitude.get(); }

        //! Get aircraft longitude in degrees
        double getLongitude() const { return m_longitude.get(); }

        //! Get aircraft altitude in meters
        double getAltitudeMSL() const { return m_elevation.get(); }

        //! Get aircraft height in meters
        double getHeightAGL() const { return m_agl.get(); }

        //! Get aircraft groundspeed in meters per second
        double getGroundSpeed() const { return m_groundSpeed.get(); }

        //! Get aircraft IAS in knots
        double getIndicatedAirspeed() const { return m_indicatedAirspeed.get(); }

        //! Get aircraft TAS in meters per second
        double getTrueAirspeed() const { return m_trueAirspeed.get(); }

        //! Get aircraft pitch in degrees above horizon
        double getPitch() const { return m_pitch.get(); }

        //! Get aircraft roll in degrees
        double getRoll() const { return m_roll.get(); }

        //! Get aircraft true heading in degrees
        double getTrueHeading() const { return m_heading.get(); }

    private:
        StringDataRef<xplane::data::sim::aircraft::view::acf_livery_path> m_liveryPath;
        DataRef<xplane::data::sim::flightmodel::position::latitude> m_latitude;
        DataRef<xplane::data::sim::flightmodel::position::longitude> m_longitude;
        DataRef<xplane::data::sim::flightmodel::position::elevation> m_elevation;
        DataRef<xplane::data::sim::flightmodel::position::y_agl> m_agl;
        DataRef<xplane::data::sim::flightmodel::position::groundspeed> m_groundSpeed;
        DataRef<xplane::data::sim::flightmodel::position::indicated_airspeed2> m_indicatedAirspeed;
        DataRef<xplane::data::sim::flightmodel::position::true_airspeed> m_trueAirspeed;
        DataRef<xplane::data::sim::flightmodel::position::theta> m_pitch;
        DataRef<xplane::data::sim::flightmodel::position::phi> m_roll;
        DataRef<xplane::data::sim::flightmodel::position::psi> m_heading;
    };

}

#endif // guard

