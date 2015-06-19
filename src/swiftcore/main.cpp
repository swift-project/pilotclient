/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftcore.h"
#include "blackcore/context_runtime.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/icons.h"
#include "blackmisc/worker.h"
#include "blackmisc/networkutils.h"
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
    parser.addOption({{"s", "session"}, QCoreApplication::translate("main", "Use session bus.")});
    parser.addOption({{"y", "system"}, QCoreApplication::translate("main", "Use system bus.")});
    parser.addOption({{"p", "p2p"}, QCoreApplication::translate("main", "Use P2P bus with <address>."), QCoreApplication::translate("main", "address") });
    parser.addOption({{"m", "minimized"}, QCoreApplication::translate("main", "Start minimized in system tray.")});

    if (!parser.parse(QCoreApplication::arguments()))
    {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    if (parser.isSet("session"))
    {
        if (parser.isSet("system") || parser.isSet("p2p"))
        {
            *errorMessage = "Multiple DBus types set at the same time.";
            return CommandLineError;
        }
        setup->m_dbusAddress = CDBusServer::sessionDBusServer();
    }

    if (parser.isSet("system"))
    {
        if (parser.isSet("session") || parser.isSet("p2p"))
        {
            *errorMessage = "Multiple DBus types set at the same time.";
            return CommandLineError;
        }
        setup->m_dbusAddress = CDBusServer::systemDBusServer();
    }

    if (parser.isSet("p2p"))
    {
        const QString address = CDBusServer::fixAddressToDBusAddress(parser.value("p2p"));
        Q_UNUSED(address);

        if (parser.isSet("session") || parser.isSet("system"))
        {
            *errorMessage = "Multiple DBus types set at the same time.";
            return CommandLineError;
        }
    }

    if (parser.isSet("minimized"))
    {
        setup->m_minimzed = true;
    }
    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CGuiUtility::initSwiftGuiApplication(a, "swiftcore", CIcons::swiftNova24());
    QCommandLineParser parser;
    parser.setApplicationDescription("swiftcore");
    parser.addHelpOption();
    parser.addVersionOption();

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

    CSwiftCore w(setup);
    if (!setup.m_minimzed) { w.show(); }
    return a.exec();
}
