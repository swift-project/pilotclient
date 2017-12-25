/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
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

    const QString &CDirectoryUtils::pluginsDirectory()
    {
        static const QString pDir(CFileUtils::appendFilePaths(binDirectory(), "plugins"));
        return pDir;
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
        static const QFileInfoList fileInfoList([]
        {
            const QDir swiftAppData(CDirectoryUtils::applicationDataDirectory()); // contains 1..n subdirs
            if (!swiftAppData.isReadable()) return QFileInfoList();
            return swiftAppData.entryInfoList({}, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
        }());
        return fileInfoList;
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

    QMap<QString, CApplicationInfo> CDirectoryUtils::applicationDataDirectoryMap(bool withoutCurrent)
    {
        static const CApplicationInfo nullInfo;
        QMap<QString, CApplicationInfo> dirs;

        for (const QFileInfo &info : CDirectoryUtils::applicationDataDirectories())
        {
            if (withoutCurrent && info.filePath().contains(normalizedApplicationDirectory(), Qt::CaseInsensitive)) continue;
            const QString appInfoFile = CFileUtils::appendFilePaths(info.filePath(), CApplicationInfo::fileName());
            const QString appInfoJson = CFileUtils::readFileToString(appInfoFile);
            if (appInfoJson.isEmpty())
            {
                dirs.insert(info.filePath(), nullInfo);
            }
            else
            {
                const CApplicationInfo appInfo = CApplicationInfo::fromJson(appInfoJson);
                dirs.insert(info.filePath(), appInfo);
            }
        }
        return dirs;
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
        return "";
    }

    const QString &CDirectoryUtils::shareDirectory()
    {
        static const QString s(getSwiftShareDirImpl());
        return s;
    }

    const QString getBootstrapResourceFileImpl()
    {
        const QString d(CDirectoryUtils::shareDirectory());
        if (d.isEmpty()) { return ""; }
        const QFile file(QDir::cleanPath(d + QDir::separator() + "shared/bootstrap/bootstrap.json"));
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
        if (d.isEmpty()) { return ""; }
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
        if (d.isEmpty()) { return ""; }
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
        if (d.isEmpty()) { return ""; }
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
        if (d.isEmpty()) { return ""; }
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
        QStringList pathes(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
        QString d = pathes.first();
        d = QDir::cleanPath(d + QDir::separator() + "swift");
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

    QString CDirectoryUtils::decodeNormalizedDirectory(const QString &directory)
    {
        return QUrl::fromPercentEncoding(directory.toUtf8());
    }

    QStringList CDirectoryUtils::getSubDirectories(const QString &rootDir)
    {
        QDir dir(rootDir);
        if (!dir.exists()) { return QStringList(); }
        return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
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
        if (!dir.exists()) { return false; }
        return !dir.isEmpty();
    }

    QStringList CDirectoryUtils::getExistingUnemptyDirectories(const QStringList &directories)
    {
        QStringList dirs;
        for (const QString &dir : directories)
        {
            if (existsUnemptyDirectory(dir)) { dirs << dir; }
        }
        return dirs;
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
            const QFileInfo target(*targetIt++);
            if (source.lastModified() == target.lastModified() && source.size() == target.size())
            {
                // same
            }
            else if (source.lastModified() < target.lastModified())
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
            const QStringList relativeSubdirs = CDirectoryUtils::getSubDirectories(dirSource);
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
