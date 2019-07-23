/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_XSWIFTBUS_TRAFFIC_PROXY_H
#define BLACKSIMPLUGIN_XSWIFTBUS_TRAFFIC_PROXY_H

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/geo/elevationplane.h"

#include <QObject>
#include <QString>
#include <QStringList>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

//! \cond PRIVATE
#define XSWIFTBUS_TRAFFIC_INTERFACENAME "org.swift_project.xswiftbus.traffic"
#define XSWIFTBUS_TRAFFIC_OBJECTPATH "/xswiftbus/traffic"
//! \endcond

namespace BlackSimPlugin
{
    namespace XPlane
    {
        //! List of doubles
        using QDoubleList = QList<double>;

        //! Planes positions
        struct PlanesPositions
        {
            //! Is empty?
            bool isEmpty() const { return callsigns.isEmpty(); }

            //! Check function
            bool hasSameSizes() const
            {
                const int s = callsigns.size();
                if (s != latitudesDeg.size())  { return false; }
                if (s != longitudesDeg.size()) { return false; }
                if (s != altitudesFt.size())   { return false; }
                if (s != pitchesDeg.size())    { return false; }
                if (s != rollsDeg.size())      { return false; }
                if (s != headingsDeg.size())   { return false; }
                return true;
            }

            QStringList callsigns;         //!< List of callsigns
            QList<double> latitudesDeg;    //!< List of latitudes
            QList<double> longitudesDeg;   //!< List of longitudes
            QList<double> altitudesFt;     //!< List of altitudes
            QList<double> pitchesDeg;      //!< List of pitches
            QList<double> rollsDeg;        //!< List of rolls
            QList<double> headingsDeg;     //!< List of headings
            QList<bool> onGrounds;         //!< List of onGrounds
        };

        //! Planes surfaces
        struct PlanesSurfaces
        {
            //! Is empty?
            bool isEmpty() const { return callsigns.isEmpty(); }

            QStringList callsigns;      //!< List of callsigns
            QList<double> gears;        //!< List of gears
            QList<double> flaps;        //!< List of flaps
            QList<double> spoilers;     //!< List of spoilers
            QList<double> speedBrakes;  //!< List of speedBrakes
            QList<double> slats;        //!< List of slats
            QList<double> wingSweeps;   //!< List of wingSweeps
            QList<double> thrusts;      //!< List of thrusts
            QList<double> elevators;    //!< List of elevators
            QList<double> rudders;      //!< List of rudders
            QList<double> ailerons;     //!< List of ailerons
            QList<bool> landLights;     //!< List of landLights
            QList<bool> beaconLights;   //!< List of beaconLights
            QList<bool> strobeLights;   //!< List of strobeLights
            QList<bool> navLights;      //!< List of navLights
            QList<int> lightPatterns;   //!< List of lightPatterns
        };

        //! Plane Transponders
        struct PlanesTransponders
        {
            //! Is empty?
            bool isEmpty() const { return callsigns.isEmpty(); }

            QStringList callsigns;  //!< List of callsigns
            QList<int> codes;       //!< List of transponder codes
            QList<bool> modeCs;     //!< List of active mode C's
            QList<bool> idents;     //!< List of active idents
        };

        //! Multiplayer Acquire Info
        struct MultiplayerAcquireInfo
        {
            bool hasAcquired;       //!< Has XSwiftBus acquired multiplayer planes?
            QString owner;          //!< Name of the plugin having multiplayer planes acquired
        };

        /*!
         * Proxy object connected to a real XSwiftBus::CTraffic object via DBus
         */
        class CXSwiftBusTrafficProxy : public QObject
        {
            Q_OBJECT

        public:
            //! Elevation callback
            using ElevationCallback = std::function<void (const BlackMisc::Geo::CElevationPlane &, const BlackMisc::Aviation::CCallsign &)>;

            //! Remote aircrafts data callback
            using RemoteAircraftDataCallback = std::function<void (const QStringList &, const QDoubleList &, const QDoubleList &, const QDoubleList &, const QDoubleList &)>;

            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(XSWIFTBUS_TRAFFIC_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(XSWIFTBUS_TRAFFIC_OBJECTPATH);
                return s;
            }

            //! Constructor
            CXSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

            //! Does the remote object exist?
            bool isValid() const { return m_dbusInterface->isValid(); }

        signals:
            //! Simulator frame
            //! \remark from simulator to driver
            void simFrame();

            //! Remote aircraft successfully added
            void remoteAircraftAdded(const QString &callsign);

            //! Remote aircraft adding failed
            void remoteAircraftAddingFailed(const QString &callsign);

        public slots:
            //! \copydoc XSwiftBus::CTraffic::acquireMultiplayerPlanes
            MultiplayerAcquireInfo acquireMultiplayerPlanes();

            //! \copydoc XSwiftBus::CTraffic::initialize
            bool initialize();

            //! \copydoc XSwiftBus::CTraffic::cleanup
            void cleanup();

            //! \copydoc XSwiftBus::CTraffic::loadPlanesPackage
            QString loadPlanesPackage(const QString &path);

            //! \copydoc XSwiftBus::CTraffic::setDefaultIcao
            void setDefaultIcao(const QString &defaultIcao);

            //! \copydoc XSwiftBus::CTraffic::setDrawingLabels
            void setDrawingLabels(bool drawing);

            //! \copydoc XSwiftBus::CTraffic::isDrawingLabels
            bool isDrawingLabels() const;

            //! \copydoc XSwiftBus::CTraffic::setMaxPlanes
            void setMaxPlanes(int planes);

            //! \copydoc XSwiftBus::CTraffic::setMaxDrawDistance
            void setMaxDrawDistance(double nauticalMiles);

            //! \copydoc XSwiftBus::CTraffic::addPlane
            void addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

            //! \copydoc XSwiftBus::CTraffic::removePlane
            void removePlane(const QString &callsign);

            //! \copydoc XSwiftBus::CTraffic::removeAllPlanes
            void removeAllPlanes();

            //! \copydoc XSwiftBus::CTraffic::setPlanesPositions
            void setPlanesPositions(const BlackSimPlugin::XPlane::PlanesPositions &planesPositions);

            //! \copydoc XSwiftBus::CTraffic::setPlanesSurfaces
            void setPlanesSurfaces(const BlackSimPlugin::XPlane::PlanesSurfaces &planesSurfaces);

            //! \copydoc XSwiftBus::CTraffic::setPlanesTransponders
            void setPlanesTransponders(const BlackSimPlugin::XPlane::PlanesTransponders &planesTransponders);

            //! \deprecated XSwiftBus::CTraffic::setInterpolatorMode
            void setInterpolatorMode(const QString &callsign, bool spline);

            //! \copydoc XSwiftBus::CTraffic::getRemoteAircraftData
            void getRemoteAircraftData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter) const;

            //! \copydoc XSwiftBus::CTraffic::getElevationAtPosition
            void getElevationAtPosition(const BlackMisc::Aviation::CCallsign &callsign, double latitudeDeg, double longitudeDeg, double altitudeMeters,
                                        const ElevationCallback &setter) const;

            //! \copydoc XSwiftBus::CTraffic::setFollowedAircraft
            void setFollowedAircraft(const QString &callsign);

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;
        };
    } // ns
} // ns

#endif // guard
