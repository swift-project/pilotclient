/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistandard/guimodeenums.h"
#include "swiftlauncher.h"
#include "blackcore/registermetadata.h"
#include "blackgui/guiutility.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/icons.h"
#include "blackmisc/project.h"

#include <QtGlobal>
#include <QProcess>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QCommandLineParser>

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

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, bool &installer, QString &errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addOption({{"i", "installer"}, QCoreApplication::translate("main", "Installer setup.")});

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments()))
    {
        errorMessage = parser.errorText();
        return CommandLineError;
    }

    // help/version
    if (parser.isSet(helpOption)) { return CommandLineHelpRequested; }
    if (parser.isSet(versionOption)) { return CommandLineVersionRequested; }

    installer = parser.isSet("installer");
    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString appName("swift launcher");
    a.setApplicationVersion(CProject::version());
    a.setApplicationName(appName);
    CGuiUtility::initSwiftGuiApplication(a, appName, CIcons::swift24());

    // Process the actual command line arguments given by the user
    QCommandLineParser parser;
    parser.setApplicationDescription(appName);
    QString errorMessage;
    bool installer = false;
    switch (parseCommandLine(parser, installer, errorMessage))
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

    // Dialog to decide external or internal core
    CSwiftLauncher launcher;
    launcher.setWindowIcon(CIcons::swift24());
    if (launcher.exec() == QDialog::Rejected) { return 0; }
    launcher.close();

    QString exe(launcher.getExecutable());
    QStringList exeArgs(launcher.getExecutableArgs());
    Q_ASSERT_X(!exe.isEmpty(), Q_FUNC_INFO, "Missing executable");
    CLogMessage(QCoreApplication::instance()).debug() << launcher.getCmdLine();
    QProcess::startDetached(exe, exeArgs);

    return 0;
}
