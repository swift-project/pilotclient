/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

#include <QMessageBox>
#include "copyxswiftbusdialog.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/directoryutils.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackGui
{
    int CCopyXSwiftBusDialog::displayDialogAndCopyBuildFiles(const QString &xplaneRootDir, QWidget *parent)
    {
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return -1; }
        if (!CXPlaneUtil::hasNewerXSwiftBusBuild(xplaneRootDir)) { return 0; }
        const QMessageBox::StandardButton reply =
            QMessageBox::question(parent, "Copy XSwiftBus",
                                  QString("Copy XSwiftBus from build directory '%1' to plugin directory '%2'?").arg(CDirectoryUtils::getXSwiftBusBuildDirectory(), CXPlaneUtil::xswiftbusPluginDir(xplaneRootDir)),
                                  QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) { return 0; }
        return CXPlaneUtil::copyXSwiftBusBuildFiles(xplaneRootDir);
    } // ns
}
