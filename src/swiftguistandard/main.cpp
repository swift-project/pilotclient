/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "swiftguistdapplication.h"

#include "blackgui/enableforframelesswindow.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackmisc/audio/audioutils.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/crashhandler.h"

#include <stdlib.h>
#include <iostream>

#include <QApplication>
#include <QTextStream>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackCore;

int main(int argc, char *argv[])
{
    //! [SwiftApplicationDemo]
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
        a.setSignalStartupAutomatically(false); // application will signal startup on its own
        a.splashScreen(CIcons::swift256());
        a.setMinimumSizeInCharacters(60, 42); // experimental
        if (!a.parseAndSynchronizeSetup()) { return EXIT_FAILURE; }
        if (!a.hasSetupReader() || !a.start())
        {
            a.gracefulShutdown();
            return EXIT_FAILURE;
        }
        //! [SwiftApplicationDemo]

        // show window
        CEnableForFramelessWindow::WindowMode windowMode = a.getWindowMode();
        SwiftGuiStd w(windowMode);
        r = a.exec();
    }

    // bye
    return r;
}
