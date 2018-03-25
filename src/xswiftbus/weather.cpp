/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "weather.h"
#include "utils.h"

namespace XSwiftBus
{
    CWeather::CWeather(CDBusConnection *dbusConnection)
        : CDBusObject(dbusConnection)
    {
        registerDBusObjectPath(InterfaceName(), ObjectPath());
    }

    //! Set cloud layer
    template <class T>
    void setCloudLayerImpl(T &layer, int base, int tops, int type, int coverage)
    {
        layer.base.set(base);
        layer.tops.set(tops);
        layer.type.set(type);
        layer.coverage.set(coverage);
    }

    void CWeather::setCloudLayer(int layer, int base, int tops, int type, int coverage)
    {
        switch (layer)
        {
        case 0: setCloudLayerImpl(m_cloudLayer0, base, tops, type, coverage); break;
        case 1: setCloudLayerImpl(m_cloudLayer1, base, tops, type, coverage); break;
        case 2: setCloudLayerImpl(m_cloudLayer2, base, tops, type, coverage); break;
        default: DEBUG_LOG("Invalid cloud layer"); break;
        }
    }

    //! Set wind layer
    template <class T>
    void setWindLayerImpl(T &layer, int altitude, double direction, int speed, int shearDirection, int shearSpeed, int turbulence)
    {
        layer.altitude.set(altitude);
        layer.direction.set(static_cast<float>(direction));
        layer.speed.set(speed);
        layer.shearDirection.set(shearDirection);
        layer.shearSpeed.set(shearSpeed);
        layer.turbulence.set(turbulence);
    }

    void CWeather::setWindLayer(int layer, int altitude, double direction, int speed, int shearDirection, int shearSpeed, int turbulence)
    {
        switch (layer)
        {
        case 0: setWindLayerImpl(m_windLayer0, altitude, direction, speed, shearDirection, shearSpeed, turbulence); break;
        case 1: setWindLayerImpl(m_windLayer1, altitude, direction, speed, shearDirection, shearSpeed, turbulence); break;
        case 2: setWindLayerImpl(m_windLayer2, altitude, direction, speed, shearDirection, shearSpeed, turbulence); break;
        default: DEBUG_LOG("Invalid wind layer"); break;
        }
    }

    const char *introspection_weather =
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
                queueDBusCall([=]()
                {
                    sendDBusReply(sender, serial, isUsingRealWeather());
                });
            }
            else if (message.getMethodName() == "setUseRealWeather")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                bool enable = false;
                message.beginArgumentRead();
                message.getArgument(enable);
                queueDBusCall([=]()
                {
                    setUseRealWeather(enable);
                });
            }
            else if (message.getMethodName() == "setVisibility")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double visibilityM = 10.0;
                message.beginArgumentRead();
                message.getArgument(visibilityM);
                queueDBusCall([=]()
                {
                    setVisibility(visibilityM);
                });
            }
            else if (message.getMethodName() == "setTemperature")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int degreesC = 10;
                message.beginArgumentRead();
                message.getArgument(degreesC);
                queueDBusCall([=]()
                {
                    setTemperature(degreesC);
                });
            }
            else if (message.getMethodName() == "setDewPoint")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int degreesC = 10;
                message.beginArgumentRead();
                message.getArgument(degreesC);
                queueDBusCall([=]()
                {
                    setDewPoint(degreesC);
                });
            }
            else if (message.getMethodName() == "setQNH")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double inHg = 29.92;
                message.beginArgumentRead();
                message.getArgument(inHg);
                queueDBusCall([=]()
                {
                    setQNH(inHg);
                });
            }
            else if (message.getMethodName() == "setPrecipitationRatio")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double precipRatio = 0.0;
                message.beginArgumentRead();
                message.getArgument(precipRatio);
                queueDBusCall([=]()
                {
                    setPrecipitationRatio(precipRatio);
                });
            }
            else if (message.getMethodName() == "setThunderstormRatio")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double cbRatio = 0.0;
                message.beginArgumentRead();
                message.getArgument(cbRatio);
                queueDBusCall([=]()
                {
                    setThunderstormRatio(cbRatio);
                });

            }
            else if (message.getMethodName() == "setTurbulenceRatio")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double turbulenceRatio = 0.0;
                message.beginArgumentRead();
                message.getArgument(turbulenceRatio);
                queueDBusCall([=]()
                {
                    setTurbulenceRatio(turbulenceRatio);
                });
            }
            else if (message.getMethodName() == "setRunwayFriction")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int friction = 10;
                message.beginArgumentRead();
                message.getArgument(friction);
                queueDBusCall([=]()
                {
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
                queueDBusCall([=]()
                {
                    setCloudLayer(layer, base, tops, type, coverage);
                });
            }
            else if (message.getMethodName() == "setWindLayer")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int layer = 0;
                int altitude = 0;
                double direction = 0;
                int speed = 0;
                int shearDirection = 0;
                int shearSpeed = 0;
                int turbulence = 0;
                message.beginArgumentRead();
                message.getArgument(layer);
                message.getArgument(altitude);
                message.getArgument(direction);
                message.getArgument(speed);
                message.getArgument(shearDirection);
                message.getArgument(shearSpeed);
                message.getArgument(turbulence);
                queueDBusCall([=]()
                {
                    setWindLayer(layer, altitude, direction, speed, shearDirection, shearSpeed, turbulence);
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

    int CWeather::processDBus()
    {
        invokeQueuedDBusCalls();
        return 1;
    }

}

//! \endcond
