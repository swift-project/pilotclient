// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/flightgear/flightgearutil.h"
#include "misc/fileutils.h"
#include "misc/directoryutils.h"
#include "config/buildconfig.h"
#include "qsystemdetection.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QTextStream>
#include <QSettings>
#include <QStandardPaths>

using namespace swift::config;

namespace swift::misc::simulation::flightgear
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
