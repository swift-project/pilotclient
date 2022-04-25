/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

#include <QMessageBox>
#include "copyxswiftbusdialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation::XPlane;
using namespace BlackCore::Context;

namespace BlackGui
{
    int CCopyXSwiftBusDialog::displayDialogAndCopyBuildFiles(const QString &xplaneRootDir, bool checkLatestFile, QWidget *parent)
    {
        if (!sGui || sGui->isShuttingDown()) { return 0; }
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return -1; }
        if (checkLatestFile && !CXPlaneUtil::hasNewerXSwiftBusBuild(xplaneRootDir)) { return 0; }
        if (!CXPlaneUtil::hasXSwiftBusBuildAndPluginDir(xplaneRootDir)) { return 0; }
        if (sGui && sGui->getIContextSimulator())
        {
            if (sGui->getIContextSimulator()->isSimulatorAvailable())
            {
                // do not show if already connected with another simulator
                if (!sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulatorInfo().isXPlane()) { return 0; }
            }
        }

        const QMessageBox::StandardButton reply = QMessageBox::question(parent,
                "Copy XSwiftBus",
                QStringLiteral("Copy XSwiftBus from build directory '%1' to plugin directory '%2'?").arg(CSwiftDirectories::getXSwiftBusBuildDirectory(), CXPlaneUtil::xswiftbusPluginDir(xplaneRootDir)),
                QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) { return 0; }
        return CXPlaneUtil::copyXSwiftBusBuildFiles(xplaneRootDir);
    } // ns
}
