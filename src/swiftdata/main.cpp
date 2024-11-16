// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <cstdlib>

#include <QApplication>
#include <QtGlobal>

#include "core/webreaderflags.h"
#include "gui/guiapplication.h"
#include "misc/crashhandler.h"
#include "misc/icons.h"
#include "swiftdata.h"

using namespace swift::misc;
using namespace swift::core;
using namespace swift::core::db;
using namespace swift::gui;

int main(int argc, char *argv[])
{
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv);
    Q_UNUSED(qa)

    CCrashHandler::instance()->init();
    CGuiApplication a(CApplicationInfo::swiftMappingTool(), CApplicationInfo::MappingTool, CIcons::swiftDatabase48());
    if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }
    a.splashScreen(CIcons::swiftDatabase256());
    a.initAndStartWebDataServices(swift::core::CWebReaderFlags::AllSwiftDbReaders,
                                  CDatabaseReaderConfigList::forMappingTool());
    a.startCoreFacadeWithoutContexts();
    if (!a.start())
    {
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }
    CSwiftData w;
    w.show();
    int r = a.exec();
    return r;
}
