/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "xswiftbusweatherproxy.h"

#include <QLatin1String>

class QDBusConnection;

#define XSWIFTBUS_SERVICENAME "org.swift-project.xswiftbus"

namespace BlackSimPlugin::XPlane
{

    CXSwiftBusWeatherProxy::CXSwiftBusWeatherProxy(QDBusConnection &connection, QObject *parent) : QObject(parent)
    {
        m_dbusInterface = new BlackMisc::CGenericDBusInterface(XSWIFTBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
    }

    bool CXSwiftBusWeatherProxy::isUsingRealWeather() const
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("isUsingRealWeather"));
    }

    void CXSwiftBusWeatherProxy::setUseRealWeather(bool enable)
    {
        m_dbusInterface->callDBus(QLatin1String("setUseRealWeather"), enable);
    }

    void CXSwiftBusWeatherProxy::setVisibility(double visibilityM)
    {
        m_dbusInterface->callDBus(QLatin1String("setVisibility"), visibilityM);
    }

    void CXSwiftBusWeatherProxy::setTemperature(int degreesC)
    {
        m_dbusInterface->callDBus(QLatin1String("setTemperature"), degreesC);
    }

    void CXSwiftBusWeatherProxy::setDewPoint(int degreesC)
    {
        m_dbusInterface->callDBus(QLatin1String("setDewPoint"), degreesC);
    }

    void CXSwiftBusWeatherProxy::setQNH(double inHg)
    {
        m_dbusInterface->callDBus(QLatin1String("setQNH"), inHg);
    }

    void CXSwiftBusWeatherProxy::setPrecipitationRatio(double precipRatio)
    {
        m_dbusInterface->callDBus(QLatin1String("setPrecipitationRatio"), precipRatio);
    }

    void CXSwiftBusWeatherProxy::setThunderstormRatio(double cbRatio)
    {
        m_dbusInterface->callDBus(QLatin1String("setThunderstormRatio"), cbRatio);
    }

    void CXSwiftBusWeatherProxy::setTurbulenceRatio(double turbulenceRatio)
    {
        m_dbusInterface->callDBus(QLatin1String("setTurbulenceRatio"), turbulenceRatio);
    }

    void CXSwiftBusWeatherProxy::setRunwayFriction(int friction)
    {
        m_dbusInterface->callDBus(QLatin1String("setRunwayFriction"), friction);
    }

    void CXSwiftBusWeatherProxy::setCloudLayer(int layer, int baseM, int topsM, int type, int coverage)
    {
        m_dbusInterface->callDBus(QLatin1String("setCloudLayer"), layer, baseM, topsM, type, coverage);
    }

    void CXSwiftBusWeatherProxy::setWindLayer(int layer, int altitudeM, double directionDeg, int speedKt, int shearDirectionDeg, int shearSpeedKt, int turbulence)
    {
        m_dbusInterface->callDBus(QLatin1String("setWindLayer"), layer, altitudeM, directionDeg, speedKt, shearDirectionDeg, shearSpeedKt, turbulence);
    }

}
