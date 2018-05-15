/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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

class QDBusConnection;

//! \cond PRIVATE
#define XSWIFTBUS_TRAFFIC_INTERFACENAME "org.swift_project.xswiftbus.traffic"
#define XSWIFTBUS_TRAFFIC_OBJECTPATH "/xswiftbus/traffic"
//! \endcond

namespace BlackSimPlugin
{
    namespace XPlane
    {
        /*!
         * Proxy object connected to a real XSwiftBus::CTraffic object via DBus
         */
        class CXSwiftBusTrafficProxy : public QObject
        {
            Q_OBJECT

        public:
            //! List of doubles
            using QDoubleList = QList<double>;

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
            //! \copydoc XSwiftBus::CTraffic::initialize
            bool initialize();

            //! \copydoc XSwiftBus::CTraffic::cleanup
            void cleanup();

            //! \copydoc XSwiftBus::CTraffic::loadPlanesPackage
            bool loadPlanesPackage(const QString &path);

            //! \copydoc XSwiftBus::CTraffic::setDefaultIcao
            void setDefaultIcao(const QString &defaultIcao);

            //! \copydoc XSwiftBus::CTraffic::setDrawingLabels
            void setDrawingLabels(bool drawing);

            //! \copydoc XSwiftBus::CTraffic::isDrawingLabels
            bool isDrawingLabels() const;

            //! \copydoc XSwiftBus::CTraffic::setMaxPlanes
            void setMaxPlanes(int planes);

            //! \copydoc XSwiftBus::CTraffic::setMaxDrawDistance
            void setMaxDrawDistance(float nauticalMiles);

            //! \copydoc XSwiftBus::CTraffic::addPlane
            void addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

            //! \copydoc XSwiftBus::CTraffic::removePlane
            void removePlane(const QString &callsign);

            //! \copydoc XSwiftBus::CTraffic::removeAllPlanes
            void removeAllPlanes();

            //! \copydoc XSwiftBus::CTraffic::setPlanePositions
            void setPlanePositions(const QStringList &callsigns, const QList<double> &latitudes, const QList<double> &longitudes, const QList<double> &altitudes,
                                   const QList<double> &pitches, const QList<double> &rolles, const QList<double> &headings);

            //! \copydoc XSwiftBus::CTraffic::setPlaneSurfaces
            void setPlaneSurfaces(const QStringList &callsign, const QList<double> &gear, const QList<double> &flap, const QList<double> &spoiler,
                                  const QList<double> &speedBrake, const QList<double> &slat, const QList<double> &wingSweep, const QList<double> &thrust,
                                  const QList<double> &elevator, const QList<double> &rudder, const QList<double> &aileron, const QList<bool> &landLight,
                                  const QList<bool> &beaconLight, const QList<bool> &strobeLight, const QList<bool> &navLight, const QList<int> &lightPattern, const QList<bool> &onGround);

            //! \copydoc XSwiftBus::CTraffic::setPlaneTransponder
            void setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident);

            //! \deprecated XSwiftBus::CTraffic::setInterpolatorMode
            void setInterpolatorMode(const QString &callsign, bool spline);

            //! \copydoc XSwiftBus::CTraffic::getRemoteAircraftsData
            void getRemoteAircraftsData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter);

            //! \copydoc XSwiftBus::CTraffic::getEelevationAtPosition
            void getEelevationAtPosition(const BlackMisc::Aviation::CCallsign &callsign, double latitude, double longitude, double altitude,
                                         const ElevationCallback &setter);

            //! \copydoc XSwiftBus::CTraffic::setFollowedAircraft
            void setFollowedAircraft(const QString &callsign);

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;
        };
    } // ns
} // ns

#endif // guard
