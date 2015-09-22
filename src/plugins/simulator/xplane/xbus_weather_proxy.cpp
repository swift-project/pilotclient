/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xbus_weather_proxy.h"
#include "blackcore/dbus_server.h"

#define XBUS_SERVICENAME "org.swift-project.xbus"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CXBusWeatherProxy::CXBusWeatherProxy(QDBusConnection &connection, QObject *parent) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
        }

        bool CXBusWeatherProxy::isUsingRealWeather() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isUsingRealWeather"));
        }

        void CXBusWeatherProxy::setUseRealWeather(bool enable)
        {
            m_dbusInterface->callDBus(QLatin1String("setUseRealWeather"), enable);
        }

        void CXBusWeatherProxy::setVisibility(float visibilityM)
        {
            m_dbusInterface->callDBus(QLatin1String("setVisibility"), visibilityM);
        }

        void CXBusWeatherProxy::setTemperature(int degreesC)
        {
            m_dbusInterface->callDBus(QLatin1String("setTemperature"), degreesC);
        }

        void CXBusWeatherProxy::setDewPoint(int degreesC)
        {
            m_dbusInterface->callDBus(QLatin1String("setDewPoint"), degreesC);
        }

        void CXBusWeatherProxy::setQNH(float inHg)
        {
            m_dbusInterface->callDBus(QLatin1String("setQNH"), inHg);
        }

        void CXBusWeatherProxy::setPrecipitationRatio(float precipRatio)
        {
            m_dbusInterface->callDBus(QLatin1String("setPrecipitationRatio"), precipRatio);
        }

        void CXBusWeatherProxy::setThunderstormRatio(float cbRatio)
        {
            m_dbusInterface->callDBus(QLatin1String("setThunderstormRatio"), cbRatio);
        }

        void CXBusWeatherProxy::setTurbulenceRatio(float turbulenceRatio)
        {
            m_dbusInterface->callDBus(QLatin1String("setTurbulenceRatio"), turbulenceRatio);
        }

        void CXBusWeatherProxy::setRunwayFriction(int friction)
        {
            m_dbusInterface->callDBus(QLatin1String("setRunwayFriction"), friction);
        }

        void CXBusWeatherProxy::setCloudLayer(int layer, int base, int tops, int type, int coverage)
        {
            m_dbusInterface->callDBus(QLatin1String("setCloudLayer"), layer, base, tops, type, coverage);
        }

        void CXBusWeatherProxy::setWindLayer(int layer, int altitude, float direction, int speed, int shearDirection, int shearSpeed, int turbulence)
        {
            m_dbusInterface->callDBus(QLatin1String("setWindLayer"), layer, altitude, direction, speed, shearDirection, shearSpeed, turbulence);
        }

    }
}
