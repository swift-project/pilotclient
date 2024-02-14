// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/webreaderflags.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/crashhandler.h"
#include "swiftdata.h"

#include <stdlib.h>
#include <QApplication>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackGui;

int main(int argc, char *argv[])
{
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv);
    Q_UNUSED(qa)

    CCrashHandler::instance()->init();
    CGuiApplication a(CApplicationInfo::swiftMappingTool(), CApplicationInfo::MappingTool, CIcons::swiftDatabase48());
    a.splashScreen(CIcons::swiftDatabase256());
    if (!a.parseAndLoadSetup()) { return EXIT_FAILURE; }
    a.useWebDataServices(BlackCore::CWebReaderFlags::AllSwiftDbReaders, CDatabaseReaderConfigList::forMappingTool());
    a.useFacadeNoContexts();
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
