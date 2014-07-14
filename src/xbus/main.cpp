/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define NOMINMAX
#include "plugin.h"
#include "utils.h"
#include "traffic.h"
#include "service.h"
#include <XPLM/XPLMPlanes.h>

#if ! defined(XPLM210)
#define XPLM_MSG_LIVERY_LOADED 108
#endif

QSharedPointer<QApplication> g_qApp;
XBus::CPlugin *g_plugin;

PLUGIN_API int XPluginStart(char *o_name, char *o_sig, char *o_desc)
{
    std::strcpy(o_name, "X-Bus");
    std::strcpy(o_sig, "net.vatsim.XBus");
    std::strcpy(o_desc, "Allows pilot client to connect to X-Plane via D-Bus");

    XBus::CTraffic::initLegacyData();
    return 1;
}

PLUGIN_API void XPluginStop()
{
}

PLUGIN_API int XPluginEnable()
{
    qRegisterMetaType<QDoubleList>();
    qDBusRegisterMetaType<QDoubleList>();

    QXPlaneMessageHandler::install();
    g_qApp = QSharedApplication::sharedInstance();
    QXPlaneEventLoop::exec();

    g_plugin = new XBus::CPlugin;
    return 1;
}

PLUGIN_API void XPluginDisable()
{
    delete g_plugin;
    g_qApp.reset();
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, long msg, void *param)
{
    if (from == XPLM_PLUGIN_XPLANE)
    {
        switch (msg)
        {
        case XPLM_MSG_PLANE_LOADED:
        case XPLM_MSG_LIVERY_LOADED:
            if (reinterpret_cast<intptr_t>(param) == XPLM_USER_AIRCRAFT)
            {
                g_plugin->onAircraftModelChanged();
            }
            break;

        case XPLM_MSG_AIRPORT_LOADED:
            g_plugin->onAircraftRepositioned();
            break;
        }
    }
}
