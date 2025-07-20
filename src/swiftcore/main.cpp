// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <cstdlib>

#include <QApplication>
#include <QString>
#include <QSystemTrayIcon>
#include <Qt>

#include "core/corefacadeconfig.h"
#include "gui/guiapplication.h"
#include "misc/crashhandler.h"
#include "misc/icons.h"
#include "swiftcore.h"

using namespace swift::misc;
using namespace swift::core;
using namespace swift::gui;

int main(int argc, char *argv[])
{
    QApplication qa(argc, argv);
    Q_UNUSED(qa) // init of qa is required, but qa not used

    CCrashHandler::instance()->init();
    CGuiApplication a(CApplicationInfo::swiftCore(), CApplicationInfo::PilotClientCore, CIcons::swiftCore24());
    a.addWindowStateOption();
    a.addDBusAddressOption();
    a.addNetworkOptions();
    a.addAudioOptions();
    if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }

    const QString dBusAddress(a.getCmdDBusAddressValue());
    a.initContextsAndStartCoreFacade(CCoreFacadeConfig(CCoreFacadeConfig::LocalInDBusServer, dBusAddress));
    if (!a.start())
    {
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        a.cmdLineErrorMessage("System tray missing", "I could not detect any system tray on this system.");
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }

    CSwiftCore w;
    if (a.getWindowState() != Qt::WindowMinimized) { w.show(); }
    return a.exec();
}
