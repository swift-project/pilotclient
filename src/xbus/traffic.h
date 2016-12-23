/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XBUS_TRAFFIC_H
#define BLACKSIM_XBUS_TRAFFIC_H

//! \file

#include "datarefs.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include <QObject>
#include <QHash>
#include <QVector>
#include <QStringList>
#include "XPMPMultiplayer.h"

//! \cond PRIVATE
#define XBUS_TRAFFIC_INTERFACENAME "org.swift_project.xbus.traffic"
#define XBUS_TRAFFIC_OBJECTPATH "/xbus/traffic"
//! \endcond

namespace BlackMisc { class IInterpolator; }

namespace XBus
{

    /*!
     * XBus service object for traffic aircraft which is accessible through DBus
     */
    class CTraffic : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", XBUS_TRAFFIC_INTERFACENAME)

    public:
        //! Constructor
        CTraffic(QObject *parent);

        //! Destructor
        virtual ~CTraffic();

        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(XBUS_TRAFFIC_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(XBUS_TRAFFIC_OBJECTPATH);
            return s;
        }

        //! Called by XPluginStart
        static void initLegacyData();

    signals:
        //! Installed models updated.
        void installedModelsUpdated(const QStringList &modelStrings, const QStringList &icaos, const QStringList &airlines, const QStringList &liveries);

    public slots:
        //! Initialize the multiplayer planes rendering and return true if successful
        bool initialize();

        //! Reverse the actions of initialize().
        void cleanup();

        //! Load a collection of planes from the given directory and return true if successful
        bool loadPlanesPackage(const QString &path);

        //! Set the ICAO code to use for aircraft without a model match
        void setDefaultIcao(const QString &defaultIcao);

        //! Set whether the plugin draws type and callsign labels above aircraft
        void setDrawingLabels(bool drawing);

        //! Get whether the plugin draws type and callsign labels above aircraft
        bool isDrawingLabels() const;

        //! Called by newly connected client to cause installedModelsUpdated to be emitted.
        void updateInstalledModels();

        //! Set the maximum number of aircraft.
        void setMaxPlanes(int planes);

        //! Set the maximum distance at which to draw aircraft (nautical miles).
        void setMaxDrawDistance(double nauticalMiles);

        //! Introduce a new traffic aircraft
        void addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

        //! Remove a traffic aircraft
        void removePlane(const QString &callsign);

        //! Remove all traffic aircraft
        void removeAllPlanes();

        //! Set the position of a traffic aircraft
        void addPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading, qint64 relativeTime);

        //! Set the flight control surfaces and lights of a traffic aircraft
        void setPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern);

        //! Set the transponder of a traffic aircraft
        void setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident);

    private:
        bool m_initialized = false;
        bool m_enabled = false;

        static int preferences(const char *section, const char *name, int def);
        static float preferences(const char *section, const char *name, float def);

        struct Plane
        {
            void *id = nullptr;
            QString callsign;
            QString aircraftIcao;
            QString airlineIcao;
            QString livery;
            bool hasSurfaces = false;
            bool hasXpdr = false;
            char label[32] {};
            BlackMisc::Aviation::CAircraftSituationList situations;
            XPMPPlaneSurfaces_t surfaces;
            XPMPPlaneRadar_t xpdr;
            Plane(void *id_, QString callsign_, QString aircraftIcao_, QString airlineIcao_, QString livery_);
        };
        QHash<QString, Plane *> m_planesByCallsign;
        QHash<void *, Plane *> m_planesById;

        int getPlaneData(void *id, int dataType, void *io_data);
        static int getPlaneData(void *id, int dataType, void *io_data, void *self)
        {
            return static_cast<CTraffic *>(self)->getPlaneData(id, dataType, io_data);
        }

        BlackMisc::IInterpolator *m_interpolator = nullptr;
    };

}

#endif // guard

