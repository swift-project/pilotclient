/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/corefacadeconfig.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/icons.h"
#include "blackmisc/directoryutils.h"
#include "swiftcore.h"

#include <stdlib.h>
#include <QApplication>
#include <QString>
#include <QSystemTrayIcon>
#include <Qt>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;

int main(int argc, char *argv[])
{
    //! [SwiftApplicationDemo]
    CGuiApplication::highDpiScreenSupport(CGuiApplication::parseScaleFactor(argc, argv));
    QApplication qa(argc, argv);
    Q_UNUSED(qa); // init of qa is required, but qa not used
    CGuiApplication a(CApplicationInfo::swiftCore(), CApplicationInfo::PilotClientCore, CIcons::swiftCore24());
    a.addWindowStateOption();
    a.addDBusAddressOption();
    a.addVatlibOptions();
    a.addParserOption({{"c", "coreaudio"}, QCoreApplication::translate("main", "Audio in core.")});
    if (!a.parseAndSynchronizeSetup()) { return EXIT_FAILURE; }

    const QString dBusAdress(a.getCmdDBusAddressValue());
    a.useContexts(a.isParserOptionSet("coreaudio") ?
                  CCoreFacadeConfig::forCoreAllLocalInDBus(dBusAdress) :
                  CCoreFacadeConfig::forCoreAllLocalInDBusNoAudio(dBusAdress));
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
