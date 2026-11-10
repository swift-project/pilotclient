// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <cstdlib>

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include "miniguiwindow.h"
#include "swiftguiminiapplication.h"

#include "gui/settings/minisettings.h"
#include "misc/crashhandler.h"
#include "misc/icons.h"
#include "misc/settingscache.h"

using namespace swift::misc;
using namespace swift::gui::settings;

int main(int argc, char *argv[])
{
    QApplication qa(argc, argv);
    Q_UNUSED(qa) // application init needed

    CCrashHandler::instance()->init();

    // Settings must be loaded from disk BEFORE reading the language, otherwise
    // CSettingsCache is still empty and TMiniLanguage::defaultValue() ("system") is returned.
    CSettingsCache::instance()->loadFromStore();

    // Language selection: from the Mini-GUI settings (Miscellaneous dialog), otherwise system language.
    const CSetting<TMiniLanguage> languageSetting { &qa };
    const QString languageCode = languageSetting.get();
    const QLocale locale = (languageCode.isEmpty() || languageCode == QStringLiteral("system")) ? QLocale::system() :
                                                                                                  QLocale(languageCode);

    // Qt's own translations (standard dialogs such as QMessageBox etc.)
    QTranslator qtTranslator;
    if (qtTranslator.load(locale, QStringLiteral("qt"), QStringLiteral("_"),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QCoreApplication::installTranslator(&qtTranslator);
    }

    // swiftguimini's own translations (.qm files are located next to the executable in the "i18n" subfolder)
    QTranslator appTranslator;
    const QString translationsDir = QCoreApplication::applicationDirPath() + QStringLiteral("/i18n");
    if (appTranslator.load(locale, QStringLiteral("swiftguimini"), QStringLiteral("_"), translationsDir))
    {
        QCoreApplication::installTranslator(&appTranslator);
    }

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