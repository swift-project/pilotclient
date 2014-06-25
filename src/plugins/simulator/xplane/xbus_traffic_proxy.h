/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_XBUS_TRAFFIC_PROXY_H
#define BLACKSIMPLUGIN_XBUS_TRAFFIC_PROXY_H

//! \file

#include "blackmisc/genericdbusinterface.h"

//! \cond PRIVATE
#define XBUS_TRAFFIC_INTERFACENAME "net.vatsim.xbus.traffic"
#define XBUS_TRAFFIC_OBJECTPATH "/xbus/traffic"
//! \endcond

namespace BlackSimPlugin
{
    namespace XPlane
    {

        /*!
         * Proxy object connected to a real XBus::CTraffic object via DBus
         */
        class CXBusTrafficProxy : public QObject
        {
            Q_OBJECT

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(XBUS_TRAFFIC_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(XBUS_TRAFFIC_OBJECTPATH);
                return s;
            }

            //! Constructor
            CXBusTrafficProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

            //! Does the remote object exist?
            bool isValid() const { return m_dbusInterface->isValid(); }

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;

            void relaySignals();

        public slots:
            //! \copydoc XBus::CTraffic::initialize
            bool initialize();

            //! \copydoc XBus::CTraffic::cleanup
            void cleanup();

            //! \copydoc XBus::CTraffic::loadPlanesPackage
            bool loadPlanesPackage(const QString &path);

            //! \copydoc XBus::CTraffic::setDefaultIcao
            void setDefaultIcao(const QString &defaultIcao);

            //! \copydoc XBus::CTraffic::setDrawingLabels
            void setDrawingLabels(bool drawing);

            //! \copydoc XBus::CTraffic::isDrawingLabels
            bool isDrawingLabels() const;

            //! \copydoc XBus::CTraffic::addPlane
            void addPlane(const QString &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

            //! \copydoc XBus::CTraffic::removePlane
            void removePlane(const QString &callsign);

            //! \copydoc XBus::CTraffic::removeAllPlanes
            void removeAllPlanes();

            //! \copydoc XBus::CTraffic::setPlanePosition
            void setPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading);

            //! \copydoc XBus::CTraffic::setPlaneSurfaces
            void setPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
                double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern);

            //! \copydoc XBus::CTraffic::setPlaneTransponder
            void setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident);
        };

    }
}

#endif // guard
