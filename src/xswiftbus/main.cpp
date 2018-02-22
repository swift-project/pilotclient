/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "plugin.h"
#include "utils.h"
#include "traffic.h"
#include "service.h"
#include "blackmisc/dbus.h"
#include <XPLM/XPLMPlanes.h>

#if ! defined(XPLM210)
#define XPLM_MSG_LIVERY_LOADED 108
#endif

// Change QSharedPointer<QCoreApplication> to QSharedPointer<QApplication> below
// in case you want to have Qt Gui components inside a X-Plane plugin. The current
// default was used since QApplication causes an infinite loop in X-Plane on MacOS
// platforms. X-Plane is allocating an NSApplication but never calling run(), rather
// it controls the main loop itself and pumps the event Q as needed. This causes
// unusual start conditions for QCocoaEventDispatcher and ends up in the infinite
// loop. Since QCoreApplication is not using QCocoaEventDispatcher it works fine
// and is used as a workaround.
// See https://dev.vatsim-germany.org/issues/293 for more information.

QSharedPointer<QCoreApplication> g_qApp;
XSwiftBus::CPlugin *g_plugin;

PLUGIN_API int XPluginStart(char *o_name, char *o_sig, char *o_desc)
{
#if APL
    // https://developer.x-plane.com/2014/12/mac-plugin-developers-you-should-be-using-native-paths/
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS",1);
#endif

    std::strcpy(o_name, "XSwiftBus");
    std::strcpy(o_sig, "org.swift-project.xswiftbus");
    std::strcpy(o_desc, "Allows swift to connect to X-Plane via D-Bus IPC");

    XSwiftBus::CTraffic::initLegacyData();
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

    g_plugin = new XSwiftBus::CPlugin;

    // Here we can be safely assume that QtDBus was loaded by the process
    preventQtDBusDllUnload();

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
