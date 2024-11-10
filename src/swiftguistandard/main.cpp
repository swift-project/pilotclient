// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftguistd.h"
#include "swiftguistdapplication.h"

#include "blackgui/enableforframelesswindow.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "misc/audio/audioutils.h"
#include "misc/crashhandler.h"

#include <cstdlib>

#include <QApplication>

using namespace BlackGui;
using namespace swift::misc;
using namespace swift::misc::audio;
using namespace BlackCore;

int main(int argc, char *argv[])
{
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv);
    Q_UNUSED(qa) // application init needed

    // initWindowsAudioDevices();
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
