// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/corefacadeconfig.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/audio/audioutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/crashhandler.h"
#include "swiftcore.h"

#include <stdlib.h>
#include <QApplication>
#include <QString>
#include <QSystemTrayIcon>
#include <Qt>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackCore;
using namespace BlackGui;

int main(int argc, char *argv[])
{
    //! [SwiftApplicationDemo]
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv);
    Q_UNUSED(qa) // init of qa is required, but qa not used

    // initWindowsAudioDevices();
    CCrashHandler::instance()->init();
    CGuiApplication a(CApplicationInfo::swiftCore(), CApplicationInfo::PilotClientCore, CIcons::swiftCore24());
    a.addWindowStateOption();
    a.addDBusAddressOption();
    a.addVatlibOptions();
    a.addAudioOptions();
    if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }

    const QString dBusAdress(a.getCmdDBusAddressValue());
    a.useContexts(CCoreFacadeConfig::forCoreAllLocalInDBus(dBusAdress));
    if (!a.start())
    {
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }
    //! [SwiftApplicationDemo]

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        a.cmdLineErrorMessage("I could not detect any system tray on this system.");
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }

    CSwiftCore w;
    if (a.getWindowState() != Qt::WindowMinimized) { w.show(); }
    return a.exec();
}
