/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "swiftguistdapplication.h"
#include "blackgui/enableforframelesswindow.h"

#include <QtGlobal>
#include <QApplication>
#include <QPushButton>
#include <QProcessEnvironment>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;

int main(int argc, char *argv[])
{
    //! [CSwiftGuiStdApplication]
    CGuiApplication::highDpiScreenSupport();
    QApplication qa(argc, argv);
    CSwiftGuiStdApplication a;
    a.signalStartupAutomatically(); // application will signal startup on its own
    a.splashScreen(":/own/icons/own/swift/swiftCirclePilotClient1024.png");
    if (!a.start()) { return EXIT_FAILURE; }
    //! [CSwiftGuiStdApplication]

    // show window
    CEnableForFramelessWindow::WindowMode windowMode = a.getWindowMode();
    SwiftGuiStd w(windowMode);
    w.show();

    int r = a.exec();
    return r;
}
