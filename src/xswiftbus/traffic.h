/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_TRAFFIC_H
#define BLACKSIM_XSWIFTBUS_TRAFFIC_H

//! \file

#include "datarefs.h"
#include "terrainprobe.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include <QObject>
#include <QHash>
#include <QVector>
#include <QStringList>
#include "XPMPMultiplayer.h"
#include <XPLM/XPLMDisplay.h>
#include <functional>
#include <utility>

//! \cond PRIVATE
#define XSWIFTBUS_TRAFFIC_INTERFACENAME "org.swift_project.xswiftbus.traffic"
#define XSWIFTBUS_TRAFFIC_OBJECTPATH "/xswiftbus/traffic"
//! \endcond

namespace BlackMisc { namespace Simulation { class CInterpolationHints; } }
namespace XSwiftBus
{
    /*!
     * XSwiftBus service object for traffic aircraft which is accessible through DBus
     */
    class CTraffic : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", XSWIFTBUS_TRAFFIC_INTERFACENAME)

    public:
        //! Constructor
        CTraffic(QObject *parent);

        //! Destructor
        virtual ~CTraffic();

        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(XSWIFTBUS_TRAFFIC_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(XSWIFTBUS_TRAFFIC_OBJECTPATH);
            return s;
        }

        //! Called by XPluginStart
        static void initLegacyData();

    signals:
        //! Signal emitted for each simulator rendering frame
        void simFrame();

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

        //! Add the position of a traffic aircraft
        void addPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading, qint64 relativeTime, qint64 timeOffset);

        //! Set the position of a traffic aircraft
        void setPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading);

        //! Add the flight control surfaces and lights of a traffic aircraft
        void addPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround, qint64 relativeTime, qint64 timeOffset);

        //! Set the flight control surfaces and lights of a traffic aircraft
        void setPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround);

        //! Set the transponder of a traffic aircraft
        void setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident);

        //! Set interpolation mode for a traffic aircraft
        void setInterpolatorMode(const QString &callsign, bool spline);

    private:
        bool m_initialized = false;
        bool m_enabled = false;

        static constexpr bool c_driverInterpolation = true;

        void emitSimFrame();

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
            BlackMisc::Simulation::CInterpolatorMulti interpolator;
            CTerrainProbe terrainProbe;
            BlackMisc::Simulation::CInterpolationHints hints();
            XPMPPlaneSurfaces_t surfaces;
            QVector<std::pair<qint64, std::function<void(Plane *)>>> pendingSurfaces;
            float targetGearPosition = 0;
            qint64 prevSurfacesLerpTime = 0;
            XPMPPlaneRadar_t xpdr;
            XPMPPlanePosition_t position;
            Plane(void *id_, QString callsign_, QString aircraftIcao_, QString airlineIcao_, QString livery_);
        };
        QHash<QString, Plane *> m_planesByCallsign;
        QHash<void *, Plane *> m_planesById;
        qint64 m_timestampLastSimFrame = QDateTime::currentMSecsSinceEpoch();

        int getPlaneData(void *id, int dataType, void *io_data);
        static int getPlaneData(void *id, int dataType, void *io_data, void *self)
        {
            return static_cast<CTraffic *>(self)->getPlaneData(id, dataType, io_data);
        }

        static int drawCallback(XPLMDrawingPhase phase, int isBefore, void *refcon);
    };
}

#endif // guard

