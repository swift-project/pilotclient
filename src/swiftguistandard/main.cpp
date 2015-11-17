/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "guimodeenums.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/blackcorefreefunctions.h"
#include "blackcore/context_runtime_config.h"
#include "blackgui/guiutility.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/icons.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/filelogger.h"

#include <QtGlobal>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QProcessEnvironment>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser,
                                        CEnableForFramelessWindow::WindowMode &windowMode,
                                        GuiModes::CoreMode &coreMode,
                                        QString &dBusAddress,
                                        QString &errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    // value core mode
    QCommandLineOption modeOption(QStringList() << "c" << "core",
                                  QCoreApplication::translate("main", "Core mode: (e)xternal, (g)ui (in GUI process), (l)ocalaudio (external, but local audio)."),
                                  "coremode");
    parser.addOption(modeOption);

    // value DBus address
    QCommandLineOption dBusOption(QStringList() << "dbus" << "dbus-address",
                                  QCoreApplication::translate("main", "DBUS address."),
                                  "dbusaddress");
    parser.addOption(dBusOption);

    // Window type
    QCommandLineOption windowOption(QStringList() << "w" << "window",
                                    QCoreApplication::translate("main", "Windows: (n)ormal, (f)rameless, (t)ool."),
                                    "windowtype");
    parser.addOption(windowOption);

    // help/version
    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    // evaluate
    if (!parser.parse(QCoreApplication::arguments()))
    {
        errorMessage = parser.errorText();
        return CommandLineError;
    }

    if (parser.isSet(helpOption)) { return CommandLineHelpRequested; }
    if (parser.isSet(versionOption)) { return CommandLineVersionRequested; }

    if (parser.isSet(dBusOption))
    {
        QString v(parser.value(dBusOption).trimmed());
        dBusAddress = CDBusServer::fixAddressToDBusAddress(v);
        if (!CDBusServer::isDBusAvailable(dBusAddress))
        {
            errorMessage = "DBus server at " + dBusAddress + " can not be reached";
            return CommandLineError;
        }
    }

    if (parser.isSet(windowOption))
    {
        QString v(parser.value(windowOption).trimmed());
        windowMode = CEnableForFramelessWindow::stringToWindowMode(v);
    }

    if (parser.isSet(modeOption))
    {
        QString v(parser.value(modeOption));
        coreMode = GuiModes::stringToCoreMode(v);
    }

    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString appName("swift pilot client GUI");
    a.setApplicationVersion(CProject::version());
    a.setApplicationName(appName);

    // Process the actual command line arguments given by the user
    QCommandLineParser parser;
    parser.setApplicationDescription(appName);

    CEnableForFramelessWindow::WindowMode windowMode = CEnableForFramelessWindow::WindowNormal;
    GuiModes::CoreMode coreMode = GuiModes::CoreInGuiProcess;
    QString dBusAddress, errorMessage;
    switch (parseCommandLine(parser, windowMode, coreMode, dBusAddress, errorMessage))
    {
    case CommandLineOk:
        break;
    case CommandLineError:
        CGuiUtility::commandLineErrorMessage(errorMessage, parser);
        return 1;
    case CommandLineVersionRequested:
        CGuiUtility::commandLineVersionRequested();
        return 0;
    case CommandLineHelpRequested:
        CGuiUtility::commandLineHelpRequested(parser);
        return 0;
    }

    BlackCore::CRuntimeConfig runtimeConfig;
    switch (coreMode)
    {
    case GuiModes::CoreExternalCoreAudio:
        runtimeConfig = CRuntimeConfig::remote(dBusAddress);
        break;
    case GuiModes::CoreInGuiProcess:
        runtimeConfig = CRuntimeConfig::local(dBusAddress);
        break;
    case GuiModes::CoreExternalAudioGui:
        runtimeConfig = CRuntimeConfig::remoteLocalAudio(dBusAddress);
        break;
    }

    // show window
    CGuiUtility::initSwiftGuiApplication(a, appName, CIcons::swift24());
    SwiftGuiStd w(windowMode);
    w.init(runtimeConfig); // object is complete by now
    w.show();

    int r = a.exec();
    return r;
}
