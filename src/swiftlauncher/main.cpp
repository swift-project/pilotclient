// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftlauncher.h"
#include "blackgui/guiapplication.h"
#include "blackcore/registermetadata.h"
#include "blackcore/db/databasereaderconfig.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/icons.h"

#include <QtGlobal>
#include <QProcess>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QCommandLineParser>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackCore::Db;

int main(int argc, char *argv[])
{
    //! [SwiftApplicationDemo]
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv); // needed
    Q_UNUSED(qa)
    CGuiApplication a(CApplicationInfo::swiftLauncher(), CApplicationInfo::Laucher, CIcons::swiftLauncher1024());
    a.addVatlibOptions(); // so it can be passed (hand over) to started applications
    a.addParserOption({ { "i", "installer" }, QCoreApplication::translate("main", "Installer setup.") });
    if (!a.parseAndSynchronizeSetup()) { return EXIT_FAILURE; }
    a.useWebDataServices(BlackCore::CWebReaderFlags::AllSwiftDbReaders, CDatabaseReaderConfigList::forLauncher());
    a.useFacadeNoContexts();
    if (!a.start())
    {
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }
    //! [SwiftApplicationDemo]

    // Dialog to decide external or internal core
    CSwiftLauncher launcher;
    CGuiApplication::registerAsRunning(); // needed because launcher's exec is called (normally application exec)
    if (launcher.exec() == QDialog::Rejected) { return EXIT_SUCCESS; }
    launcher.close();
    const bool s = launcher.startDetached();
    return s ? EXIT_SUCCESS : EXIT_FAILURE;
}
