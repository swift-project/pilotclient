// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <cstdlib>

#include <QApplication>

#include "miniguiwindow.h"
#include "swiftguiminiapplication.h"

#include "misc/crashhandler.h"
#include "misc/icons.h"

using namespace swift::misc;

int main(int argc, char *argv[])
{
    QApplication qa(argc, argv);
    Q_UNUSED(qa) // application init needed

    CCrashHandler::instance()->init();

    int r = 0;
    {
        CSwiftGuiMiniApplication a;
        if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }
        a.splashScreen(CIcons::swift256());
        if (!a.start())
        {
            a.gracefulShutdown();
            return EXIT_FAILURE;
        }

        CMiniGuiWindow w;
        a.initMainApplicationWidget(&w);
        w.show();
        r = a.exec();
    }

    return r;
}