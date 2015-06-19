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

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CGuiUtility::initSwiftGuiApplication(a, "swiftgui", CIcons::swift24());

    // modes
    BlackGui::CEnableForFramelessWindow::WindowMode windowMode;

    // Dialog to decide external or internal core
    CIntroWindow intro;
    intro.setWindowIcon(CIcons::swift24());
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
