/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistandard/guimodeenums.h"
#include "introwindow.h"
#include "blackcore/blackcorefreefunctions.h"
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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CGuiUtility::initSwiftGuiApplication(a, "swiftgui", CIcons::swift24());

    // Dialog to decide external or internal core
    CIntroWindow intro;
    intro.setWindowIcon(CIcons::swift24());
    if (intro.exec() == QDialog::Rejected) { return 0; }

    GuiModes::CoreMode coreMode = intro.getCoreMode();
    CEnableForFramelessWindow::WindowMode windowMode = intro.getWindowMode();
    QString dBusAddress(intro.getDBusAddress());
    intro.close();

    QString args(" --core %1 --dbus %2 --window %3");
    QString exe = QDir::currentPath() + "/swiftguistd.exe" +
                  args.arg(GuiModes::coreModeToString(coreMode)).arg(dBusAddress).arg(CEnableForFramelessWindow::windowModeToString(windowMode));
    QProcess *process = new QProcess(QCoreApplication::instance());
    Q_ASSERT_X(process, Q_FUNC_INFO, "Cannot start process");
    Q_UNUSED(process);
    process->startDetached(exe);

    return 0;
}
