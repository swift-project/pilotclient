// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftlauncher.h"
#include "blackgui/guiapplication.h"
#include "blackcore/db/databasereaderconfig.h"
#include "misc/directoryutils.h"
#include "misc/icons.h"
#include "blackcore/webdataservices.h"

#include <QtGlobal>
#include <QApplication>
#include <QPushButton>
#include <QCommandLineParser>

using namespace BlackGui;
using namespace swift::misc;
using namespace BlackCore;
using namespace BlackCore::Db;

//! Init the DB cache from local resource files if the cache has no timestamp or the cache was not updated since 2 years
void initDbCacheFromResourceFileIfRequired(CGuiApplication &a)
{
    Q_ASSERT_X(a.hasWebDataServices(), Q_FUNC_INFO, "Requires web services");

    CWebDataServices *webDataServices = a.getWebDataServices();

    // caches from local files (i.e. the files delivered)
    const QDateTime ts = webDataServices->getLatestDbEntityCacheTimestamp();
    if (!ts.isValid() || ts < QDateTime::currentDateTimeUtc().addYears(-2))
    {
        webDataServices->initDbCachesFromLocalResourceFiles(false);
    }
}

int main(int argc, char *argv[])
{
    CGuiApplication::highDpiScreenSupport(CGuiApplication::scaleFactor(argc, argv));
    QApplication qa(argc, argv); // needed
    Q_UNUSED(qa)
    CGuiApplication a(CApplicationInfo::swiftLauncher(), CApplicationInfo::Laucher, CIcons::swiftLauncher1024());
    a.addNetworkOptions(); // so it can be passed (hand over) to started applications
    a.addDBusAddressOption(); // required by the dbus server address selector
    const QCommandLineOption installerOption { { "i", "installer" }, QCoreApplication::translate("main", "Installer setup.") };
    a.addParserOption(installerOption);
    if (!a.parseCommandLineArgsAndLoadSetup()) { return EXIT_FAILURE; }
    a.initAndStartWebDataServices(BlackCore::CWebReaderFlags::AllSwiftDbReaders, CDatabaseReaderConfigList::forLauncher());

    const bool installMode = a.isParserOptionSet(installerOption);
    if (installMode) initDbCacheFromResourceFileIfRequired(a);

    a.startCoreFacadeWithoutContexts();
    if (!a.start())
    {
        a.gracefulShutdown();
        return EXIT_FAILURE;
    }

    CSwiftLauncher launcher(installMode);
    const int res = a.exec();
    if (res != EXIT_SUCCESS || !launcher.shouldStartAppDetached())
    {
        return res;
    }

    const bool s = launcher.startDetached();
    return s ? EXIT_SUCCESS : EXIT_FAILURE;
}
