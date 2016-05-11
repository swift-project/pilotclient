/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/buildconfig.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"

#include <QDir>
#include <QList>
#include <QPair>
#include <QSettings>
#include <QStringList>
#include <QVariant>

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
                if (CBuildConfig::isCompiledWithFsxSupport())
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

            QString CFsCommonUtil::fsxDir()
            {
                QString dir(fsxDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }\

                //! \todo allow to read a user defined value from settings
                return "P:/FlightSimulatorX (MSI)";
            }

            QString CFsCommonUtil::p3dDir()
            {
                //! \todo P3D resolution
                return fsxDir();
            }

            QString CFsCommonUtil::fsxSimObjectsDirFromRegistry()
            {
                QString fsxPath = fsxDirFromRegistry();
                if (fsxPath.isEmpty()) { return ""; }
                fsxPath = QDir(fsxPath).filePath("SimObjects");
                return fsxPath;
            }

            QString CFsCommonUtil::fsxSimObjectsDir()
            {
                QString dir(fsxSimObjectsDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }

                //! \todo allow to read a user defined value from settings
                return "P:/FSX (MSI)/SimObjects"; // "p:/temp/SimObjects"
            }

            const QStringList &CFsCommonUtil::fsxSimObjectsExcludeDirectories()
            {
                static const QStringList exclude
                {
                    "SimObjects/Animals",
                    "SimObjects/Misc",
                    "SimObjects/GroundVehicles",
                    "SimObjects/Boats"
                };
                return exclude;
            }

            QString CFsCommonUtil::p3dSimObjectsDir()
            {
                //! \todo P3D resolution
                return fsxSimObjectsDir();
            }

            const QStringList &CFsCommonUtil::p3dSimObjectsExcludeDirectories()
            {
                return fsxSimObjectsExcludeDirectories();
            }

            QString CFsCommonUtil::fs9DirFromRegistry()
            {
                QString fs9Path;
                if (CBuildConfig::isCompiledWithFs9Support())
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

            QString CFsCommonUtil::fs9Dir()
            {
                QString dir(fs9DirFromRegistry());
                if (!dir.isEmpty()) { return dir; }

                //! \todo hardcoded sim parts should come from settings
                return "C:/Flight Simulator 9";
            }

            QString CFsCommonUtil::fs9AircraftDirFromRegistry()
            {
                QString fs9Path = fs9DirFromRegistry();
                if (fs9Path.isEmpty()) { return ""; }
                fs9Path = QDir(fs9Path).filePath("Aircraft");
                return fs9Path;
            }

            QString CFsCommonUtil::fs9AircraftDir()
            {
                QString dir(fs9AircraftDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                //! \todo hardcoded sim parts should come from settings
                return "C:/Flight Simulator 9/Aircraft";
            }

            const QStringList &CFsCommonUtil::fs9AircraftObjectsExcludeDirectories()
            {
                static const QStringList exclude;
                return exclude;
            }

        } // namespace
    } // namespace
} // namespace
