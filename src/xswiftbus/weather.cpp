// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "weather.h"
#include "utils.h"

namespace XSwiftBus
{
    CWeather::CWeather(CSettingsProvider *settingsProvider) : CDBusObject(settingsProvider)
    {
        // void
    }

    //! Set cloud layer
    template <class T>
    void setCloudLayerImpl(T &layer, int base, int tops, int type, int coverage)
    {
        layer.base.setAsInt(base);
        layer.tops.setAsInt(tops);
        layer.type.set(type);
        layer.coverage.setAsInt(coverage);
    }

    void CWeather::setCloudLayer(int layer, int baseM, int topsM, int type, int coverage)
    {
        switch (layer)
        {
        case 0: setCloudLayerImpl(m_cloudLayer0, baseM, topsM, type, coverage); break;
        case 1: setCloudLayerImpl(m_cloudLayer1, baseM, topsM, type, coverage); break;
        case 2: setCloudLayerImpl(m_cloudLayer2, baseM, topsM, type, coverage); break;
        default: DEBUG_LOG("Invalid cloud layer"); break;
        }
    }

    //! Set wind layer
    template <class T>
    void setWindLayerImpl(T &layer, int altitude, double direction, int speed, int shearDirection, int shearSpeed, int turbulence)
    {
        layer.altitude.setAsInt(altitude);
        layer.direction.set(static_cast<float>(direction));
        layer.speed.setAsInt(speed);
        layer.shearDirection.setAsInt(shearDirection);
        layer.shearSpeed.setAsInt(shearSpeed);
        layer.turbulence.setAsInt(turbulence);
    }

    void CWeather::setWindLayer(int layer, int altitudeM, double directionDeg, int speedKt, int shearDirectionDeg, int shearSpeedKt, int turbulence)
    {
        switch (layer)
        {
        case 0: setWindLayerImpl(m_windLayer0, altitudeM, directionDeg, speedKt, shearDirectionDeg, shearSpeedKt, turbulence); break;
        case 1: setWindLayerImpl(m_windLayer1, altitudeM, directionDeg, speedKt, shearDirectionDeg, shearSpeedKt, turbulence); break;
        case 2: setWindLayerImpl(m_windLayer2, altitudeM, directionDeg, speedKt, shearDirectionDeg, shearSpeedKt, turbulence); break;
        default: DEBUG_LOG("Invalid wind layer"); break;
        }
    }

    static const char *introspection_weather =
        DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
#include "org.swift_project.xswiftbus.weather.xml"
        ;

    DBusHandlerResult CWeather::dbusMessageHandler(const CDBusMessage &message_)
    {
        CDBusMessage message(message_);
        const std::string sender = message.getSender();
        const dbus_uint32_t serial = message.getSerial();
        const bool wantsReply = message.wantsReply();

        if (message.getInterfaceName() == DBUS_INTERFACE_INTROSPECTABLE)
        {
            if (message.getMethodName() == "Introspect")
            {
                sendDBusReply(sender, serial, introspection_weather);
            }
        }
        else if (message.getInterfaceName() == XSWIFTBUS_WEATHER_INTERFACENAME)
        {
            if (message.getMethodName() == "isUsingRealWeather")
            {
                queueDBusCall([=]() {
                    sendDBusReply(sender, serial, isUsingRealWeather());
                });
            }
            else if (message.getMethodName() == "setUseRealWeather")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                bool enable = false;
                message.beginArgumentRead();
                message.getArgument(enable);
                queueDBusCall([=]() {
                    setUseRealWeather(enable);
                });
            }
            else if (message.getMethodName() == "setVisibility")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double visibilityM = 10.0;
                message.beginArgumentRead();
                message.getArgument(visibilityM);
                queueDBusCall([=]() {
                    setVisibility(visibilityM);
                });
            }
            else if (message.getMethodName() == "setTemperature")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int degreesC = 10;
                message.beginArgumentRead();
                message.getArgument(degreesC);
                queueDBusCall([=]() {
                    setTemperature(degreesC);
                });
            }
            else if (message.getMethodName() == "setDewPoint")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int degreesC = 10;
                message.beginArgumentRead();
                message.getArgument(degreesC);
                queueDBusCall([=]() {
                    setDewPoint(degreesC);
                });
            }
            else if (message.getMethodName() == "setQNH")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double inHg = 29.92;
                message.beginArgumentRead();
                message.getArgument(inHg);
                queueDBusCall([=]() {
                    setQNH(inHg);
                });
            }
            else if (message.getMethodName() == "setPrecipitationRatio")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double precipRatio = 0.0;
                message.beginArgumentRead();
                message.getArgument(precipRatio);
                queueDBusCall([=]() {
                    setPrecipitationRatio(precipRatio);
                });
            }
            else if (message.getMethodName() == "setThunderstormRatio")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double cbRatio = 0.0;
                message.beginArgumentRead();
                message.getArgument(cbRatio);
                queueDBusCall([=]() {
                    setThunderstormRatio(cbRatio);
                });
            }
            else if (message.getMethodName() == "setTurbulenceRatio")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double turbulenceRatio = 0.0;
                message.beginArgumentRead();
                message.getArgument(turbulenceRatio);
                queueDBusCall([=]() {
                    setTurbulenceRatio(turbulenceRatio);
                });
            }
            else if (message.getMethodName() == "setRunwayFriction")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int friction = 10;
                message.beginArgumentRead();
                message.getArgument(friction);
                queueDBusCall([=]() {
                    setRunwayFriction(friction);
                });
            }
            else if (message.getMethodName() == "setCloudLayer")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int layer = 0;
                int base = 0;
                int tops = 0;
                int type = 0;
                int coverage = 0;
                message.beginArgumentRead();
                message.getArgument(layer);
                message.getArgument(base);
                message.getArgument(tops);
                message.getArgument(type);
                message.getArgument(coverage);
                queueDBusCall([=]() {
                    setCloudLayer(layer, base, tops, type, coverage);
                });
            }
            else if (message.getMethodName() == "setWindLayer")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int layer = 0;
                int altitudeM = 0;
                double directionDeg = 0;
                int speedKt = 0;
                int shearDirectionDeg = 0;
                int shearSpeedKt = 0;
                int turbulence = 0;
                message.beginArgumentRead();
                message.getArgument(layer);
                message.getArgument(altitudeM);
                message.getArgument(directionDeg);
                message.getArgument(speedKt);
                message.getArgument(shearDirectionDeg);
                message.getArgument(shearSpeedKt);
                message.getArgument(turbulence);
                queueDBusCall([=]() {
                    setWindLayer(layer, altitudeM, directionDeg, speedKt, shearDirectionDeg, shearSpeedKt, turbulence);
                });
            }
            else
            {
                // Unknown message. Tell DBus that we cannot handle it
                return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    int CWeather::process()
    {
        invokeQueuedDBusCalls();
        return 1;
    }
}

//! \endcond
