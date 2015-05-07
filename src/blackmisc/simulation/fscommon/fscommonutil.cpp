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

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {

            using FsRegistryPathPair = QList<QPair<QString, QString>>;

            QString CFsCommonUtil::fsxDirFromRegistry()
            {
                QString fsxPath;
                if (CProject::isCompiledWithFsxSupport())
                {

                    FsRegistryPathPair fsxRegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("AppPath") }
                    };

                    for (const auto &registryPair : fsxRegistryPathPairs)
                    {
                        QSettings fsxRegistry(registryPair.first, QSettings::NativeFormat);
                        fsxPath = fsxRegistry.value(registryPair.second).toString().trimmed();

                        if (!fsxPath.isEmpty()) break;
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

            QString CFsCommonUtil::fs9DirFromRegistry()
            {
                QString fs9Path;
                if (CProject::isCompiledWithFs9Support())
                {
                    FsRegistryPathPair fs9RegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DirectPlay\\Applications\\Microsoft Flight Simulator 2004"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\DirectPlay\\Applications\\Microsoft Flight Simulator 2004"), QStringLiteral("AppPath") }
                    };

                    for (const auto &registryPair : fs9RegistryPathPairs)
                    {
                        QSettings fs9Registry(registryPair.first, QSettings::NativeFormat);
                        fs9Path = fs9Registry.value(registryPair.second).toString().trimmed();

                        if (!fs9Path.isEmpty()) break;
                    }
                }
                return fs9Path;
            }

            QString CFsCommonUtil::fs9AircraftDirFromRegistry()
            {
                QString fs9Path = fs9DirFromRegistry();
                if (fs9Path.isEmpty()) { return ""; }
                fs9Path = QDir(fs9Path).filePath("Aircraft");
                return fs9Path;
            }

        } // namespace
    } // namespace
} // namespace
