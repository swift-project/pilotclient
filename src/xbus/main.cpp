/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "plugin.h"
#include "utils.h"
#include "traffic.h"
#include "service.h"
#include <XPLM/XPLMPlanes.h>

#if ! defined(XPLM210)
#define XPLM_MSG_LIVERY_LOADED 108
#endif

// Change QSharedPointer<QCoreApplication> to QSharedPointer<QApplication> below
// in case you want to have Qt Gui components inside a X-Plane plugin. The current
// default was used since QApplication causes an infinite loop in X-Plane on OSX
// platforms. X-Plane is allocating an NSApplication but never calling run(), rather
// it controls the main loop itself and pumps the event Q as needed. This causes
// unusual start conditions for QCocoaEventDispatcher and ends up in the infinite
// loop. Since QCoreApplication is not using QCocoaEventDispatcher it works fine
// and is used as a workaround.
// See https://dev.vatsim-germany.org/issues/293 for more information.

QSharedPointer<QCoreApplication> g_qApp;
XBus::CPlugin *g_plugin;

PLUGIN_API int XPluginStart(char *o_name, char *o_sig, char *o_desc)
{
    std::strcpy(o_name, "X-Bus");
    std::strcpy(o_sig, "org.swift-project.xbus");
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
