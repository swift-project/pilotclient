/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FLIGHTGEAR_TRAFFIC_PROXY_H
#define BLACKSIMPLUGIN_FLIGHTGEAR_TRAFFIC_PROXY_H

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/geo/elevationplane.h"

#include <QObject>
#include <QString>
#include <QStringList>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

//! \cond PRIVATE
#define FGSWIFTBUS_TRAFFIC_INTERFACENAME "org.swift_project.fgswiftbus.traffic"
#define FGSWIFTBUS_TRAFFIC_OBJECTPATH "/fgswiftbus/traffic"
//! \endcond

namespace BlackSimPlugin
{
    namespace Flightgear
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

            //! Push back the latest situation
            void push_back(const BlackMisc::Aviation::CAircraftSituation &situation)
            {
                this->callsigns.push_back(situation.getCallsign().asString());
                this->latitudesDeg.push_back(situation.latitude().value(BlackMisc::PhysicalQuantities::CAngleUnit::deg()));
                this->longitudesDeg.push_back(situation.longitude().value(BlackMisc::PhysicalQuantities::CAngleUnit::deg()));
                this->altitudesFt.push_back(situation.getAltitude().value(BlackMisc::PhysicalQuantities::CLengthUnit::ft()));
                this->pitchesDeg.push_back(situation.getPitch().value(BlackMisc::PhysicalQuantities::CAngleUnit::deg()));
                this->rollsDeg.push_back(situation.getBank().value(BlackMisc::PhysicalQuantities::CAngleUnit::deg()));
                this->headingsDeg.push_back(situation.getHeading().value(BlackMisc::PhysicalQuantities::CAngleUnit::deg()));
                this->onGrounds.push_back(situation.getOnGround() == BlackMisc::Aviation::CAircraftSituation::OnGround);
                this->groundSpeedKts.push_back(situation.getGroundSpeed().value(BlackMisc::PhysicalQuantities::CSpeedUnit::kts()));
            }

            QStringList   callsigns;       //!< List of callsigns
            QList<double> latitudesDeg;    //!< List of latitudes
            QList<double> longitudesDeg;   //!< List of longitudes
            QList<double> altitudesFt;     //!< List of altitudes
            QList<double> pitchesDeg;      //!< List of pitches
            QList<double> rollsDeg;        //!< List of rolls
            QList<double> headingsDeg;     //!< List of headings
            QList<double> groundSpeedKts; //!<List of groundspeeds
            QList<bool> onGrounds;      //!< List of onGrounds
        };

        //! Planes surfaces
        struct PlanesSurfaces
        {
            //! Is empty?
            bool isEmpty() const { return callsigns.isEmpty(); }

            //! Push back the latest parts
            void push_back(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts)
            {
                this->callsigns.push_back(callsign.asString());
                this->gears.push_back(parts.isFixedGearDown() ? 1 : 0);
                this->flaps.push_back(parts.getFlapsPercent() / 100.0);
                this->spoilers.push_back(parts.isSpoilersOut() ? 1 : 0);
                this->speedBrakes.push_back(parts.isSpoilersOut() ? 1 : 0);
                this->slats.push_back(parts.getFlapsPercent() / 100.0);
                this->wingSweeps.push_back(0.0);
                this->thrusts.push_back(parts.isAnyEngineOn() ? 0 : 0.75);
                this->elevators.push_back(0.0);
                this->rudders.push_back(0.0);
                this->ailerons.push_back(0.0);
                this->landLights.push_back(parts.getLights().isLandingOn());
                this->beaconLights.push_back(parts.getLights().isBeaconOn());
                this->strobeLights.push_back(parts.getLights().isStrobeOn());
                this->navLights.push_back(parts.getLights().isNavOn());
                this->lightPatterns.push_back(0);
                this->taxiLights.push_back(parts.getLights().isTaxiOn());
            }

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
            QList<bool> taxiLights;     //!< List of taxi lights
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
            bool hasAcquired;       //!< Has FGSwiftBus acquired multiplayer planes?
            QString owner;          //!< Name of the plugin having multiplayer planes acquired
        };

        /*!
         * Proxy object connected to a real FGSwiftBus::CTraffic object via DBus
         */
        class CFGSwiftBusTrafficProxy : public QObject
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
                static QString s(FGSWIFTBUS_TRAFFIC_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(FGSWIFTBUS_TRAFFIC_OBJECTPATH);
                return s;
            }

            //! Constructor
            CFGSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

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
            //! Returns whether multiplayer planes have been acquired. If not, owner will be set to the plugin that acquired it.
            MultiplayerAcquireInfo acquireMultiplayerPlanes();

            //! Initialize the multiplayer planes rendering and return true if successful
            bool initialize();

            //! Reverse the actions of initialize().
            void cleanup();

            //! Introduce a new traffic aircraft
            void addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

            //! Remove a traffic aircraft
            void removePlane(const QString &callsign);

            //! Remove all traffic aircraft
            void removeAllPlanes();

            //! Set the position of multiple traffic aircrafts
            void setPlanesPositions(const BlackSimPlugin::Flightgear::PlanesPositions &planesPositions);

            //! Set the surfaces of multiple traffic aircrafts
            void setPlanesSurfaces(const BlackSimPlugin::Flightgear::PlanesSurfaces &planesSurfaces);

            //! Set the transponders of multiple traffic aircrafts
            void setPlanesTransponders(const BlackSimPlugin::Flightgear::PlanesTransponders &planesTransponders);

            //! Get remote aircrafts data (lat, lon, elevation and CG)
            void getRemoteAircraftData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter) const;

            //! \copydoc XSwiftBus::CTraffic::getElevationAtPosition
            void getElevationAtPosition(const BlackMisc::Aviation::CCallsign &callsign, double latitudeDeg, double longitudeDeg, double altitudeMeters,
                                        const ElevationCallback &setter) const;

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;
        };
    } // ns
} // ns

#endif // guard
