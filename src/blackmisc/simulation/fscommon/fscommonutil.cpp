/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "fscommonutil.h"
#include "aircraftcfgparser.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/logmessage.h"
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
                const FsRegistryPathPair fsxRegistryPathPairs =
                {
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator - Steam Edition\\10.0"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("SetupPath") },
                    { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("SetupPath") }
                };

                for (const auto &registryPair : fsxRegistryPathPairs)
                {
                    const QSettings fsxRegistry(registryPair.first, QSettings::NativeFormat);
                    fsxPath = fsxRegistry.value(registryPair.second).toString().trimmed();

                    if (fsxPath.isEmpty()) { continue; }
                    fsxPath = CFileUtils::normalizeFilePathToQtStandard(fsxPath);

                    // if path does NOT exists we continue to search, maybe another one does
                    const QDir dir(fsxPath);
                    if (dir.exists()) { break; }
                    fsxPath.clear();
                }
                return CFileUtils::normalizeFilePathToQtStandard(fsxPath);
            }

            const QString &CFsCommonUtil::fsxDirFromRegistry()
            {
                static const QString fsxPath(fsxDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxDirImpl()
            {
                const QString dir(CFsCommonUtil::fsxDirFromRegistry());
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
                const QString fsxPath = CFileUtils::normalizeFilePathToQtStandard(CFsCommonUtil::fsxDirFromRegistry());
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
                return CFileUtils::appendFilePaths(CFileUtils::normalizeFilePathToQtStandard(simDir), "SimObjects");
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
                FsRegistryPathPair p3dRegistryPathPairs =
                {
                    // latest versions first
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v6"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v5"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v4"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v3"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v2"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_CURRENT_USER\\Software\\LockheedMartin\\Prepar3d"), QStringLiteral("AppPath") }
                };
                for (const auto &registryPair : p3dRegistryPathPairs)
                {
                    const QSettings p3dRegistry(registryPair.first, QSettings::NativeFormat);
                    p3dPath = p3dRegistry.value(registryPair.second).toString().trimmed();

                    if (p3dPath.isEmpty()) { continue; }
                    p3dPath = CFileUtils::normalizeFilePathToQtStandard(p3dPath);

                    // if path does NOT exists we continue to search, maybe another one does
                    const QDir dir(p3dPath);
                    if (dir.exists()) { break; }
                    p3dPath.clear();
                }
                return p3dPath;
            }

            const QString &CFsCommonUtil::p3dDirFromRegistry()
            {
                static const QString p3dPath = CFileUtils::normalizeFilePathToQtStandard(p3dDirFromRegistryImpl());
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
                return CFsCommonUtil::p3dSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsCommonUtil::p3dSimObjectsDir()
            {
                static const QString dir(p3dSimObjectsDirImpl());
                return dir;
            }

            QStringList CFsCommonUtil::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths(const QString &simObjectsDir)
            {
                // finding the user settings only works on P3D machine
                QStringList allPaths = CFsCommonUtil::allFsxSimObjectPaths().values();
                const QString sod = CFileUtils::normalizeFilePathToQtStandard(simObjectsDir.isEmpty() ? CFsCommonUtil::fsxSimObjectsDir() : simObjectsDir);
                if (!sod.isEmpty() && !allPaths.contains(sod, Qt::CaseInsensitive))
                {
                    // case insensitive is important here
                    allPaths.push_front(sod);
                }

                allPaths.removeAll({}); // remove all empty
                allPaths.removeDuplicates();
                allPaths.sort(Qt::CaseInsensitive);
                return allPaths;
            }

            QStringList CFsCommonUtil::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(const QString &simObjectsDir, const QString &versionHint)
            {
                // finding the user settings only works on P3D machine
                QStringList allPaths = CFsCommonUtil::allP3dAddOnXmlSimObjectPaths(versionHint).values();
                const QString sod    = CFileUtils::normalizeFilePathToQtStandard(simObjectsDir.isEmpty() ? CFsCommonUtil::p3dSimObjectsDir() : simObjectsDir);
                if (!sod.isEmpty() && !allPaths.contains(sod, Qt::CaseInsensitive))
                {
                    // case insensitive is important here
                    allPaths.push_front(sod);
                }

                allPaths.removeAll({}); // remove all empty
                allPaths.removeDuplicates();
                allPaths.sort(Qt::CaseInsensitive);
                return allPaths;
            }

            QString CFsCommonUtil::guessP3DVersion(const QString &candidate)
            {
                if (candidate.isEmpty()) { return "v5"; }
                if (candidate.contains("v5", Qt::CaseInsensitive)) { return QStringLiteral("v5"); }
                if (candidate.contains("v4", Qt::CaseInsensitive)) { return QStringLiteral("v4"); }

                if (candidate.contains("5", Qt::CaseInsensitive))  { return QStringLiteral("v5"); }
                if (candidate.contains("4", Qt::CaseInsensitive))  { return QStringLiteral("v4"); }

                return "v5"; // that is the future (in 2020)
            }

            QString CFsCommonUtil::p3dSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
                return CFileUtils::normalizeFilePathToQtStandard(CFileUtils::appendFilePaths(simDir, "SimObjects"));
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
                FsRegistryPathPair fs9RegistryPathPairs =
                {
                    { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DirectPlay\\Applications\\Microsoft Flight Simulator 2004"), QStringLiteral("AppPath") },
                    { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\DirectPlay\\Applications\\Microsoft Flight Simulator 2004"), QStringLiteral("AppPath") }
                };

                for (const auto &registryPair : fs9RegistryPathPairs)
                {
                    QSettings fs9Registry(registryPair.first, QSettings::NativeFormat);
                    fs9Path = fs9Registry.value(registryPair.second).toString().trimmed();

                    if (fs9Path.isEmpty()) { continue; }
                    fs9Path = CFileUtils::normalizeFilePathToQtStandard(fs9Path);

                    // if path does NOT exists we continue to search, maybe another one does
                    const QDir dir(fs9Path);
                    if (dir.exists()) { break; }
                    fs9Path.clear();
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
                if (simObjectsDirectory.isEmpty())        { return false; }
                if (!model.hasFileName())                 { return false; } // we can do nothing here

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
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, u"No terrain probe source files in '%1'") << CDirectoryUtils::shareTerrainProbeDirectory());
                    return -1;
                }

                if (simObjectDir.isEmpty())
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, u"No simObject directory"));
                    return -1;
                }

                QString targetDir = CFileUtils::appendFilePathsAndFixUnc(simObjectDir, "Misc");
                const QDir td(targetDir);
                if (!td.exists())
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, u"Cannot access target directory '%1'") << targetDir);
                    return -1;
                }

                const QString lastSegment = CFileUtils::lastPathSegment(CDirectoryUtils::shareTerrainProbeDirectory());
                targetDir = CFileUtils::appendFilePathsAndFixUnc(targetDir, lastSegment);
                const bool hasDir = td.mkpath(targetDir);
                if (!hasDir)
                {
                    messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, u"Cannot create target directory '%1'") << targetDir);
                    return -1;
                }

                const int copied = CDirectoryUtils::copyDirectoryRecursively(CDirectoryUtils::shareTerrainProbeDirectory(), targetDir, true);
                messages.push_back(CStatusMessage(getLogCategories(), CStatusMessage::SeverityInfo, u"Copied %1 files from '%2' to '%3'") << copied << CDirectoryUtils::shareTerrainProbeDirectory() << targetDir);
                return copied;
            }

            QSet<QString> CFsCommonUtil::findP3dAddOnConfigFiles(const QString &versionHint)
            {
                static const QString cfgFile("add-ons.cfg");
                return CFsCommonUtil::findP3dConfigFiles(cfgFile, versionHint);
            }

            QSet<QString> CFsCommonUtil::findP3dSimObjectsConfigFiles(const QString &versionHint)
            {
                static const QString cfgFile("simobjects.cfg");
                return CFsCommonUtil::findP3dConfigFiles(cfgFile, versionHint);
            }

            QSet<QString> CFsCommonUtil::findP3dConfigFiles(const QString &configFile, const QString &versionHint)
            {
                // locations will be swift paths, I will go one level up and then search for Lockheed Martin
                const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
                QSet<QString> files;
                for (const QString &path : locations)
                {
                    const QString pathUp = CFileUtils::appendFilePaths(CFileUtils::pathUp(path), "Lockheed Martin");
                    const QDir d(pathUp);
                    if (!d.exists()) { continue; }
                    if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"P3D config dir: '%1'") << d.absolutePath(); }

                    // all versions sub directories
                    // looking for "add-ons.cfg" or simobjects.cfg
                    const QStringList entries = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                    for (const QString &entry : entries)
                    {
                        // right version or just one file
                        if (entry.contains(versionHint, Qt::CaseInsensitive))
                        {
                            const QString f = CFileUtils::appendFilePaths(d.absolutePath(), entry, configFile);
                            const QFileInfo fi(f);
                            if (fi.exists())
                            {
                                files.insert(f);
                                if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"P3D config file: '%1'") << f; }
                            }
                        } // contains
                    } // entries
                }
                return files;
            }

            QSet<QString> CFsCommonUtil::allConfigFilesPathValues(const QStringList &configFiles, bool checked, const QString &pathPrefix)
            {
                if (configFiles.isEmpty()) { return QSet<QString>(); }
                QSet<QString> paths;
                for (const QString &configFile : configFiles)
                {
                    // manually parsing because QSettings did not work properly
                    const QString fileContent = CFileUtils::readFileToString(configFile);
                    if (fileContent.isEmpty()) { continue; }
                    const QList<QStringRef> lines = splitLinesRefs(fileContent);
                    static const QString p("Path=");
                    for (const QStringRef &line : lines)
                    {
                        const int i = line.lastIndexOf(p, -1, Qt::CaseInsensitive);
                        if (i < 0 || line.endsWith('=')) { continue; }
                        const QStringRef path = line.mid(i + p.length());
                        const QDir dir(QDir::fromNativeSeparators(pathPrefix.isEmpty() ? path.toString() : CFileUtils::appendFilePathsAndFixUnc(pathPrefix, path.toString())));
                        if (!checked || dir.exists()) { paths.insert(dir.absolutePath()); }
                    }
                }
                return paths;
            }

            QSet<QString> CFsCommonUtil::allP3dAddOnXmlSimObjectPaths(const QStringList &addOnPaths, bool checked)
            {
                if (addOnPaths.isEmpty()) { return QSet<QString>(); }
                QSet<QString> simObjectPaths;
                for (const QString &addOnPath : addOnPaths)
                {
                    const QString filename = CFileUtils::appendFilePaths(addOnPath, "add-on.xml");
                    QDomDocument doc;
                    QFile file(filename);
                    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) { continue; }
                    if (CFsCommonUtil::logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"Reading '%1' from addon path: '%2'") << file.fileName() << addOnPath; }

                    const QDomNodeList components = doc.elementsByTagName("AddOn.Component");
                    for (int i = 0; i < components.size(); i++)
                    {
                        const QDomNode component    = components.item(i);
                        const QDomElement category  = component.firstChildElement("Category");
                        const QString categoryValue = category.text();
                        if (!caseInsensitiveStringCompare(categoryValue, QStringLiteral("SimObjects"))) { continue; }
                        const QDomElement path  = component.firstChildElement("Path");
                        const QString pathValue = CFileUtils::normalizeFilePathToQtStandard(path.text());
                        const bool correctPath  = pathValue.contains("Airplanes", Qt::CaseInsensitive) ||
                                                  pathValue.contains("Rotorcraft", Qt::CaseInsensitive);
                        if (!correctPath) { continue; }

                        // absolute or relative path
                        const QString fp = pathValue.left(3).contains(':') ?
                                           pathValue :
                                           CFileUtils::appendFilePaths(addOnPath, pathValue);
                        if (CFsCommonUtil::logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"Testing '%1' as addon path: '%2'") << fp << addOnPath; }
                        if (!checked || QDir(fp).exists())
                        {
                            simObjectPaths.insert(CFileUtils::normalizeFilePathToQtStandard(fp));
                            if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"P3D SimObjects path: '%1'") << fp; }
                        }
                    } // components
                } // paths

                return simObjectPaths;
            }

            QSet<QString> CFsCommonUtil::allP3dAddOnXmlSimObjectPaths(const QString &versionHint)
            {
                // all add-ons.cfg files
                const QStringList addOnConfigFiles = CFsCommonUtil::findP3dAddOnConfigFiles(versionHint).values();

                // all PATH values in those files
                const QStringList addOnPaths = CFsCommonUtil::allConfigFilesPathValues(addOnConfigFiles, true, {}).values();

                // based on all paths of all config files search the XML files
                const QSet<QString> all = CFsCommonUtil::allP3dAddOnXmlSimObjectPaths(addOnPaths, true);
                return all;
            }

            QSet<QString> CFsCommonUtil::allFsxSimObjectPaths()
            {
                return CFsCommonUtil::fsxSimObjectsPaths(CFsCommonUtil::findFsxConfigFiles(), true);
            }

            QStringList CFsCommonUtil::findFsxConfigFiles()
            {
                const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
                QStringList files;
                for (const QString &path : locations)
                {
                    const QString file = CFileUtils::appendFilePaths(CFileUtils::pathUp(path), "Microsoft/FSX/fsx.cfg");
                    const QFileInfo fi(file);
                    if (fi.exists())
                    {
                        files.push_back(fi.absoluteFilePath());
                        if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"FSX config file: '%1'") << fi.absoluteFilePath(); }
                    }
                }
                return files;
            }

            QSet<QString> CFsCommonUtil::fsxSimObjectsPaths(const QStringList &fsxFiles, bool checked)
            {
                QSet<QString> paths;
                for (const QString &fsxFile : fsxFiles)
                {
                    paths.unite(CFsCommonUtil::fsxSimObjectsPaths(fsxFile, checked));
                }
                return paths;
            }

            QSet<QString> CFsCommonUtil::fsxSimObjectsPaths(const QString &fsxFile, bool checked)
            {
                const QString fileContent = CFileUtils::readFileToString(fsxFile);
                if (fileContent.isEmpty()) { return QSet<QString>(); }
                const QList<QStringRef> lines = splitLinesRefs(fileContent);
                static const QString p("SimObjectPaths.");

                const QFileInfo fsxFileInfo(fsxFile);
                const QString relPath = fsxFileInfo.absolutePath();

                QSet<QString> paths;
                for (const QStringRef &line : lines)
                {
                    const int i1 = line.lastIndexOf(p, -1, Qt::CaseInsensitive);
                    if (i1 < 0) { continue; }
                    const int i2 = line.lastIndexOf('=');
                    if (i2 < 0 || i1 >= i2 || line.endsWith('=')) { continue; }
                    const QStringRef path = line.mid(i2 + 1);
                    QString soPath = QDir::fromNativeSeparators(path.toString());
                    if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"FSX SimObjects path checked: '%1' in '%2'") << line << fsxFile; }

                    // ignore exclude patterns
                    if (containsAny(soPath, CFsCommonUtil::fsxSimObjectsExcludeDirectoryPatterns(), Qt::CaseInsensitive)) { continue; }

                    // make absolute
                    if (!soPath.left(3).contains(':')) { soPath = CFileUtils::appendFilePaths(relPath, soPath); }

                    const QDir dir(soPath); // always absolute path now
                    if (checked && !dir.exists())
                    {
                        // skip, not existing
                        if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"FSX SimObjects path skipped, not existing: '%1' in '%2'") << dir.absolutePath() << fsxFile; }
                        continue;
                    }

                    const QString afp = dir.absolutePath().toLower();
                    if (!CDirectoryUtils::containsFileInDir(afp, airFileFilter(), true))
                    {
                        if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"FSX SimObjects path: Skipping '%1' from '%2', no '%3' file") << afp << fsxFile << airFileFilter(); }
                        continue;
                    }

                    paths.insert(afp);
                    if (logConfigPathReading()) { CLogMessage(getLogCategories()).info(u"FSX SimObjects path: '%1' from '%2'") << afp << fsxFile; }
                }
                return paths;
            }

            CStatusMessageList CFsCommonUtil::validateAircraftConfigFiles(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped)
            {
                CStatusMessage m;
                CAircraftModelList sorted(models);
                sorted.sortByFileName();
                wasStopped = false;
                CStatusMessageList msgs = sorted.validateFiles(validModels, invalidModels, ignoreEmptyFileNames, stopAtFailedFiles, wasStopped, QString(), true);
                if (wasStopped || validModels.isEmpty()) { return msgs; }

                const CAircraftModelList nonFsModels = validModels.findNonFsFamilyModels();
                if (!nonFsModels.isEmpty())
                {
                    for (const CAircraftModel &model : nonFsModels)
                    {
                        m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("Removed '%1' non FS family model").arg(model.getModelStringAndDbKey()), true);
                        msgs.push_back(m);
                        if (wasStopped) { break; } // allow to break from "outside"
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
                    if (wasStopped) { break; } // allow to break from "outside"
                    const CAircraftCfgEntriesList entries  = CAircraftCfgParser::performParsingOfSingleFile(fileName, ok, msgs);
                    const QSet<QString> removeModelStrings = entries.getTitleSetUpperCase();
                    const CAircraftModelList removedModels = validModels.removeIfFileButNotInSet(fileName, removeModelStrings);
                    for (const CAircraftModel &removedModel : removedModels)
                    {
                        removedCfgEntries++;
                        m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, QStringLiteral("'%1' removed because no longer in '%2'").arg(removedModel.getModelStringAndDbKey(), removedModel.getFileName()), true);
                        msgs.push_back(m);
                        CAircraftModelList::addAsValidOrInvalidModel(removedModel, false, validModels, invalidModels);
                    }
                }

                if (removedCfgEntries < 1)
                {
                    m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityInfo, QStringLiteral("Not removed any models, all OK!"), true);
                    msgs.push_back(m);
                }

                if (!validModels.isEmpty())
                {
                    m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityInfo, QStringLiteral("cfg validation, valid models: %1").arg(validModels.size()), true);
                    msgs.push_back(m);
                }
                if (!invalidModels.isEmpty())
                {
                    m = CStatusMessage(getLogCategories(), CStatusMessage::SeverityWarning, QStringLiteral("cfg validation, invalid models: %1").arg(invalidModels.size()), true);
                    msgs.push_back(m);
                }

                // finished
                return msgs;
            }

            CStatusMessageList CFsCommonUtil::validateP3DSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir)
            {
                const QString simObjectsDir = simulatorDir.isEmpty() ? CFsCommonUtil::p3dSimObjectsDir() : CFsCommonUtil::p3dSimObjectsDirFromSimDir(simulatorDir);
                const QStringList simObjectPaths = CFsCommonUtil::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(simObjectsDir, guessP3DVersion(simObjectsDir));
                return CFsCommonUtil::validateSimObjectsPath(QSet<QString>(simObjectPaths.begin(), simObjectPaths.end()), models, validModels, invalidModels, ignoreEmptyFileNames, stopAtFailedFiles, wasStopped);
            }

            CStatusMessageList CFsCommonUtil::validateFSXSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &stopped, const QString &simulatorDir)
            {
                Q_UNUSED(simulatorDir)
                const QStringList simObjectPaths = CFsCommonUtil::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths();
                return CFsCommonUtil::validateSimObjectsPath(QSet<QString>(simObjectPaths.begin(), simObjectPaths.end()), models, validModels, invalidModels, ignoreEmptyFileNames, stopAtFailedFiles, stopped);
            }

            const QString CFsCommonUtil::airFileFilter()
            {
                static const QString a("*.air");
                return a;
            }

            CStatusMessageList CFsCommonUtil::validateSimObjectsPath(
                const QSet<QString> &simObjectDirs, const CAircraftModelList &models,
                CAircraftModelList  &validModels,         CAircraftModelList &invalidModels,
                bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &stopped)
            {
                CStatusMessageList msgs;
                if (simObjectDirs.isEmpty())
                {
                    msgs.push_back(CStatusMessage(getLogCategories()).validationInfo(u"No SimObject directories from cfg files, skipping validation"));
                    return msgs;
                }

                CAircraftModelList sortedModels(models);
                sortedModels.sortByFileName();
                if (sortedModels.isEmpty())
                {
                    msgs.push_back(CStatusMessage(getLogCategories()).validationInfo(u"No models to validate"));
                    return msgs;
                }

                // info
                const QString simObjDirs = joinStringSet(simObjectDirs, ", ");
                msgs.push_back(CStatusMessage(getLogCategories()).validationInfo(u"Validating %1 models against %2 SimObjects path(s): '%3'") << models.size() << simObjectDirs.size() << simObjDirs);

                // validate
                int failed = 0;
                for (const CAircraftModel &model : models)
                {
                    if (!model.hasFileName())
                    {
                        if (ignoreEmptyFileNames) { continue; }
                        msgs.push_back(CStatusMessage(getLogCategories()).validationWarning(u"No file name for model '%1'") << model.getModelString());
                        CAircraftModelList::addAsValidOrInvalidModel(model, false, validModels, invalidModels);
                        continue;
                    }

                    bool ok = false;
                    for (const QString &path : simObjectDirs)
                    {
                        if (!model.isInPath(path, CFileUtils::osFileNameCaseSensitivity())) { continue; }
                        ok = true;
                        break;
                    }
                    CAircraftModelList::addAsValidOrInvalidModel(model, ok, validModels, invalidModels);
                    if (!ok)
                    {
                        msgs.push_back(CStatusMessage(getLogCategories()).validationWarning(u"Model '%1' '%2' in none of the %3 SimObjects path(s)") << model.getModelString() << model.getFileName() << simObjectDirs.size());
                        failed++;
                    }

                    if (stopAtFailedFiles > 0 && failed >= stopAtFailedFiles)
                    {
                        stopped = true;
                        msgs.push_back(CStatusMessage(getLogCategories()).validationWarning(u"Stopping after %1 failed models") << failed);
                        break;
                    }
                } // models

                return msgs;
            }

            bool CFsCommonUtil::logConfigPathReading()
            {
                return true;
            }
        } // namespace
    } // namespace
} // namespace
