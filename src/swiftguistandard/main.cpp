// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <cstdlib>

#include <QApplication>

#include "swiftguistd.h"
#include "swiftguistdapplication.h"

#include "gui/enableforframelesswindow.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/crashhandler.h"

using namespace swift::gui;
using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::core;

int main(int argc, char *argv[])
{
    QApplication qa(argc, argv);
    Q_UNUSED(qa) // application init needed

    CCrashHandler::instance()->init();

    // at shutdown the whole application will be "deleted" outside the block
    // that should already delete all widgets
    int r = 0;
    {
        CSwiftGuiStdApplication a; // application with contexts
        if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }
        a.splashScreen(CIcons::swift256());
        a.setMinimumSizeInCharacters(60, 42); // experimental
        if (!a.start())
        {
            a.gracefulShutdown();
            return EXIT_FAILURE;
        }

        // show window
        CEnableForFramelessWindow::WindowMode windowMode = a.getWindowMode();
        SwiftGuiStd w(windowMode);
        r = a.exec();
    }

    // bye
    return r;
}
