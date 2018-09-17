/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackconfig/buildconfig.h"

#include <QDir>
#include <QList>
#include <QPair>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;

//
// all FSX/P3D keys: http://www.fsdeveloper.com/forum/threads/registry-keys-fsx-fsx-se-p3dv1-p3dv2.432633/
//

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            using FsRegistryPathPair = QList<QPair<QString, QString>>;

            QString fsxDirFromRegistryImpl()
            {
                QString fsxPath;
                if (CBuildConfig::isCompiledWithFsxSupport())
                {
                    FsRegistryPathPair fsxRegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator - Steam Edition\\10.0"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("SetupPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft Games\\Flight simulator\\10.0"), QStringLiteral("SetupPath") }
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

            const QString &CFsCommonUtil::fsxDirFromRegistry()
            {
                static const QString fsxPath(fsxDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxDirImpl()
            {
                QString dir(CFsCommonUtil::fsxDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                QStringList someDefaultDirs(
                {
                    "C:/Program Files (x86)/Microsoft Games/Microsoft Flight Simulator X",
                    "C:/FSX"
                });
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    // developer directories
                    someDefaultDirs.push_back("P:/FSX (MSI)");
                }
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsCommonUtil::fsxDir()
            {
                static const QString dir(fsxDirImpl());
                return dir;
            }

            QString fsxSimObjectsDirFromRegistryImpl()
            {
                const QString fsxPath = CFsCommonUtil::fsxDirFromRegistry();
                if (fsxPath.isEmpty()) { return ""; }
                return CFsCommonUtil::fsxSimObjectsDirFromSimDir(fsxPath);
            }

            const QString &CFsCommonUtil::fsxSimObjectsDirFromRegistry()
            {
                static const QString fsxPath(fsxSimObjectsDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxSimObjectsDirImpl()
            {
                QString dir(CFsCommonUtil::fsxDir());
                if (dir.isEmpty()) { return ""; }
                return CFsCommonUtil::fsxSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::fsxSimObjectsDir()
            {
                static const QString dir(fsxSimObjectsDirImpl());
                return dir;
            }

            QString CFsCommonUtil::fsxSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return QStringLiteral(""); }
                return CFileUtils::appendFilePaths(simDir, "SimObjects");
            }

            const QStringList &CFsCommonUtil::fsxSimObjectsExcludeDirectoryPatterns()
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

            QString p3dDirFromRegistryImpl()
            {
                QString p3dPath;
                if (CBuildConfig::isCompiledWithP3DSupport())
                {
                    FsRegistryPathPair p3dRegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v4"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v3"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v2"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\LockheedMartin\\Prepar3d"), QStringLiteral("AppPath") }
                    };
                    for (const auto &registryPair : p3dRegistryPathPairs)
                    {
                        QSettings p3dRegistry(registryPair.first, QSettings::NativeFormat);
                        p3dPath = p3dRegistry.value(registryPair.second).toString().trimmed();

                        if (!p3dPath.isEmpty()) break;
                    }
                }
                return p3dPath;
            }

            const QString &CFsCommonUtil::p3dDirFromRegistry()
            {
                static const QString p3dPath(p3dDirFromRegistryImpl());
                return p3dPath;
            }

            QString p3dDirImpl()
            {
                QString dir(CFsCommonUtil::p3dDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                const QStringList someDefaultDirs(
                {
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D v4",
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D v3",
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D v2",
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D"
                });
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsCommonUtil::p3dDir()
            {
                static const QString dir(p3dDirImpl());
                return dir;
            }

            QString p3dSimObjectsDirFromRegistryImpl()
            {
                const QString p3dPath = CFsCommonUtil::p3dDirFromRegistry();
                if (p3dPath.isEmpty()) { return ""; }
                return CFsCommonUtil::fsxSimObjectsDirFromSimDir(p3dPath);
            }

            const QString &CFsCommonUtil::p3dSimObjectsDirFromRegistry()
            {
                static const QString p3dPath(p3dSimObjectsDirFromRegistryImpl());
                return p3dPath;
            }

            QString p3dSimObjectsDirImpl()
            {
                QString dir(CFsCommonUtil::p3dDir());
                if (dir.isEmpty()) { return ""; }
                return CFsCommonUtil::fsxSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::p3dSimObjectsDir()
            {
                static const QString dir(p3dSimObjectsDirImpl());
                return dir;
            }

            QString CFsCommonUtil::p3dSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return QStringLiteral(""); }
                return CFileUtils::appendFilePaths(simDir, "SimObjects");
            }

            const QStringList &CFsCommonUtil::p3dSimObjectsExcludeDirectoryPatterns()
            {
                static const QStringList exclude
                {
                    // FSX
                    "SimObjects/Animals",
                    "SimObjects/Misc",
                    "SimObjects/GroundVehicles",
                    "SimObjects/Boats",

                    // P3D new
                    "SimObjects/Avatars",
                    "SimObjects/Countermeasures",
                    "SimObjects/Submersible",
                    "SimObjects/Weapons",
                };
                return exclude;
            }

            QString fs9DirFromRegistryImpl()
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

            const QString &CFsCommonUtil::fs9DirFromRegistry()
            {
                static const QString fs9Path(fs9DirFromRegistryImpl());
                return fs9Path;
            }

            QString fs9DirImpl()
            {
                QString dir(CFsCommonUtil::fs9DirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                const QStringList someDefaultDirs(
                {
                    "C:/Flight Simulator 9",
                    "C:/FS9"
                });
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsCommonUtil::fs9Dir()
            {
                static const QString v(fs9DirImpl());
                return v;
            }

            QString fs9AircraftDirFromRegistryImpl()
            {
                QString fs9Path = CFsCommonUtil::fs9DirFromRegistry();
                if (fs9Path.isEmpty()) { return QStringLiteral(""); }
                return CFsCommonUtil::fs9AircraftDirFromSimDir(fs9Path);
            }

            const QString &CFsCommonUtil::fs9AircraftDirFromRegistry()
            {
                static const QString dir(fs9AircraftDirFromRegistryImpl());
                return dir;
            }

            QString fs9AircraftDirImpl()
            {
                const QString dir(CFsCommonUtil::fs9Dir());
                if (dir.isEmpty()) { return QStringLiteral(""); }
                return CFsCommonUtil::fs9AircraftDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::fs9AircraftDir()
            {
                static const QString dir(fs9AircraftDirImpl());
                return dir;
            }

            QString CFsCommonUtil::fs9AircraftDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return QStringLiteral(""); }
                return CFileUtils::appendFilePaths(simDir, "Aircraft");
            }

            const QStringList &CFsCommonUtil::fs9AircraftObjectsExcludeDirectoryPatterns()
            {
                static const QStringList exclude;
                return exclude;
            }

            bool CFsCommonUtil::adjustFileDirectory(CAircraftModel &model, const QString &simObjectsDirectory)
            {
                if (model.hasExistingCorrespondingFile()) { return true; }
                if (simObjectsDirectory.isEmpty()) { return false; }
                if (!model.hasFileName()) { return false; } // we can do nothing here

                const QString simObjectsDirectoryFix = CFileUtils::fixWindowsUncPath(simObjectsDirectory);
                const QDir dir(simObjectsDirectoryFix);
                if (!dir.exists()) { return false; }

                const QString lastSegment = QStringLiteral("/") % CFileUtils::lastPathSegment(simObjectsDirectoryFix) % QStringLiteral("/");
                const int index = model.getFileName().lastIndexOf(lastSegment);
                if (index < 0) { return false; }
                const QString relPart = model.getFileName().mid(index + lastSegment.length());
                if (relPart.isEmpty()) { return false; }
                const QString newFile = CFileUtils::appendFilePathsAndFixUnc(simObjectsDirectory, relPart);
                const QFileInfo nf(newFile);
                if (!nf.exists()) { return false; }

                model.setFileName(newFile);
                return true;
            }

            bool CFsCommonUtil::adjustFileDirectory(CAircraftModel &model, const QStringList &simObjectsDirectories)
            {
                for (const QString &simObjectDir : simObjectsDirectories)
                {
                    if (CFsCommonUtil::adjustFileDirectory(model, simObjectDir)) { return true; }
                }
                return false;
            }

            int CFsCommonUtil::copyFsxTerrainProbeFiles(const QString &simObjectDir, CStatusMessageList &messages)
            {
                static const CLogCategoryList cats({ CLogCategory::validation(), CLogCategory::driver() });
                messages.clear();
                if (!CDirectoryUtils::existsUnemptyDirectory(CDirectoryUtils::shareTerrainProbeDirectory()))
                {
                    messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, QString("No terrain probe source files in '%1'").arg(CDirectoryUtils::shareTerrainProbeDirectory())));
                    return -1;
                }

                if (simObjectDir.isEmpty())
                {
                    messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "No simObject directory"));
                    return -1;
                }

                QString targetDir = CFileUtils::appendFilePathsAndFixUnc(simObjectDir, "Misc");
                QDir td(targetDir);
                if (!td.exists())
                {
                    messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, QString("Cannot access target directory '%1'").arg(targetDir)));
                    return -1;
                }

                const QString lastSegment = CFileUtils::lastPathSegment(CDirectoryUtils::shareTerrainProbeDirectory());
                targetDir = CFileUtils::appendFilePathsAndFixUnc(targetDir, lastSegment);
                const bool hasDir = td.mkpath(targetDir);
                if (!hasDir)
                {
                    messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, QString("Cannot create target directory '%1'").arg(targetDir)));
                    return -1;
                }

                const int copied = CDirectoryUtils::copyDirectoryRecursively(CDirectoryUtils::shareTerrainProbeDirectory(), targetDir, true);
                messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityInfo, QString("Copied %1 files from '%2' to '%3'").arg(copied).arg(CDirectoryUtils::shareTerrainProbeDirectory(), targetDir)));
                return copied;
            }
        } // namespace
    } // namespace
} // namespace
