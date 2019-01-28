/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "fscommonutil.h"
#include "aircraftcfgparser.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

#include <QDir>
#include <QList>
#include <QPair>
#include <QFileInfo>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDomDocument>
#include <QDomNodeList>
#include <QSettings>
#include <QStringBuilder>

using namespace BlackConfig;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            using FsRegistryPathPair = QList<QPair<QString, QString>>;

            const CLogCategoryList &CFsCommonUtil::getLogCategories()
            {
                static const CLogCategoryList cats({ CLogCategory::validation(), CLogCategory::driver() });
                return cats;
            }

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
                if (fsxPath.isEmpty()) { return {}; }
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
                if (dir.isEmpty()) { return {}; }
                return CFsCommonUtil::fsxSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::fsxSimObjectsDir()
            {
                static const QString dir(fsxSimObjectsDirImpl());
                return dir;
            }

            QString CFsCommonUtil::fsxSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
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
                if (p3dPath.isEmpty()) { return {}; }
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
                if (dir.isEmpty()) { return {}; }
                return CFsCommonUtil::fsxSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::p3dSimObjectsDir()
            {
                static const QString dir(p3dSimObjectsDirImpl());
                return dir;
            }

            QStringList CFsCommonUtil::p3dSimObjectsDirPlusAddOnSimObjectsDirs(const QString &simObjectsDir)
            {
                // finding the user settings only works on P3D machine
                const QSet<QString> allP3dAddOnSimObjectPaths = CFsCommonUtil::allP3dAddOnSimObjectPaths("v4");
                QStringList all(allP3dAddOnSimObjectPaths.toList());
                all.push_front(simObjectsDir.isEmpty() ? p3dSimObjectsDir() : simObjectsDir);
                all.sort(Qt::CaseInsensitive);
                return all;
            }

            QString CFsCommonUtil::p3dSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
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
                if (fs9Path.isEmpty()) { return {}; }
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
                if (dir.isEmpty()) { return {}; }
                return CFsCommonUtil::fs9AircraftDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::fs9AircraftDir()
            {
                static const QString dir(fs9AircraftDirImpl());
                return dir;
            }

            QString CFsCommonUtil::fs9AircraftDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
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

                const QString lastSegment = u'/' % CFileUtils::lastPathSegment(simObjectsDirectoryFix) % u'/';
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
                messages.clear();
                if (!CDirectoryUtils::existsUnemptyDirectory(CDirectoryUtils::shareTerrainProbeDirectory()))
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("No terrain probe source files in '%1'").arg(CDirectoryUtils::shareTerrainProbeDirectory())));
                    return -1;
                }

                if (simObjectDir.isEmpty())
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, "No simObject directory"));
                    return -1;
                }

                QString targetDir = CFileUtils::appendFilePathsAndFixUnc(simObjectDir, "Misc");
                QDir td(targetDir);
                if (!td.exists())
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("Cannot access target directory '%1'").arg(targetDir)));
                    return -1;
                }

                const QString lastSegment = CFileUtils::lastPathSegment(CDirectoryUtils::shareTerrainProbeDirectory());
                targetDir = CFileUtils::appendFilePathsAndFixUnc(targetDir, lastSegment);
                const bool hasDir = td.mkpath(targetDir);
                if (!hasDir)
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("Cannot create target directory '%1'").arg(targetDir)));
                    return -1;
                }

                const int copied = CDirectoryUtils::copyDirectoryRecursively(CDirectoryUtils::shareTerrainProbeDirectory(), targetDir, true);
                messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityInfo, QStringLiteral("Copied %1 files from '%2' to '%3'").arg(copied).arg(CDirectoryUtils::shareTerrainProbeDirectory(), targetDir)));
                return copied;
            }

            QSet<QString> CFsCommonUtil::findP3dAddOnConfigFiles(const QString &versionHint)
            {
                const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
                QSet<QString> files;
                for (const QString &path : locations)
                {
                    QString pathUp = CFileUtils::appendFilePaths(CFileUtils::pathUp(path), "Lockheed Martin");
                    const QDir d(pathUp);
                    if (!d.exists()) { continue; }

                    // all version sub directories
                    // looking for "add-ons.cfg"
                    static const QString cfgFile("add-ons.cfg");
                    const QStringList entries = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                    for (const QString &entry : entries)
                    {
                        // right version or just one file
                        if (entry.contains(versionHint, Qt::CaseInsensitive))
                        {
                            const QString f = CFileUtils::appendFilePaths(d.absolutePath(), entry, cfgFile);
                            const QFileInfo fi(f);
                            if (fi.exists()) { files.insert(f); }
                        }
                    }
                }
                return files;
            }

            QSet<QString> CFsCommonUtil::allP3dAddOnPaths(const QStringList &addOnConfigsFiles, bool checked)
            {
                if (addOnConfigsFiles.isEmpty()) { return QSet<QString>(); }
                QSet<QString> paths;
                for (const QString &configFile : addOnConfigsFiles)
                {
                    // manually parsing because QSettings did not work properly
                    const QString fileContent = CFileUtils::readFileToString(configFile);
                    if (fileContent.isEmpty()) { continue; }
                    const QList<QStringRef> lines = splitLinesRefs(fileContent);
                    static const QString p("PATH=");
                    for (const QStringRef &line : lines)
                    {
                        const int i = line.lastIndexOf(p, Qt::CaseInsensitive);
                        if (i < 0 || line.endsWith('=')) { continue; }
                        const QStringRef path = line.mid(i + p.length());
                        const QDir dir(QDir::fromNativeSeparators(path.toString()));
                        if (!checked || dir.exists()) { paths.insert(dir.absolutePath()); }
                    }
                }
                return paths;
            }

            QSet<QString> CFsCommonUtil::allP3dAddOnSimObjectPaths(const QStringList &addOnPaths, bool checked)
            {
                if (addOnPaths.isEmpty()) { return QSet<QString>(); }
                QSet<QString> simObjectPaths;
                for (const QString &addOnPath : addOnPaths)
                {
                    const QString filename = CFileUtils::appendFilePaths(addOnPath, "add-on.xml");
                    QDomDocument doc;
                    QFile file(filename);
                    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) { continue; }
                    const QDomNodeList components = doc.elementsByTagName("AddOn.Component");
                    for (int i = 0; i < components.size(); i++)
                    {
                        const QDomNode component = components.item(i);
                        const QDomElement category = component.firstChildElement("Category");
                        const QString categoryValue = category.text();
                        if (!caseInsensitiveStringCompare(categoryValue, QStringLiteral("SimObjects"))) { continue; }
                        const QDomElement path = component.firstChildElement("Path");
                        const QString pathValue = path.text();
                        if (pathValue.isEmpty()) { continue; }
                        if (!checked || QDir(pathValue).exists()) { simObjectPaths.insert(CFileUtils::normalizeFilePathToQtStandard(pathValue)); }
                    }
                }
                return simObjectPaths;
            }

            QSet<QString> CFsCommonUtil::allP3dAddOnSimObjectPaths(const QString &versionHint)
            {
                const QStringList configFiles = CFsCommonUtil::findP3dAddOnConfigFiles(versionHint).toList();
                const QStringList addOnPaths = CFsCommonUtil::allP3dAddOnPaths(configFiles, true).toList();
                const QSet<QString> all = CFsCommonUtil::allP3dAddOnSimObjectPaths(addOnPaths, true);
                return all;
            }

            QStringList CFsCommonUtil::findFsxConfigFiles()
            {
                const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
                QStringList files;
                for (const QString &path : locations)
                {
                    const QString file = CFileUtils::appendFilePaths(CFileUtils::pathUp(path), "Microsoft/FSX/fsx.cfg");
                    const QFileInfo fi(file);
                    if (fi.exists()) { files.push_back(fi.absoluteFilePath()); }
                }
                return files;
            }

            QSet<QString> CFsCommonUtil::fsxSimObjectsPaths(const QStringList &fsxFiles, bool checked)
            {
                QSet<QString> paths;
                for (const QString &fsxFile : fsxFiles)
                {
                    paths.unite(fsxSimObjectsPaths(fsxFile, checked));
                }
                return paths;
            }

            QSet<QString> CFsCommonUtil::fsxSimObjectsPaths(const QString &fsxFile, bool checked)
            {
                const QString fileContent = CFileUtils::readFileToString(fsxFile);
                if (fileContent.isEmpty()) { return QSet<QString>(); }
                const QList<QStringRef> lines = splitLinesRefs(fileContent);
                static const QString p("SimObjectPaths.");

                QSet<QString> paths;
                for (const QStringRef &line : lines)
                {
                    const int i1 = line.lastIndexOf(p, Qt::CaseInsensitive);
                    if (i1 < 0) { continue; }
                    const int i2 = line.lastIndexOf('=');
                    if (i2 < 0 || i1 >= i2 || line.endsWith('=')) { continue; }
                    const QStringRef path = line.mid(i2 + 1);
                    const QString soPath = QDir::fromNativeSeparators(path.toString());
                    const QFileInfo fi(soPath);

                    // relative or absolute paths
                    if (fi.isAbsolute() && (!checked || fi.exists()))
                    {
                        paths.insert(fi.absolutePath());
                    }
                    else
                    {
                        paths.insert(soPath);
                    }
                }
                return paths;
            }

            CStatusMessageList CFsCommonUtil::validateConfigFiles(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmpty, int stopAtFailedFiles, bool &stopped)
            {
                CAircraftModelList sorted(models);
                CStatusMessage m;
                sorted.sortByFileName();
                stopped = false;
                CStatusMessageList msgs = sorted.validateFiles(validModels, invalidModels, ignoreEmpty, stopAtFailedFiles, stopped, true);
                if (stopped || validModels.isEmpty()) { return msgs; }

                const CAircraftModelList nonFsModels = validModels.findNonFsFamilyModels();
                if (!nonFsModels.isEmpty())
                {
                    for (const CAircraftModel &model : nonFsModels)
                    {
                        m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("Removed '%1' non FS family model").arg(model.getModelStringAndDbKey()), true);
                        msgs.push_back(m);
                    }

                    const int d = validModels.removeIfNotFsFamily();
                    m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("Removed %1 non FS family models").arg(d), true);
                    msgs.push_back(m);
                }

                // all those files should work
                int removedCfgEntries = 0;
                const QSet<QString> fileNames = validModels.getAllFileNames();
                for (const QString &fileName : fileNames)
                {
                    bool ok = false;
                    const CAircraftCfgEntriesList entries  = CAircraftCfgParser::performParsingOfSingleFile(fileName, ok, msgs);
                    const QSet<QString> removeModelStrings = entries.getTitleSetUpperCase();
                    const CAircraftModelList removedModels = validModels.removeIfFileButNotInSet(fileName, removeModelStrings);
                    for (const CAircraftModel &removedModel : removedModels)
                    {
                        removedCfgEntries++;
                        m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("'%1', removed because no longer in '%2'").arg(removedModel.getModelStringAndDbKey(), removedModel.getFileName()), true);
                        msgs.push_back(m);
                    }
                    invalidModels.push_back(removedModels);
                }

                if (removedCfgEntries < 1)
                {
                    m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityInfo, QStringLiteral("Not removed any models, all OK!"), true);
                    msgs.push_back(m);
                }

                m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityInfo, QStringLiteral("cfg validation, %1 valid models").arg(validModels.size()), true);
                msgs.push_back(m);
                m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityWarning, QStringLiteral("cfg validation, %1 invalid models").arg(invalidModels.size()), true);
                msgs.push_back(m);

                return msgs;
            }
        } // namespace
    } // namespace
} // namespace
