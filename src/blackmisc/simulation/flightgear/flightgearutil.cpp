/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/flightgear/flightgearutil.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/directoryutils.h"
#include "blackconfig/buildconfig.h"
#include "qsystemdetection.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QTextStream>
#include <QSettings>
#include <QStandardPaths>

using namespace BlackConfig;

namespace BlackMisc::Simulation::Flightgear
{

    const QString &CFlightgearUtil::flightgearRootDir()
    {
        static QString flightgearRootDir;
        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            QSettings flightgearRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\FlightGear_is1", QSettings::NativeFormat);
            flightgearRootDir = flightgearRegistry.value("InstallLocation").toString().trimmed();
        }
        return flightgearRootDir;
    }

    bool CFlightgearUtil::isFlightgearRootDirExisting()
    {
        static const bool exists = QDir(flightgearRootDir()).exists();
        return exists;
    }

    QStringList CFlightgearUtil::modelDirectoriesFromSimDir(const QString &simulatorDir)
    {
        QStringList dirs;
        if (CBuildConfig::isRunningOnWindowsNtPlatform() && !simulatorDir.isEmpty())
        {
            QString terraSyncFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Flightgear/Aircraft";
            if (QDir(terraSyncFolder).exists()) { dirs.append(terraSyncFolder); }
            QString fgdataAIFolder = simulatorDir + "data/AI/Aircraft";
            if (QDir(fgdataAIFolder).exists()) { dirs.append(fgdataAIFolder); }
        }
        return dirs;
    }

    const QStringList &CFlightgearUtil::flightgearModelDirectories()
    {
        static const QStringList dirs = flightgearRootDir().isEmpty() ? QStringList() : modelDirectoriesFromSimDir(flightgearRootDir());
        return dirs;
    }

    const QStringList &CFlightgearUtil::flightgearModelExcludeDirectoryPatterns()
    {
        static const QStringList empty;
        return empty;
    }
} // namespace
