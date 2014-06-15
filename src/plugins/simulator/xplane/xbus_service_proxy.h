/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_XBUS_SERVICE_PROXY_H
#define BLACKSIMPLUGIN_XBUS_SERVICE_PROXY_H

//! \file

#include "blackmisc/genericdbusinterface.h"

//! \private
#define XBUS_SERVICE_INTERFACENAME "net.vatsim.PilotClient.XBus"
//! \private
#define XBUS_SERVICE_OBJECTPATH "/XBus"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        /*!
         * Proxy object connected to a real XBus::CService object via DBus
         */
        class CXBusServiceProxy : public QObject
        {
            Q_OBJECT

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(XBUS_SERVICE_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(XBUS_SERVICE_OBJECTPATH);
                return s;
            }

            //! Constructor
            CXBusServiceProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

            //! Does the remote object exist?
            bool isValid() const { return m_dbusInterface->isValid(); }

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;

            void relaySignals();

        signals:
            //! \copydoc XBus::CService::aircraftModelChanged
            void aircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao);

        public slots:
            //! \copydoc XBus::CService::getAircraftModelPath
            QString getAircraftModelPath() const;

            //! \copydoc XBus::CService::getAircraftModelFilename
            QString getAircraftModelFilename() const;

            //! \copydoc XBus::CService::getAircraftModelLivery
            QString getAircraftModelLivery() const;

            //! \copydoc XBus::CService::getAircraftIcaoCode
            QString getAircraftIcaoCode() const;

            //! \copydoc XBus::CService::getXPlaneVersionMajor
            int getXPlaneVersionMajor() const;

            //! \copydoc XBus::CService::getXPlaneVersionMinor
            int getXPlaneVersionMinor() const;

            //! \copydoc XBus::CService::getXPlaneInstallationPath
            QString getXPlaneInstallationPath() const;

            //! \copydoc XBus::CService::getXPlanePreferencesPath
            QString getXPlanePreferencesPath() const;

            //! \copydoc XBus::CService::getLatitude
            double getLatitude() const;

            //! \copydoc XBus::CService::getLongitude
            double getLongitude() const;

            //! \copydoc XBus::CService::getAltitudeMSL
            double getAltitudeMSL() const;

            //! \copydoc XBus::CService::getHeightAGL
            double getHeightAGL() const;

            //! \copydoc XBus::CService::getGroundSpeed
            double getGroundSpeed() const;

            //! \copydoc XBus::CService::getIndicatedAirspeed
            double getIndicatedAirspeed() const;

            //! \copydoc XBus::CService::getTrueAirspeed
            double getTrueAirspeed() const;

            //! \copydoc XBus::CService::getPitch
            double getPitch() const;

            //! \copydoc XBus::CService::getRoll
            double getRoll() const;

            //! \copydoc XBus::CService::getTrueHeading
            double getTrueHeading() const;
        };

    }
}

#endif // guard
