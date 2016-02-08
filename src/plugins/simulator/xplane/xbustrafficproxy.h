/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_XBUS_TRAFFIC_PROXY_H
#define BLACKSIMPLUGIN_XBUS_TRAFFIC_PROXY_H

//! \file

#include "blackmisc/genericdbusinterface.h"

//! \cond PRIVATE
#define XBUS_TRAFFIC_INTERFACENAME "org.swift_project.xbus.traffic"
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

        signals:
            //! \copydoc XBus::CTraffic::installedModelsUpdated
            void installedModelsUpdated(const QStringList &modelStrings, const QStringList &icaos, const QStringList &airlines, const QStringList &liveries);

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

            //! \copydoc XBus::CTraffic::updateInstalledModels
            void updateInstalledModels() const;

            //! \copydoc XBus::CTraffic::setMaxPlanes
            void setMaxPlanes(int planes);

            //! \copydoc XBus::CTraffic::setMaxDrawDistance
            void setMaxDrawDistance(float nauticalMiles);

            //! \copydoc XBus::CTraffic::addPlane
            void addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

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
