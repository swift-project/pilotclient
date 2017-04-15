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
#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QSet>
#include <QDateTime>
#include <QRegularExpression>
#include <QStandardPaths>

namespace BlackMisc
{
    QString applicationDirectoryPathImpl()
    {
        QString appDirectoryString(qApp->applicationDirPath());
        if (appDirectoryString.endsWith("Contents/MacOS")) { appDirectoryString += "/../../.."; }
        QDir appDirectory(appDirectoryString);
        return appDirectory.absolutePath();
    }

    QString CDirectoryUtils::applicationDirectoryPath()
    {
        static const QString appDirectory(applicationDirectoryPathImpl());
        return appDirectory;
    }

    QString normalizedApplicationDirectoryImpl()
    {
        QString appDir = CDirectoryUtils::applicationDirectoryPath();
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

    const QString &CDirectoryUtils::swiftApplicationDataDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation), "/org.swift-project/");
        return p;
    }

    QFileInfoList CDirectoryUtils::swiftApplicationDataDirectories()
    {
        QDir swiftAppData(CDirectoryUtils::swiftApplicationDataDirectory()); // contains 1..n subdirs
        if (!swiftAppData.isReadable()) return QFileInfoList();
        return swiftAppData.entryInfoList({}, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
    }

    QStringList CDirectoryUtils::swiftApplicationDataDirectoryList(bool withoutCurrent, bool beautify)
    {
        QStringList dirs;
        const QFileInfoList directories(CDirectoryUtils::swiftApplicationDataDirectories());
        for (const QFileInfo &info : directories)
        {
            if (withoutCurrent && info.filePath().contains(normalizedApplicationDirectory(), Qt::CaseInsensitive)) continue;
            dirs.append(beautify ?
                        CDirectoryUtils::decodeNormalizedDirectory(info.fileName()) :
                        info.fileName());
        }
        return dirs;
    }

    const QString &CDirectoryUtils::swiftNormalizedApplicationDataDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(swiftApplicationDataDirectory(), normalizedApplicationDirectory());
        return p;
    }

    const QString &CDirectoryUtils::getLogDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(swiftNormalizedApplicationDataDirectory(), "/logs");
        return p;
    }

    const QString &CDirectoryUtils::getCrashpadDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(swiftNormalizedApplicationDataDirectory(), "/crashpad");
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
