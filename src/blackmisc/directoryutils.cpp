/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/range.h"
#include "blackconfig/buildconfig.h"
#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QSet>
#include <QDateTime>
#include <QRegularExpression>
#include <QStandardPaths>

using namespace BlackConfig;

namespace BlackMisc
{
    QString binDirectoryImpl()
    {
        QString appDirectoryString(qApp->applicationDirPath());
        if (appDirectoryString.endsWith("Contents/MacOS")) { appDirectoryString += "/../../.."; }
        QDir appDirectory(appDirectoryString);
        return appDirectory.absolutePath();
    }

    const QString &CDirectoryUtils::binDirectory()
    {
        static const QString binDir(binDirectoryImpl());
        return binDir;
    }

    bool CDirectoryUtils::isInApplicationDirectory(const QString &path)
    {
        if (path.isEmpty()) { return false; }
        return path.contains(qApp->applicationDirPath(), CFileUtils::osFileNameCaseSensitivity());
    }

    const QString &CDirectoryUtils::pluginsDirectory()
    {
        static const QString pDir(CFileUtils::appendFilePaths(binDirectory(), "plugins"));
        return pDir;
    }

    const QString &CDirectoryUtils::audioPluginDirectory()
    {
        static const QString pDir(CFileUtils::appendFilePaths(binDirectory(), "audio"));
        return pDir;
    }

    const QString &CDirectoryUtils::getXSwiftBusBuildDirectory()
    {
        if (!CBuildConfig::isLocalDeveloperDebugBuild())
        {
            static const QString e;
            return e;
        }

        // the xswiftbus directory in out, not in dist
        static const QString bd = []
        {
            QDir dir(binDirectory());
            if (!dir.cdUp()) { return QString(); }
            if (!dir.cd("xswiftbus")) { return QString(); }
            return dir.absolutePath();
        }();
        return bd;
    }

    QString CDirectoryUtils::executableFilePath(const QString &executable)
    {
        Q_ASSERT_X(!executable.isEmpty(), Q_FUNC_INFO, "Missing executable file path");
        Q_ASSERT_X(CBuildConfig::isKnownExecutableName(executable), Q_FUNC_INFO, "Unknown exectuable");

        QString s = CFileUtils::appendFilePaths(CDirectoryUtils::binDirectory(), executable);
        if (CBuildConfig::isRunningOnMacOSPlatform())
        {
            // MacOS bundle may or may not be a bundle
            const QDir dir(s + QLatin1String(".app/Contents/MacOS"));
            if (dir.exists())
            {
                s += QLatin1String(".app/Contents/MacOS/") + executable;
            }
        }
        else if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            s += QLatin1String(".exe");
        }
        return s;
    }

    QString normalizedApplicationDirectoryImpl()
    {
        QString appDir = CDirectoryUtils::binDirectory();
        Q_ASSERT(appDir.size() > 0);
        // Remove leading '/' on Unix
        if (appDir.at(0) == '/') { appDir.remove(0, 1); }
        return QUrl::toPercentEncoding(appDir);
    }

    const QString &CDirectoryUtils::normalizedApplicationDirectory()
    {
        static const QString appDir(normalizedApplicationDirectoryImpl());
        return appDir;
    }

    bool CDirectoryUtils::isMacOSAppBundle()
    {
        static const bool appBundle = CBuildConfig::isRunningOnMacOSPlatform() &&
                                      qApp->applicationDirPath().contains("Contents/MacOS", Qt::CaseInsensitive);
        return appBundle;
    }

    const QString &CDirectoryUtils::applicationDataDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation), "/org.swift-project/");
        return p;
    }

    const QFileInfoList &CDirectoryUtils::applicationDataDirectories()
    {
        static QFileInfoList fileInfoList = currentApplicationDataDirectories();
        return fileInfoList;
    }

    QFileInfoList CDirectoryUtils::currentApplicationDataDirectories()
    {
        const QDir swiftAppData(CDirectoryUtils::applicationDataDirectory()); // contains 1..n subdirs
        if (!swiftAppData.isReadable()) { return QFileInfoList(); }
        return swiftAppData.entryInfoList({}, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
    }

    int CDirectoryUtils::applicationDataDirectoriesCount()
    {
        return CDirectoryUtils::applicationDataDirectories().size();
    }

    QStringList CDirectoryUtils::applicationDataDirectoryList(bool withoutCurrent, bool decodedDirName)
    {
        QStringList dirs;
        for (const QFileInfo &info : CDirectoryUtils::applicationDataDirectories())
        {
            if (withoutCurrent && info.filePath().contains(normalizedApplicationDirectory(), Qt::CaseInsensitive)) continue;
            dirs.append(decodedDirName ?
                        CDirectoryUtils::decodeNormalizedDirectory(info.filePath()) :
                        info.filePath());
        }
        return dirs;
    }

    const CDirectoryUtils::FilePerApplication &CDirectoryUtils::applicationDataDirectoryMapWithoutCurrentVersion()
    {
        static const FilePerApplication directories = currentApplicationDataDirectoryMapWithoutCurrentVersion();
        return directories;
    }

    CDirectoryUtils::FilePerApplication CDirectoryUtils::currentApplicationDataDirectoryMapWithoutCurrentVersion()
    {
        FilePerApplication directories;
        for (const QFileInfo &info : CDirectoryUtils::currentApplicationDataDirectories())
        {
            // check for myself (the running swift)
            if (caseInsensitiveStringCompare(info.filePath(), CDirectoryUtils::normalizedApplicationDataDirectory())) { continue; }

            // the application info will be written by each swift application started
            // so the application type will always contain that application
            const QString appInfoFile = CFileUtils::appendFilePaths(info.filePath(), CApplicationInfo::fileName());
            const QString appInfoJson = CFileUtils::readFileToString(appInfoFile);
            CApplicationInfo appInfo;
            if (appInfoJson.isEmpty())
            {
                // no JSON means the app no longer exists
                const QString exeDir = CDirectoryUtils::decodeNormalizedDirectory(info.filePath());
                appInfo.setExecutablePath(exeDir);
            }
            else
            {
                appInfo = CApplicationInfo::fromJson(appInfoJson);
            }
            appInfo.setApplicationDataDirectory(info.filePath());
            directories.insert(info.filePath(), appInfo);
        }

        return directories;
    }

    bool CDirectoryUtils::hasOtherSwiftDataDirectories()
    {
        return CDirectoryUtils::applicationDataDirectoryMapWithoutCurrentVersion().size() > 0;
    }

    const QString &CDirectoryUtils::normalizedApplicationDataDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(applicationDataDirectory(), normalizedApplicationDirectory());
        return p;
    }

    const QString &CDirectoryUtils::logDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(normalizedApplicationDataDirectory(), "/logs");
        return p;
    }

    QString getSwiftShareDirImpl()
    {
        QDir dir(CDirectoryUtils::binDirectory());
        const bool success = dir.cd("../share");
        if (success)
        {
            Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
            return dir.absolutePath();
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "missing dir");
        return {};
    }

    const QString &CDirectoryUtils::shareDirectory()
    {
        static const QString s(getSwiftShareDirImpl());
        return s;
    }

    const QString &CDirectoryUtils::shareTestDirectory()
    {
        static const QString test(CFileUtils::appendFilePaths(CDirectoryUtils::shareDirectory(), "test"));
        return test;
    }

    const QString &CDirectoryUtils::shareMiscDirectory()
    {
        static const QString misc(CFileUtils::appendFilePaths(CDirectoryUtils::shareDirectory(), "misc"));
        return misc;
    }

    const QString &CDirectoryUtils::shareTerrainProbeDirectory()
    {
        static const QString tpd(CFileUtils::appendFilePaths(CDirectoryUtils::shareDirectory(), "simulator/swiftTerrainProbe"));
        return tpd;
    }

    const QString &CDirectoryUtils::shareMatchingScriptDirectory()
    {
        static const QString ms(CFileUtils::appendFilePaths(CDirectoryUtils::shareDirectory(), "matchingscript"));
        return ms;
    }

    const QString &CDirectoryUtils::bootstrapFileName()
    {
        static const QString n("bootstrap.json");
        return n;
    }

    const QString getBootstrapResourceFileImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QFile file(QDir::cleanPath(d + QDir::separator() + "shared/bootstrap/" + CDirectoryUtils::bootstrapFileName()));
        Q_ASSERT_X(file.exists(), Q_FUNC_INFO, "missing bootstrap file");
        return QFileInfo(file).absoluteFilePath();
    }

    const QString &CDirectoryUtils::bootstrapResourceFilePath()
    {
        static const QString s(getBootstrapResourceFileImpl());
        return s;
    }

    QString getSwiftStaticDbFilesDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "shared/dbdata"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::staticDbFilesDirectory()
    {
        static const QString s(getSwiftStaticDbFilesDirImpl());
        return s;
    }

    QString getSoundFilesDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "sounds"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::soundFilesDirectory()
    {
        static const QString s(getSoundFilesDirImpl());
        return s;
    }

    QString getStylesheetsDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "qss"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::stylesheetsDirectory()
    {
        static const QString s(getStylesheetsDirImpl());
        return s;
    }

    QString getImagesDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "images"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::imagesDirectory()
    {
        static const QString s(getImagesDirImpl());
        return s;
    }

    const QString &CDirectoryUtils::imagesAirlinesDirectory()
    {
        static const QString s(QDir::cleanPath(imagesDirectory() + QDir::separator() + "airlines"));
        return s;
    }

    const QString &CDirectoryUtils::imagesFlagsDirectory()
    {
        static const QString s(QDir::cleanPath(imagesDirectory() + QDir::separator() + "flags"));
        return s;
    }

    QString getHtmlDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "html"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::htmlDirectory()
    {
        static const QString s(getHtmlDirImpl());
        return s;
    }

    QString getLegalDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "legal"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::legalDirectory()
    {
        static const QString s(getLegalDirImpl());
        return s;
    }

    const QString &CDirectoryUtils::aboutFilePath()
    {
        static const QString about = QDir::cleanPath(CDirectoryUtils::legalDirectory() + QDir::separator() + "about.html");
        return about;
    }

    QString testFilesDirImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "test"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CDirectoryUtils::testFilesDirectory()
    {
        static QString s(testFilesDirImpl());
        return s;
    }

    const QString &CDirectoryUtils::htmlTemplateFilePath()
    {
        static const QString s(htmlDirectory() + QDir::separator() + "swifttemplate.html");
        return s;
    }

    QString getDocumentationDirectoryImpl()
    {
        const QStringList pathes(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
        QString d = pathes.first();
        d = QDir::cleanPath(CFileUtils::appendFilePaths(d, "swift"));
        QDir dir(d);
        if (dir.exists()) { return dir.absolutePath(); }
        return pathes.first();
    }

    const QString &CDirectoryUtils::documentationDirectory()
    {
        static const QString d(getDocumentationDirectoryImpl());
        return d;
    }

    const QString &CDirectoryUtils::crashpadDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(normalizedApplicationDataDirectory(), "/crashpad");
        return p;
    }

    const QString &CDirectoryUtils::crashpadDatabaseDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(crashpadDirectory(), "/database");
        return p;
    }

    const QString &CDirectoryUtils::crashpadMetricsDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(crashpadDirectory(), "/metrics");
        return p;
    }

    QString CDirectoryUtils::decodeNormalizedDirectory(const QString &directory)
    {
        return QUrl::fromPercentEncoding(directory.toUtf8());
    }

    QStringList CDirectoryUtils::getRelativeSubDirectories(const QString &rootDir)
    {
        const QDir dir(rootDir);
        if (!dir.exists()) { return QStringList(); }
        return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    }

    bool CDirectoryUtils::containsFileInDir(const QString &dir, const QString &filter, bool recursively)
    {
        const QDir directory(dir);
        if (!directory.exists()) { return false; }

        const QStringList nameFilter({ filter });
        if (!directory.entryInfoList(nameFilter, QDir::Files | QDir::NoDot | QDir::NoDotDot).isEmpty()) { return true; }

        if (!recursively) { return false; }
        const QStringList relSubDirs = CDirectoryUtils::getRelativeSubDirectories(dir);
        for (const QString &relSubDir : relSubDirs)
        {
            const QString absSubDir = CFileUtils::appendFilePaths(directory.absolutePath(), relSubDir);
            if (CDirectoryUtils::containsFileInDir(absSubDir, filter, recursively)) { return true; }
        }
        return false;
    }

    QStringList CDirectoryUtils::verifyRuntimeDirectoriesAndFiles()
    {
        QStringList failed;
        QDir d(CDirectoryUtils::binDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        d = QDir(CDirectoryUtils::imagesDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        d = QDir(CDirectoryUtils::stylesheetsDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        d = QDir(CDirectoryUtils::applicationDataDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        // check if the executables are avialable
        QString fn = CDirectoryUtils::executableFilePath(CBuildConfig::swiftCoreExecutableName());
        if (!QFile::exists(fn)) { failed.append(fn); }

        fn = CDirectoryUtils::executableFilePath(CBuildConfig::swiftDataExecutableName());
        if (!QFile::exists(fn)) { failed.append(fn); }

        fn = CDirectoryUtils::executableFilePath(CBuildConfig::swiftGuiExecutableName());
        if (!QFile::exists(fn)) { failed.append(fn); }

        return failed;
    }

    bool CDirectoryUtils::existsUnemptyDirectory(const QString &testDir)
    {
        if (testDir.isEmpty()) { return false; }
        const QDir dir(testDir);
        if (!CDirectoryUtils::isDirExisting(dir)) { return false; }
        return !dir.isEmpty();
    }

    bool CDirectoryUtils::mkPathIfNotExisting(const QString &dir)
    {
        const QDir d(dir);
        if (d.exists()) { return true; }
        QDir mkDir;
        return mkDir.mkpath(dir);
    }

    QStringList CDirectoryUtils::getExistingUnemptyDirectories(const QStringList &directories)
    {
        QStringList dirs;
        for (const QString &dir : directories)
        {
            if (CDirectoryUtils::existsUnemptyDirectory(dir)) { dirs << dir; }
        }
        return dirs;
    }

    bool CDirectoryUtils::isDirExisting(const QString &path)
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            const QDir dir(path);
            return dir.exists();
        }

        // Windows
        if (!CFileUtils::isWindowsUncPath(path))
        {
            const QDir dir(path);
            return dir.exists();
        }
        const QString machine(CFileUtils::windowsUncMachine(path));
        if (!CFileUtils::canPingUncMachine(machine)) { return false; } // avoid long "hanging" if machine is switched off

        const QDir dir(path);
        const bool e = dir.exists();
        return e;
    }

    bool CDirectoryUtils::isDirExisting(const QDir &dir)
    {
        if (!CFileUtils::isWindowsUncPath(dir.absolutePath())) { return dir.exists(); }
        return CDirectoryUtils::isDirExisting(dir.absolutePath()); // check for UNC
    }

    bool CDirectoryUtils::isSameExistingDirectory(const QString &dir1, const QString &dir2)
    {
        if (dir1.isEmpty() || dir2.isEmpty()) { return false; }
        const QDir d1(dir1);
        const QDir d2(dir2);
        if (!d1.exists() || !d2.exists()) { return false; }
        return d1.absolutePath() == d2.absolutePath();
    }

    bool CDirectoryUtils::isSameOrSubDirectoryOf(const QString &testDir, const QString &dir2)
    {
        if (testDir.isEmpty() || dir2.isEmpty()) { return false; }
        const QDir d2(dir2);
        return CDirectoryUtils::isSameOrSubDirectoryOf(testDir, d2);
    }

    bool CDirectoryUtils::isSameOrSubDirectoryOf(const QString &dir1, const QDir &parentDir)
    {
        QDir d1(dir1);
        do
        {
            if (d1 == parentDir) { return true; }
        }
        while (d1.cdUp());

        // not found
        return false;
    }

    bool CDirectoryUtils::isSameOrSubDirectoryOfStringBased(const QString &testDir, const QString &parentDir)
    {
        const Qt::CaseSensitivity cs = CFileUtils::osFileNameCaseSensitivity();
        const QString td = CFileUtils::fixWindowsUncPath(QDir::cleanPath(testDir));
        const QString pd = CFileUtils::fixWindowsUncPath(QDir::cleanPath(parentDir));
        return td.contains(pd, cs);
    }

    QSet<QString> CDirectoryUtils::fileNamesToQSet(const QFileInfoList &fileInfoList)
    {
        QSet<QString> sl;
        for (const QFileInfo &info : fileInfoList)
        {
            sl.insert(info.fileName());
        }
        return sl;
    }

    QSet<QString> CDirectoryUtils::canonicalFileNamesToQSet(const QFileInfoList &fileInfoList)
    {
        QSet<QString> sl;
        for (const QFileInfo &info : fileInfoList)
        {
            sl.insert(info.canonicalFilePath());
        }
        return sl;
    }

    const QSet<QString> CDirectoryUtils::filesToCanonicalNames(const QSet<QString> &fileNames, const QSet<QString> &canonicalFileNames)
    {
        QSet<QString> found;
        if (fileNames.isEmpty()) return found;
        for (const QString &canonical : canonicalFileNames)
        {
            if (canonical.endsWith('/')) continue;
            const QString c = canonical.mid(1 + canonical.lastIndexOf('/'));
            if (fileNames.contains(c))
            {
                found.insert(canonical);
            }
        }
        return found;
    }

    int CDirectoryUtils::copyDirectoryRecursively(const QString &fromDir, const QString &toDir, bool replaceOnConflict)
    {
        QDir dir(fromDir);
        const QStringList fromFiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        if (!mkPathIfNotExisting(toDir)) { return -1; }

        int count = 0;
        for (const QString &copyFile : fromFiles)
        {
            const QString from = CFileUtils::appendFilePaths(fromDir, copyFile);
            const QString to = CFileUtils::appendFilePaths(toDir, copyFile);
            if (QFile::exists(to))
            {
                if (!replaceOnConflict) { continue; }
                if (!QFile::remove(to)) { return -1; }
            }
            if (!QFile::copy(from, to)) { return -1; }
            count++;
        }

        const QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &copyDir : subDirs)
        {
            const QString fromSubDir = CFileUtils::appendFilePaths(fromDir, copyDir);
            const QString toSubDir = CFileUtils::appendFilePaths(toDir, copyDir);
            if (!mkPathIfNotExisting(toDir)) { return -1; }

            const int c = copyDirectoryRecursively(fromSubDir, toSubDir, replaceOnConflict);
            if (c < 0) { return -1; }
            count += c;
        }
        return count;
    }

    CDirectoryUtils::DirComparison CDirectoryUtils::compareTwoDirectories(const QString &dirSource, const QString &dirTarget, bool nestedDirs)
    {
        DirComparison comp;
        const QDir d1(dirSource);
        const QDir d2(dirTarget);

        QFileInfoList dSourceList;
        QFileInfoList dTargetList;
        if (d1.exists()) { dSourceList = d1.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name); }
        if (d2.exists()) { dTargetList = d2.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name); }

        // only names
        const QSet<QString> sSourceFiles = CDirectoryUtils::fileNamesToQSet(dSourceList);
        const QSet<QString> sTargetFiles = CDirectoryUtils::fileNamesToQSet(dTargetList);

        // full paths
        const QSet<QString> sSourceCanonicalFiles = CDirectoryUtils::canonicalFileNamesToQSet(dSourceList);
        const QSet<QString> sTargetCanonicalFiles = CDirectoryUtils::canonicalFileNamesToQSet(dTargetList);

        QSet<QString> missingInTarget(sSourceFiles);
        QSet<QString> missingInSource(sTargetFiles);
        QSet<QString> sameNames(sSourceFiles);
        missingInSource.subtract(sSourceFiles);
        missingInTarget.subtract(sTargetFiles);
        sameNames.intersect(sTargetFiles);

        comp.source = sSourceCanonicalFiles;
        comp.missingInSource = CDirectoryUtils::filesToCanonicalNames(missingInSource, sSourceCanonicalFiles);
        comp.missingInTarget = CDirectoryUtils::filesToCanonicalNames(missingInTarget, sSourceCanonicalFiles);
        comp.sameNameInSource = CDirectoryUtils::filesToCanonicalNames(sameNames, sSourceCanonicalFiles);
        comp.sameNameInTarget = CDirectoryUtils::filesToCanonicalNames(sameNames, sTargetCanonicalFiles);

        Q_ASSERT_X(comp.sameNameInSource.size() == comp.sameNameInTarget.size(), Q_FUNC_INFO, "Same sets require same size");
        QSet<QString>::const_iterator targetIt = comp.sameNameInTarget.cbegin();
        for (const QString &sourceFile : as_const(comp.sameNameInSource))
        {
            const QFileInfo source(sourceFile);
            const QFileInfo target(*targetIt);
            ++targetIt; // avoid cpp check warning
            if (source.lastModified() == target.lastModified() && source.size() == target.size()) { continue; }

            if (source.lastModified() < target.lastModified())
            {
                comp.newerInTarget.insert(target.canonicalFilePath());
            }
            else
            {
                // source.lastModified() >= target.lastModified()
                comp.newerInSource.insert(source.canonicalFilePath());
            }
        }

        if (nestedDirs)
        {
            const QStringList relativeSubdirs = CDirectoryUtils::getRelativeSubDirectories(dirSource);
            if (!relativeSubdirs.isEmpty())
            {
                for (const QString &relativeSubdir : relativeSubdirs)
                {
                    const QString sourceSubdir = CFileUtils::appendFilePaths(dirSource, relativeSubdir);
                    const QString targetSubdir = CFileUtils::appendFilePaths(dirTarget, relativeSubdir);
                    const DirComparison subComparison = CDirectoryUtils::compareTwoDirectories(sourceSubdir, targetSubdir, true);
                    comp.insert(subComparison);
                }
            }
        }

        comp.ok = true;
        return comp;
    }

    void CDirectoryUtils::DirComparison::insert(const CDirectoryUtils::DirComparison &otherComparison)
    {
        source           += otherComparison.source;
        missingInSource  += otherComparison.missingInSource;
        missingInTarget  += otherComparison.missingInTarget;
        newerInSource    += otherComparison.newerInSource;
        newerInTarget    += otherComparison.newerInTarget;
        sameNameInSource += otherComparison.sameNameInSource;
        sameNameInTarget += otherComparison.sameNameInTarget;
    }
} // ns

//! \endcond
