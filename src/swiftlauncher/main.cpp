// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftlauncher.h"
#include "blackgui/guiapplication.h"
#include "blackcore/db/databasereaderconfig.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/icons.h"

#include <QtGlobal>
#include <QApplication>
#include <QPushButton>
#include <QCommandLineParser>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackCore::Db;

int main(int argc, char *argv[])
{
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv); // needed
    Q_UNUSED(qa)
    CGuiApplication a(CApplicationInfo::swiftLauncher(), CApplicationInfo::Laucher, CIcons::swiftLauncher1024());
    a.addVatlibOptions(); // so it can be passed (hand over) to started applications
    a.addParserOption({ { "i", "installer" }, QCoreApplication::translate("main", "Installer setup.") });
    if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }
    a.useWebDataServices(BlackCore::CWebReaderFlags::AllSwiftDbReaders, CDatabaseReaderConfigList::forLauncher());
    a.useFacadeNoContexts();
    if (!a.start())
    {
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }

    CSwiftLauncher launcher;
    const int res = a.exec();
    if (res != EXIT_SUCCESS || !launcher.shouldStartAppDetached())
    {
        return res;
    }

    const bool s = launcher.startDetached();
    return s ? EXIT_SUCCESS : EXIT_FAILURE;
}
