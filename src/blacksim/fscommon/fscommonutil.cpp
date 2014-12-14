/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/project.h"
#include "fscommonutil.h"
#include <QSettings>
#include <QDir>

using namespace BlackMisc;

namespace BlackSim
{
    namespace FsCommon
    {

        QString CFsCommonUtil::fsxDirFromRegistry()
        {
            QString fsxPath;
            if (CProject::isCompiledWithFsxSupport())
            {
                // set FSX path
                QSettings fsxRegistry("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0", QSettings::NativeFormat);
                fsxPath = fsxRegistry.value("AppPath").toString().trimmed();
                if (fsxPath.isEmpty())
                {
                    // another trial
                    QSettings fsxRegistry("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0", QSettings::NativeFormat);
                    fsxPath = fsxRegistry.value("SetupPath").toString().trimmed();
                }
            }
            return fsxPath;
        }

        QString CFsCommonUtil::fsxSimObjectsDirFromRegistry()
        {
            QString fsxPath = fsxDirFromRegistry();
            if (fsxPath.isEmpty()) { return ""; }
            fsxPath = QDir(fsxPath).filePath("SimObjects");
            return fsxPath;
        }

    } // namespace
} // namespace
