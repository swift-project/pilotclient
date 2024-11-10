// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <QMessageBox>
#include "copyxswiftbusdialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "config/buildconfig.h"

using namespace swift::config;
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
                                                                        "Copy xswiftbus",
                                                                        QStringLiteral("Copy xswiftbus from build directory '%1' to plugin directory '%2'?").arg(CSwiftDirectories::getXSwiftBusBuildDirectory(), CXPlaneUtil::xswiftbusPluginDir(xplaneRootDir)),
                                                                        QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) { return 0; }
        return CXPlaneUtil::copyXSwiftBusBuildFiles(xplaneRootDir);
    } // ns
}
