/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftcore.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackcore/contextapplication.h"
#include "blackcore/contextapplicationimpl.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/icons.h"
#include "blackmisc/worker.h"
#include "blackmisc/network/networkutils.h"
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;

int main(int argc, char *argv[])
{
    CGuiApplication a(argc, argv, "swift core");
    a.setWindowIcon(CIcons::swiftNova24());
    a.addWindowStateOption();
    a.addDBusAddressOption();
    a.addParserOption({{"r", "start"}, QCoreApplication::translate("main", "Start the server.")});
    a.addParserOption({{"c", "coreaudio"}, QCoreApplication::translate("main", "Audio in core.")});
    a.parse();

    const QString dBusAdress(a.getCmdDBusAddressValue());
    a.useContexts(a.isParserOptionSet("coreaudio") ?
                  CCoreFacadeConfig::forCoreAllLocalInDBus(dBusAdress) :
                  CCoreFacadeConfig::forCoreAllLocalInDBusNoAudio(dBusAdress));

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        a.parserErrorMessage("I could not detect any system tray on this system.");
        return EXIT_FAILURE;
    }

    CSwiftCore w;
    if (a.getWindowState() != Qt::WindowMinimized) { w.show(); }
    return a.exec();
}
