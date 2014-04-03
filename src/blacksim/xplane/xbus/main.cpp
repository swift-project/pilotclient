/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define _CRT_SECURE_NO_WARNINGS
#include "stub.h"
#include "utils.h"

QSharedPointer<QApplication> g_qApp;
XBus::CStub *g_stub;

PLUGIN_API int XPluginStart(char *o_name, char *o_sig, char *o_desc)
{
    std::strcpy(o_name, "X-Bus");
    std::strcpy(o_sig, "net.vatsim.XBus");
    std::strcpy(o_desc, "Allows pilot client to connect to X-Plane via D-Bus");
    return 1;
}

PLUGIN_API void XPluginStop()
{
}

PLUGIN_API int XPluginEnable()
{
    QXPlaneMessageHandler::install();
    g_qApp = QSharedApplication::sharedInstance();
    QXPlaneEventLoop::exec();
    g_stub = new XBus::CStub;
    return 1;
}

PLUGIN_API void XPluginDisable()
{
    delete g_stub;
    g_qApp.reset();
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, long msg, void *param)
{
    Q_UNUSED(from);
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
