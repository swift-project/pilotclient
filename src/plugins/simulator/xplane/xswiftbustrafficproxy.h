// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_XSWIFTBUS_TRAFFIC_PROXY_H
#define SWIFT_SIMPLUGIN_XSWIFTBUS_TRAFFIC_PROXY_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/callsign.h"
#include "misc/genericdbusinterface.h"
#include "misc/geo/elevationplane.h"
#include "misc/logcategories.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

//! \cond PRIVATE
#define XSWIFTBUS_TRAFFIC_INTERFACENAME "org.swift_project.xswiftbus.traffic"
#define XSWIFTBUS_TRAFFIC_OBJECTPATH "/xswiftbus/traffic"
//! \endcond

namespace swift::simplugin::xplane
{
    //! List of doubles
    using QDoubleList = QList<double>;

    //! List of bools
    using QBoolList = QList<bool>;

    //! Planes positions
    struct PlanesPositions
    {
        //! Is empty?
        bool isEmpty() const { return callsigns.isEmpty(); }

        //! Check function
        bool hasSameSizes() const
        {
            const int s = callsigns.size();
            if (s != latitudesDeg.size()) { return false; }
            if (s != longitudesDeg.size()) { return false; }
            if (s != altitudesFt.size()) { return false; }
            if (s != pitchesDeg.size()) { return false; }
            if (s != rollsDeg.size()) { return false; }
            if (s != headingsDeg.size()) { return false; }
            return true;
        }

        //! Push back the latest situation
        void push_back(const swift::misc::aviation::CAircraftSituation &situation)
        {
            this->callsigns.push_back(situation.getCallsign().asString());
            this->latitudesDeg.push_back(
                situation.latitude().value(swift::misc::physical_quantities::CAngleUnit::deg()));
            this->longitudesDeg.push_back(
                situation.longitude().value(swift::misc::physical_quantities::CAngleUnit::deg()));
            this->altitudesFt.push_back(
                situation.getAltitude().value(swift::misc::physical_quantities::CLengthUnit::ft()));
            this->pitchesDeg.push_back(situation.getPitch().value(swift::misc::physical_quantities::CAngleUnit::deg()));
            this->rollsDeg.push_back(situation.getBank().value(swift::misc::physical_quantities::CAngleUnit::deg()));
            this->headingsDeg.push_back(
                situation.getHeading().value(swift::misc::physical_quantities::CAngleUnit::deg()));
            this->onGrounds.push_back(situation.isOnGround());
        }

        QStringList callsigns; //!< List of callsigns
        QList<double> latitudesDeg; //!< List of latitudes
        QList<double> longitudesDeg; //!< List of longitudes
        QList<double> altitudesFt; //!< List of altitudes
        QList<double> pitchesDeg; //!< List of pitches
        QList<double> rollsDeg; //!< List of rolls
        QList<double> headingsDeg; //!< List of headings
        QList<bool> onGrounds; //!< List of onGrounds
    };

    //! Planes surfaces
    struct PlanesSurfaces
    {
        //! Is empty?
        bool isEmpty() const { return callsigns.isEmpty(); }

        //! Push back the latest parts
        void push_back(const swift::misc::aviation::CCallsign &callsign,
                       const swift::misc::aviation::CAircraftParts &parts)
        {
            this->callsigns.push_back(callsign.asString());
            this->gears.push_back(parts.isFixedGearDown() ? 1 : 0);
            this->flaps.push_back(parts.getFlapsPercent() / 100.0);
            this->spoilers.push_back(parts.isSpoilersOut() ? 1 : 0);
            this->speedBrakes.push_back(parts.isSpoilersOut() ? 1 : 0);
            this->slats.push_back(parts.getFlapsPercent() / 100.0);
            this->wingSweeps.push_back(0.0);
            this->thrusts.push_back(parts.isAnyEngineOn() ? 0.75 : 0);
            this->elevators.push_back(0.0);
            this->rudders.push_back(0.0);
            this->ailerons.push_back(0.0);
            this->landLights.push_back(parts.getLights().isLandingOn());
            this->taxiLights.push_back(parts.getLights().isTaxiOn());
            this->beaconLights.push_back(parts.getLights().isBeaconOn());
            this->strobeLights.push_back(parts.getLights().isStrobeOn());
            this->navLights.push_back(parts.getLights().isNavOn());
            this->lightPatterns.push_back(0);
        }

        QStringList callsigns; //!< List of callsigns
        QList<double> gears; //!< List of gears
        QList<double> flaps; //!< List of flaps
        QList<double> spoilers; //!< List of spoilers
        QList<double> speedBrakes; //!< List of speed brakes
        QList<double> slats; //!< List of slats
        QList<double> wingSweeps; //!< List of wing sweeps
        QList<double> thrusts; //!< List of thrusts
        QList<double> elevators; //!< List of elevators
        QList<double> rudders; //!< List of rudders
        QList<double> ailerons; //!< List of ailerons
        QList<bool> landLights; //!< List of landing lights
        QList<bool> taxiLights; //!< List of taxi lights
        QList<bool> beaconLights; //!< List of beacon lights
        QList<bool> strobeLights; //!< List of strobe lights
        QList<bool> navLights; //!< List of nav lights
        QList<int> lightPatterns; //!< List of light patterns
    };

    //! Plane Transponders
    struct PlanesTransponders
    {
        //! Is empty?
        bool isEmpty() const { return callsigns.isEmpty(); }

        QStringList callsigns; //!< List of callsigns
        QList<int> codes; //!< List of transponder codes
        QList<bool> modeCs; //!< List of active mode C's
        QList<bool> idents; //!< List of active idents
    };

    //! Multiplayer Acquire Info
    struct MultiplayerAcquireInfo
    {
        bool hasAcquired; //!< Has xswiftbus acquired multiplayer planes?
        QString owner; //!< Name of the plugin having multiplayer planes acquired
    };

    /*!
     * Proxy object connected to a real XSwiftBus::CTraffic object via DBus
     */
    class CXSwiftBusTrafficProxy : public QObject
    {
        Q_OBJECT

    public:
        //! Elevation callback
        using ElevationCallback = std::function<void(const swift::misc::geo::CElevationPlane &,
                                                     const swift::misc::aviation::CCallsign &, bool)>;

        //! Remote aircrafts data callback
        using RemoteAircraftDataCallback =
            std::function<void(const QStringList &, const QDoubleList &, const QDoubleList &, const QDoubleList &,
                               const QBoolList &, const QDoubleList &)>;

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

        //! Log. categories
        static const QStringList &getLogCategories();

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

        //! \copydoc XSwiftBus::CTraffic::setMaxPlanes
        void setMaxPlanes(int planes);

        //! \copydoc XSwiftBus::CTraffic::setMaxDrawDistance
        void setMaxDrawDistance(double nauticalMiles);

        //! \copydoc XSwiftBus::CTraffic::addPlane
        void addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao,
                      const QString &airlineIcao, const QString &livery);

        //! \copydoc XSwiftBus::CTraffic::removePlane
        void removePlane(const QString &callsign);

        //! \copydoc XSwiftBus::CTraffic::removeAllPlanes
        void removeAllPlanes();

        //! \copydoc XSwiftBus::CTraffic::setPlanesPositions
        void setPlanesPositions(const swift::simplugin::xplane::PlanesPositions &planesPositions);

        //! \copydoc XSwiftBus::CTraffic::setPlanesSurfaces
        void setPlanesSurfaces(const swift::simplugin::xplane::PlanesSurfaces &planesSurfaces);

        //! \copydoc XSwiftBus::CTraffic::setPlanesTransponders
        void setPlanesTransponders(const swift::simplugin::xplane::PlanesTransponders &planesTransponders);

        //! \deprecated XSwiftBus::CTraffic::setInterpolatorMode
        void setInterpolatorMode(const QString &callsign, bool spline);

        //! \copydoc XSwiftBus::CTraffic::getRemoteAircraftData
        void getRemoteAircraftData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter) const;

        //! \copydoc XSwiftBus::CTraffic::getElevationAtPosition
        void getElevationAtPosition(const swift::misc::aviation::CCallsign &callsign, double latitudeDeg,
                                    double longitudeDeg, double altitudeMeters, const ElevationCallback &setter) const;

        //! \copydoc XSwiftBus::CTraffic::setFollowedAircraft
        void setFollowedAircraft(const QString &callsign);

    private:
        swift::misc::CGenericDBusInterface *m_dbusInterface = nullptr;
    };
} // namespace swift::simplugin::xplane

#endif // guard
