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
#include "blackmisc/network/ping.h"
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
using namespace BlackMisc::Network;

namespace BlackMisc
{
    bool CDirectoryUtils::isInApplicationDirectory(const QString &path)
    {
        if (path.isEmpty()) { return false; }
        return path.contains(qApp->applicationDirPath(), CFileUtils::osFileNameCaseSensitivity());
    }

    bool CDirectoryUtils::isMacOSAppBundle()
    {
        static const bool appBundle = CBuildConfig::isRunningOnMacOSPlatform() &&
                                      qApp->applicationDirPath().contains("Contents/MacOS", Qt::CaseInsensitive);
        return appBundle;
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
        if (CBuildConfig::isRunningOnWindowsNtPlatform() && CFileUtils::isWindowsUncPath(path))
        {
            const QString machine(CFileUtils::windowsUncMachine(path));
            if (!canPingUncMachine(machine)) { return false; } // avoid long "hanging" if machine is switched off
        }
        return QDir(path).exists();
    }

    bool CDirectoryUtils::canPingUncMachine(const QString &machine)
    {
        static QMap<QString, qint64> good;
        static QMap<QString, qint64> bad;

        if (machine.isEmpty()) { return false; }
        const QString m = machine.toLower();
        if (good.contains(m)) { return true; } // good ones we only test once
        if (bad.contains(m))
        {
            const qint64 ts = bad.value(m);
            const qint64 dt = QDateTime::currentSecsSinceEpoch() - ts;
            if (dt < 1000 * 60) { return false; } // still bad

            // outdated, test again
        }

        const bool p = canPing(m);
        if (p)
        {
            good.insert(m, QDateTime::currentSecsSinceEpoch());
            bad.remove(m);
        }
        else
        {
            bad.insert(m, QDateTime::currentSecsSinceEpoch());
            good.remove(m);
        }
        return p;
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
        for (const QString &sourceFile : std::as_const(comp.sameNameInSource))
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
