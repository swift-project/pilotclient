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
            //! \copydoc XSwiftBus::CTraffic::simFrame
            //! \remark from simulator to driver
            void simFrame();

            //! \copydoc XSwiftBus::CTraffic::remoteAircraftData
            //! \remark from simulator to driver for elevation and CG
            void remoteAircraftData(const QString &callsign, double latitude, double longitude, double elevation, double modelVerticalOffset);

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

            //! \copydoc XSwiftBus::CTraffic::addPlanePosition
            void addPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading, qint64 relativeTime, qint64 timeOffset);

            //! \copydoc XSwiftBus::CTraffic::setPlanePosition
            void setPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading);

            //! \copydoc XSwiftBus::CTraffic::addPlaneSurfaces
            void addPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
                                  double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround, qint64 relativeTime, qint64 timeOffset);

            //! \copydoc XSwiftBus::CTraffic::setPlaneSurfaces
            void setPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
                                  double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround);

            //! \copydoc XSwiftBus::CTraffic::setPlaneTransponder
            void setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident);

            //! \copydoc XSwiftBus::CTraffic::setInterpolatorMode
            void setInterpolatorMode(const QString &callsign, bool spline);

            //! \copydoc XSwiftBus::CTraffic::requestRemoteAircraftData
            void requestRemoteAircraftData();

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;
        };
    } // ns
} // ns

#endif // guard
