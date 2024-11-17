// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef NOMINMAX
#    define NOMINMAX
#endif

// clang-format off
#include "plugin.h"
#include "traffic.h"
#include "utils.h"
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMPlugin.h>
#include <cstring>
// clang-format on

#if !defined(XPLM210)
#    define XPLM_MSG_LIVERY_LOADED 108
#endif

static XSwiftBus::CPlugin *g_plugin = nullptr;

PLUGIN_API int XPluginStart(char *o_name, char *o_sig, char *o_desc)
{
#ifdef APL
    // https://developer.x-plane.com/2014/12/mac-plugin-developers-you-should-be-using-native-paths/
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
#endif

    INFO_LOG("xswiftbus plugin starting");
    std::strcpy(o_name, "xswiftbus");
    std::strcpy(o_sig, "org.swift-project.xswiftbus");
    std::strcpy(o_desc, "Allows swift to connect to X-Plane via D-Bus IPC");
    return 1;
}

PLUGIN_API void XPluginStop()
{
    // void
}

PLUGIN_API int XPluginEnable()
{
    g_plugin = new XSwiftBus::CPlugin;
    return 1;
}

PLUGIN_API void XPluginDisable() { delete g_plugin; }

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, long msg, void *param)
{
    if (from == XPLM_PLUGIN_XPLANE)
    {
        if (!g_plugin || !g_plugin->isRunning())
        {
            WARNING_LOG("Received message, but plugin NOT running");
            return;
        }

        switch (msg)
        {
        case XPLM_MSG_PLANE_LOADED:
        case XPLM_MSG_LIVERY_LOADED:
            if (reinterpret_cast<intptr_t>(param) == XPLM_USER_AIRCRAFT) { g_plugin->onAircraftModelChanged(); }
            break;

        case XPLM_MSG_AIRPORT_LOADED: g_plugin->onAircraftRepositioned(); break;

        case XPLM_MSG_SCENERY_LOADED: g_plugin->onSceneryLoaded(); break;
        }
    }
}
