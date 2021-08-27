/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackmisc/swiftdirectories.h"
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

            const QStringList &CFsDirectories::getLogCategories()
            {
                static const QStringList cats({ CLogCategories::validation(), CLogCategories::driver() });
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

            const QString &CFsDirectories::fsxDirFromRegistry()
            {
                static const QString fsxPath(fsxDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxDirImpl()
            {
                const QString dir(CFsDirectories::fsxDirFromRegistry());
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

            const QString &CFsDirectories::fsxDir()
            {
                static const QString dir(fsxDirImpl());
                return dir;
            }

            QString fsxSimObjectsDirFromRegistryImpl()
            {
                const QString fsxPath = CFileUtils::normalizeFilePathToQtStandard(CFsDirectories::fsxDirFromRegistry());
                if (fsxPath.isEmpty()) { return {}; }
                return CFsDirectories::fsxSimObjectsDirFromSimDir(fsxPath);
            }

            const QString &CFsDirectories::fsxSimObjectsDirFromRegistry()
            {
                static const QString fsxPath(fsxSimObjectsDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxSimObjectsDirImpl()
            {
                QString dir(CFsDirectories::fsxDir());
                if (dir.isEmpty()) { return {}; }
                return CFsDirectories::fsxSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsDirectories::fsxSimObjectsDir()
            {
                static const QString dir(fsxSimObjectsDirImpl());
                return dir;
            }

            QString CFsDirectories::fsxSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
                return CFileUtils::appendFilePaths(CFileUtils::normalizeFilePathToQtStandard(simDir), "SimObjects");
            }

            const QStringList &CFsDirectories::fsxSimObjectsExcludeDirectoryPatterns()
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

            const QString &CFsDirectories::p3dDirFromRegistry()
            {
                static const QString p3dPath = CFileUtils::normalizeFilePathToQtStandard(p3dDirFromRegistryImpl());
                return p3dPath;
            }

            QString p3dDirImpl()
            {
                QString dir(CFsDirectories::p3dDirFromRegistry());
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

            const QString &CFsDirectories::p3dDir()
            {
                static const QString dir(p3dDirImpl());
                return dir;
            }

            QString p3dSimObjectsDirFromRegistryImpl()
            {
                const QString p3dPath = CFsDirectories::p3dDirFromRegistry();
                if (p3dPath.isEmpty()) { return {}; }
                return CFsDirectories::fsxSimObjectsDirFromSimDir(p3dPath);
            }

            const QString &CFsDirectories::p3dSimObjectsDirFromRegistry()
            {
                static const QString p3dPath(p3dSimObjectsDirFromRegistryImpl());
                return p3dPath;
            }

            QString p3dSimObjectsDirImpl()
            {
                QString dir(CFsDirectories::p3dDir());
                if (dir.isEmpty()) { return {}; }
                return CFsDirectories::p3dSimObjectsDirFromSimDir(dir);
            }

            const QString &CFsDirectories::p3dSimObjectsDir()
            {
                static const QString dir(p3dSimObjectsDirImpl());
                return dir;
            }

            QStringList CFsDirectories::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths(const QString &simObjectsDir)
            {
                // finding the user settings only works on P3D machine
                QStringList allPaths = CFsDirectories::allFsxSimObjectPaths().values();
                const QString sod = CFileUtils::normalizeFilePathToQtStandard(simObjectsDir.isEmpty() ? CFsDirectories::fsxSimObjectsDir() : simObjectsDir);
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

            QStringList CFsDirectories::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(const QString &simObjectsDir, const QString &versionHint)
            {
                // finding the user settings only works on P3D machine
                QStringList allPaths = CFsDirectories::allP3dAddOnXmlSimObjectPaths(versionHint).values();
                const QString sod    = CFileUtils::normalizeFilePathToQtStandard(simObjectsDir.isEmpty() ? CFsDirectories::p3dSimObjectsDir() : simObjectsDir);
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

            QString CFsDirectories::guessP3DVersion(const QString &candidate)
            {
                if (candidate.isEmpty()) { return "v5"; }
                if (candidate.contains("v5", Qt::CaseInsensitive)) { return QStringLiteral("v5"); }
                if (candidate.contains("v4", Qt::CaseInsensitive)) { return QStringLiteral("v4"); }

                if (candidate.contains("5", Qt::CaseInsensitive))  { return QStringLiteral("v5"); }
                if (candidate.contains("4", Qt::CaseInsensitive))  { return QStringLiteral("v4"); }

                return "v5"; // that is the future (in 2020)
            }

            QString CFsDirectories::p3dSimObjectsDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
                return CFileUtils::normalizeFilePathToQtStandard(CFileUtils::appendFilePaths(simDir, "SimObjects"));
            }

            const QStringList &CFsDirectories::p3dSimObjectsExcludeDirectoryPatterns()
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

            const QString &CFsDirectories::fs9DirFromRegistry()
            {
                static const QString fs9Path(fs9DirFromRegistryImpl());
                return fs9Path;
            }

            QString fs9DirImpl()
            {
                QString dir(CFsDirectories::fs9DirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                const QStringList someDefaultDirs(
                {
                    "C:/Flight Simulator 9",
                    "C:/FS9"
                });
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsDirectories::fs9Dir()
            {
                static const QString v(fs9DirImpl());
                return v;
            }

            QString fs9AircraftDirFromRegistryImpl()
            {
                QString fs9Path = CFsDirectories::fs9DirFromRegistry();
                if (fs9Path.isEmpty()) { return {}; }
                return CFsDirectories::fs9AircraftDirFromSimDir(fs9Path);
            }

            const QString &CFsDirectories::fs9AircraftDirFromRegistry()
            {
                static const QString dir(fs9AircraftDirFromRegistryImpl());
                return dir;
            }

            QString fs9AircraftDirImpl()
            {
                const QString dir(CFsDirectories::fs9Dir());
                if (dir.isEmpty()) { return {}; }
                return CFsDirectories::fs9AircraftDirFromSimDir(dir);
            }

            const QString &CFsDirectories::fs9AircraftDir()
            {
                static const QString dir(fs9AircraftDirImpl());
                return dir;
            }

            QString CFsDirectories::fs9AircraftDirFromSimDir(const QString &simDir)
            {
                if (simDir.isEmpty()) { return {}; }
                return CFileUtils::appendFilePaths(simDir, "Aircraft");
            }

            const QStringList &CFsDirectories::fs9AircraftObjectsExcludeDirectoryPatterns()
            {
                static const QStringList exclude;
                return exclude;
            }

            QSet<QString> CFsDirectories::findP3dAddOnConfigFiles(const QString &versionHint)
            {
                static const QString cfgFile("add-ons.cfg");
                return CFsDirectories::findP3dConfigFiles(cfgFile, versionHint);
            }

            QSet<QString> CFsDirectories::findP3dSimObjectsConfigFiles(const QString &versionHint)
            {
                static const QString cfgFile("simobjects.cfg");
                return CFsDirectories::findP3dConfigFiles(cfgFile, versionHint);
            }

            QSet<QString> CFsDirectories::findP3dConfigFiles(const QString &configFile, const QString &versionHint)
            {
                // locations will be swift paths, I will go one level up and then search for Lockheed Martin
                const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
                QSet<QString> files;
                for (const QString &path : locations)
                {
                    const QString pathUp = CFileUtils::appendFilePaths(CFileUtils::pathUp(path), "Lockheed Martin");
                    const QDir d(pathUp);
                    if (!d.exists()) { continue; }
                    if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"P3D config dir: '%1'") << d.absolutePath(); }

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
                                if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"P3D config file: '%1'") << f; }
                            }
                        } // contains
                    } // entries
                }
                return files;
            }

            QSet<QString> CFsDirectories::allConfigFilesPathValues(const QStringList &configFiles, bool checked, const QString &pathPrefix)
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

            QSet<QString> CFsDirectories::allP3dAddOnXmlSimObjectPaths(const QStringList &addOnPaths, bool checked)
            {
                if (addOnPaths.isEmpty()) { return QSet<QString>(); }
                QSet<QString> simObjectPaths;
                for (const QString &addOnPath : addOnPaths)
                {
                    const QString filename = CFileUtils::appendFilePaths(addOnPath, "add-on.xml");
                    QDomDocument doc;
                    QFile file(filename);
                    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) { continue; }
                    if (CFsDirectories::logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"Reading '%1' from addon path: '%2'") << file.fileName() << addOnPath; }

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
                        const QString fp = pathValue.leftRef(3).contains(':') ?
                                           pathValue :
                                           CFileUtils::appendFilePaths(addOnPath, pathValue);
                        if (CFsDirectories::logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"Testing '%1' as addon path: '%2'") << fp << addOnPath; }
                        if (!checked || QDir(fp).exists())
                        {
                            simObjectPaths.insert(CFileUtils::normalizeFilePathToQtStandard(fp));
                            if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"P3D SimObjects path: '%1'") << fp; }
                        }
                    } // components
                } // paths

                return simObjectPaths;
            }

            QSet<QString> CFsDirectories::allP3dAddOnXmlSimObjectPaths(const QString &versionHint)
            {
                // all add-ons.cfg files
                const QStringList addOnConfigFiles = CFsDirectories::findP3dAddOnConfigFiles(versionHint).values();

                // all PATH values in those files
                const QStringList addOnPaths = CFsDirectories::allConfigFilesPathValues(addOnConfigFiles, true, {}).values();

                // based on all paths of all config files search the XML files
                const QSet<QString> all = CFsDirectories::allP3dAddOnXmlSimObjectPaths(addOnPaths, true);
                return all;
            }

            QSet<QString> CFsDirectories::allFsxSimObjectPaths()
            {
                return CFsDirectories::fsxSimObjectsPaths(CFsDirectories::findFsxConfigFiles(), true);
            }

            QStringList CFsDirectories::findFsxConfigFiles()
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
                        if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"FSX config file: '%1'") << fi.absoluteFilePath(); }
                    }
                }
                return files;
            }

            QSet<QString> CFsDirectories::fsxSimObjectsPaths(const QStringList &fsxFiles, bool checked)
            {
                QSet<QString> paths;
                for (const QString &fsxFile : fsxFiles)
                {
                    paths.unite(CFsDirectories::fsxSimObjectsPaths(fsxFile, checked));
                }
                return paths;
            }

            QSet<QString> CFsDirectories::fsxSimObjectsPaths(const QString &fsxFile, bool checked)
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
                    if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"FSX SimObjects path checked: '%1' in '%2'") << line << fsxFile; }

                    // ignore exclude patterns
                    if (containsAny(soPath, CFsDirectories::fsxSimObjectsExcludeDirectoryPatterns(), Qt::CaseInsensitive)) { continue; }

                    // make absolute
                    if (!soPath.leftRef(3).contains(':')) { soPath = CFileUtils::appendFilePaths(relPath, soPath); }

                    const QDir dir(soPath); // always absolute path now
                    if (checked && !dir.exists())
                    {
                        // skip, not existing
                        if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"FSX SimObjects path skipped, not existing: '%1' in '%2'") << dir.absolutePath() << fsxFile; }
                        continue;
                    }

                    const QString afp = dir.absolutePath().toLower();
                    if (!CDirectoryUtils::containsFileInDir(afp, airFileFilter(), true))
                    {
                        if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"FSX SimObjects path: Skipping '%1' from '%2', no '%3' file") << afp << fsxFile << airFileFilter(); }
                        continue;
                    }

                    paths.insert(afp);
                    if (logConfigPathReading()) { CLogMessage(static_cast<CFsDirectories *>(nullptr)).info(u"FSX SimObjects path: '%1' from '%2'") << afp << fsxFile; }
                }
                return paths;
            }

            const QString &CFsDirectories::airFileFilter()
            {
                static const QString a("*.air");
                return a;
            }

            bool CFsDirectories::logConfigPathReading()
            {
                return true;
            }
        } // namespace
    } // namespace
} // namespace
