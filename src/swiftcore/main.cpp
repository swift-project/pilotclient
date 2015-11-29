/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftcore.h"
#include "blackcore/contextruntime.h"
#include "blackcore/contextapplication.h"
#include "blackcore/contextapplicationimpl.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/icons.h"
#include "blackmisc/worker.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/project.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/filelogger.h"
#include "blackgui/guiutility.h"
#include "blackgui/stylesheetutility.h"

#include <QApplication>
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, CSwiftCore::SetupInfo *setup, QString *errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addOption({{"d", "dbus"}, QCoreApplication::translate("main", "DBus options: session, system, p2p."), "dbus"});
    parser.addOption({{"m", "minimized"}, QCoreApplication::translate("main", "Start minimized in system tray.")});
    parser.addOption({{"r", "start"}, QCoreApplication::translate("main", "Start the server.")});
    parser.addOption({{"c", "coreaudio"}, QCoreApplication::translate("main", "Audio in core.")});

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments()))
    {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    // help/version
    if (parser.isSet(helpOption)) { return CommandLineHelpRequested; }
    if (parser.isSet(versionOption)) { return CommandLineVersionRequested; }

    setup->m_dbusAddress = CDBusServer::sessionBusAddress(); // default
    if (parser.isSet("dbus"))
    {
        QString v(parser.value("dbus").trimmed().toLower());
        if (v.startsWith("p2p") || v.contains("peer") || v.contains("tcp:") || v.contains("host"))
        {
            setup->m_dbusAddress = CDBusServer::p2pAddress();
        }
        else if (v.contains("sys"))
        {
            setup->m_dbusAddress = CDBusServer::systemBusAddress(); // default
        }
    }

    if (parser.isSet("minimized")) { setup->m_minimzed = true; }
    if (parser.isSet("start")) { setup->m_start = true; }
    if (parser.isSet("coreaudio")) { setup->m_coreAudio = true; }

    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString appName("swift core");
    a.setApplicationVersion(CProject::version());
    a.setApplicationName(appName);
    QCommandLineParser parser;
    parser.setApplicationDescription(appName);

    CSwiftCore::SetupInfo setup;
    QString errorMessage;
    switch (parseCommandLine(parser, &setup, &errorMessage))
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

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }

    CGuiUtility::initSwiftGuiApplication(a, appName, CIcons::swiftNova24());
    CSwiftCore w(setup);
    if (!setup.m_minimzed) { w.show(); }
    return a.exec();
}
