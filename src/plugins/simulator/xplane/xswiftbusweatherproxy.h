/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_XSWIFTBUS_WEATHER_PROXY_H
#define BLACKSIMPLUGIN_XSWIFTBUS_WEATHER_PROXY_H

#include "blackmisc/genericdbusinterface.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

//! \cond PRIVATE
#define XSWIFTBUS_WEATHER_INTERFACENAME "org.swift_project.xswiftbus.weather"
#define XSWIFTBUS_WEATHER_OBJECTPATH "/xswiftbus/weather"
//! \endcond

namespace BlackSimPlugin::XPlane
{

    /*!
     * Proxy object connected to a real XSwiftBus::CWeather object via DBus
     */
    class CXSwiftBusWeatherProxy : public QObject
    {
        Q_OBJECT

    public:
        //! Service name
        static const QString &InterfaceName()
        {
            static QString s(XSWIFTBUS_WEATHER_INTERFACENAME);
            return s;
        }

        //! Service path
        static const QString &ObjectPath()
        {
            static QString s(XSWIFTBUS_WEATHER_OBJECTPATH);
            return s;
        }

        //! Constructor
        CXSwiftBusWeatherProxy(QDBusConnection &connection, QObject *parent = nullptr);

        //! Does the remote object exist?
        bool isValid() const { return m_dbusInterface->isValid(); }

    private:
        BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;

    public slots:
        //! \copydoc XSwiftBus::CWeather::isUsingRealWeather
        bool isUsingRealWeather() const;

        //! \copydoc XSwiftBus::CWeather::setUseRealWeather
        void setUseRealWeather(bool enable);

        //! \copydoc XSwiftBus::CWeather::setVisibility
        void setVisibility(double visibilityM);

        //! \copydoc XSwiftBus::CWeather::setTemperature
        void setTemperature(int degreesC);

        //! \copydoc XSwiftBus::CWeather::setDewPoint
        void setDewPoint(int degreesC);

        //! \copydoc XSwiftBus::CWeather::setQNH
        void setQNH(double inHg);

        //! \copydoc XSwiftBus::CWeather::setPrecipitationRatio
        void setPrecipitationRatio(double precipRatio);

        //! \copydoc XSwiftBus::CWeather::setThunderstormRatio
        void setThunderstormRatio(double cbRatio);

        //! \copydoc XSwiftBus::CWeather::setTurbulenceRatio
        void setTurbulenceRatio(double turbulenceRatio);

        //! \copydoc XSwiftBus::CWeather::setRunwayFriction
        void setRunwayFriction(int friction);

        //! \copydoc XSwiftBus::CWeather::setCloudLayer
        void setCloudLayer(int layer, int baseM, int topsM, int type, int coverage);

        //! \copydoc XSwiftBus::CWeather::setWindLayer
        void setWindLayer(int layer, int altitudeM, double directionDeg, int speedKt, int shearDirectionDeg, int shearSpeedKt, int turbulence);
    };

}

#endif // guard
