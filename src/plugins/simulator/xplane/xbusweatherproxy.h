/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_XBUS_WEATHER_PROXY_H
#define BLACKSIMPLUGIN_XBUS_WEATHER_PROXY_H

//! \file

#include "blackmisc/genericdbusinterface.h"

//! \cond PRIVATE
#define XBUS_WEATHER_INTERFACENAME "org.swift_project.xbus.weather"
#define XBUS_WEATHER_OBJECTPATH "/xbus/weather"
//! \endcond

namespace BlackSimPlugin
{
    namespace XPlane
    {

        /*!
         * Proxy object connected to a real XBus::CWeather object via DBus
         */
        class CXBusWeatherProxy : public QObject
        {
            Q_OBJECT

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(XBUS_WEATHER_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(XBUS_WEATHER_OBJECTPATH);
                return s;
            }

            //! Constructor
            CXBusWeatherProxy(QDBusConnection &connection, QObject *parent = nullptr);

            //! Does the remote object exist?
            bool isValid() const { return m_dbusInterface->isValid(); }

        private:
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;

        public slots:
            //! \copydoc XBus::CWeather::isUsingRealWeather
            bool isUsingRealWeather() const;

            //! \copydoc XBus::CWeather::setUseRealWeather
            void setUseRealWeather(bool enable);

            //! \copydoc XBus::CWeather::setVisibility
            void setVisibility(float visibilityM);

            //! \copydoc XBus::CWeather::setTemperature
            void setTemperature(int degreesC);

            //! \copydoc XBus::CWeather::setDewPoint
            void setDewPoint(int degreesC);

            //! \copydoc XBus::CWeather::setQNH
            void setQNH(float inHg);

            //! \copydoc XBus::CWeather::setPrecipitationRatio
            void setPrecipitationRatio(float precipRatio);

            //! \copydoc XBus::CWeather::setThunderstormRatio
            void setThunderstormRatio(float cbRatio);

            //! \copydoc XBus::CWeather::setTurbulenceRatio
            void setTurbulenceRatio(float turbulenceRatio);

            //! \copydoc XBus::CWeather::setRunwayFriction
            void setRunwayFriction(int friction);

            //! \copydoc XBus::CWeather::setCloudLayer
            void setCloudLayer(int layer, int base, int tops, int type, int coverage);

            //! \copydoc XBus::CWeather::setWindLayer
            void setWindLayer(int layer, int altitude, float direction, int speed, int shearDirection, int shearSpeed, int turbulence);
        };

    }
}

#endif // guard
