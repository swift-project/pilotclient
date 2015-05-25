/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "introwindow.h"
#include "swiftguistd.h"
#include "guimodeenums.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/blackcorefreefunctions.h"
#include "blackcore/context_runtime_config.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/icons.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/filelogger.h"

#include <QtGlobal>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;

/*!
 * \brief Main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    // register
    BlackMisc::initResources();
    BlackMisc::registerMetadata();
    BlackMisc::Simulation::registerMetadata();
    BlackCore::registerMetadata();
    // BlackMisc::displayAllUserMetatypesTypes();

    // application
    QApplication a(argc, argv);
    CLogHandler::instance()->install();
    CLogHandler::instance()->enableConsoleOutput(true);
    CLogHandler::instance()->handlerForPattern(
        CLogPattern::anyOf({ CLogCategory::contextSlot(), CLogCategory::context() })
    )->enableConsoleOutput(false);
    CLogHandler::instance()->handlerForPattern(
        CLogPattern::anyOf({ CLogCategory::contextSlot(), CLogCategory::context() }).withSeverityAtOrAbove(CStatusMessage::SeverityInfo)
    )->enableConsoleOutput(true);
    CFileLogger fileLogger(QStringLiteral("swiftgui_std"), QString(), &a);
    fileLogger.changeLogPattern(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityDebug));

    // Translations
    QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
    CLogMessage("swift.standardgui.main").debug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
    QTranslator translator;
    if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
    {
        CLogMessage("swift.standardgui.main").debug() << "Translator loaded";
    }

    QIcon icon(BlackMisc::CIcons::swift24());
    QApplication::setWindowIcon(icon);
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameMainWindow()
    }
    );
    a.installTranslator(&translator);
    a.setStyleSheet(s);

    // modes
    BlackGui::CEnableForFramelessWindow::WindowMode windowMode;

    // Dialog to decide external or internal core
    CIntroWindow intro;
    intro.setWindowIcon(icon);
    BlackCore::CRuntimeConfig runtimeConfig;
    if (intro.exec() == QDialog::Rejected)
    {
        return 0;
    }
    else
    {
        GuiModes::CoreMode coreMode = intro.getCoreMode();
        windowMode = intro.getWindowMode();
        QString dbusAddress = CDBusServer::fixAddressToDBusAddress(intro.getDBusAddress());
        switch (coreMode)
        {
        case GuiModes::CoreExternal:
            runtimeConfig =  CRuntimeConfig::remote(dbusAddress);
            break;
        case GuiModes::CoreInGuiProcess:
            runtimeConfig =  CRuntimeConfig::local(dbusAddress);
            break;
        case GuiModes::CoreExternalAudioLocal:
            runtimeConfig =  CRuntimeConfig::remoteLocalAudio(dbusAddress);
            break;
        }
    }
    intro.close();

    // show window
    SwiftGuiStd w(windowMode);
    w.init(runtimeConfig); // object is complete by now
    w.show();

    int r = a.exec();
    return r;
}
