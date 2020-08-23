/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
    if (!a.parseAndSynchronizeSetup()) { return EXIT_FAILURE; }

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
