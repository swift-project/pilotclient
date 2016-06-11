/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/worker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QList>
#include <QTextStream>
#include <QtGlobal>
#include <algorithm>

using namespace BlackConfig;

namespace BlackMisc
{
    const QString &CFileUtils::jsonAppendix()
    {
        static const QString j(".json");
        return j;
    }

    const QString &CFileUtils::jsonWildcardAppendix()
    {
        static const QString jw("*" + jsonAppendix());
        return jw;
    }

    bool CFileUtils::writeStringToFile(const QString &content, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return false; }
        QTextStream stream(&file);
        stream << content;
        file.close();
        return true;
    }

    QString CFileUtils::readFileToString(const QString &fileNameAndPath)
    {
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return ""; }
        QTextStream stream(&file);
        QString content(stream.readAll());
        file.close();
        return content;
    }

    QString CFileUtils::readFileToString(const QString &filePath, const QString &fileName)
    {
        return readFileToString(appendFilePaths(filePath, fileName));
    }

    bool CFileUtils::writeStringToFileInBackground(const QString &content, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        CWorker *worker = BlackMisc::CWorker::fromTask(QCoreApplication::instance(), "writeStringToFileInBackground", [content, fileNameAndPath]()
        {
            bool s = CFileUtils::writeStringToFile(content, fileNameAndPath);
            Q_UNUSED(s);
        });
        return worker ? true : false;
    }

    QString CFileUtils::appendFilePaths(const QString &path1, const QString &path2)
    {
        if (path1.isEmpty()) { return QDir::cleanPath(path2); }
        if (path2.isEmpty()) { return QDir::cleanPath(path1); }
        return QDir::cleanPath(path1 + QDir::separator() + path2);
    }

    bool CFileUtils::copyRecursively(const QString &sourceDir, const QString &destinationDir)
    {
        QFileInfo sourceFileInfo(sourceDir);
        if (sourceFileInfo.isDir())
        {
            QDir targetDir(destinationDir);
            if (!targetDir.mkpath("."))
            {
                return false;
            }

            QDir originDir(sourceFileInfo.absoluteFilePath());
            auto fileNames = originDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
            for (const QString &fileName : fileNames)
            {
                if (!copyRecursively(originDir.absoluteFilePath(fileName), targetDir.absoluteFilePath(fileName)))
                {
                    return false;
                }
            }
        }
        else
        {
            if (!QFile::copy(sourceDir, destinationDir))
            {
                return false;
            }
        }

        return true;
    }

    QString CFileUtils::normalizeFilePathToQtStandard(const QString &filePath)
    {
        if (filePath.isEmpty()) { return ""; }
        QString n(filePath);
        n = n.replace('\\', '/').replace("//", "/");
        return n;
    }

    QStringList CFileUtils::makeDirectoriesRelative(const QStringList &directories, const QString &rootDirectory, Qt::CaseSensitivity cs)
    {
        if (rootDirectory.isEmpty() || rootDirectory == "/") { return directories; }
        const QString rd(rootDirectory.endsWith('/') ? rootDirectory.left(rootDirectory.length() - 1) : rootDirectory);
        const int p = rd.length();
        QStringList relativeDirectories;
        for (const QString &dir : directories)
        {
            if (dir.startsWith(rd, cs) && dir.length() > p + 1)
            {
                relativeDirectories.append(dir.mid(p + 1));
            }
            else
            {
                relativeDirectories.append(dir); // absolute
            }
        }
        return relativeDirectories;
    }

    Qt::CaseSensitivity CFileUtils::osFileNameCaseSensitivity()
    {
        return CBuildConfig::isRunningOnWindowsNtPlatform() ? Qt::CaseInsensitive : Qt::CaseSensitive;
    }

    bool CFileUtils::matchesExcludeDirectory(const QString &directoryPath, const QString &excludeDirectory, Qt::CaseSensitivity cs)
    {
        if (directoryPath.isEmpty() || excludeDirectory.isEmpty()) { return false; }
        const QString ed(normalizeFilePathToQtStandard(excludeDirectory));
        return directoryPath.contains(ed, cs);
    }

    bool CFileUtils::isExcludedDirectory(const QDir &directory, const QStringList &excludeDirectories, Qt::CaseSensitivity cs)
    {
        if (excludeDirectories.isEmpty()) { return false; }
        const QString d = directory.absolutePath();
        return isExcludedDirectory(d, excludeDirectories, cs);
    }

    bool CFileUtils::isExcludedDirectory(const QFileInfo &fileInfo, const QStringList &excludeDirectories, Qt::CaseSensitivity cs)
    {
        if (excludeDirectories.isEmpty()) { return false; }
        return isExcludedDirectory(fileInfo.absoluteDir(), excludeDirectories, cs);
    }

    bool CFileUtils::isExcludedDirectory(const QString &directoryPath, const QStringList &excludeDirectories, Qt::CaseSensitivity cs)
    {
        if (excludeDirectories.isEmpty()) { return false; }
        for (const QString &ex : excludeDirectories)
        {
            if (matchesExcludeDirectory(directoryPath, ex, cs))
            {
                return true;
            }
        }
        return false;
    }

    QString CFileUtils::findFirstExisting(const QStringList &filesOrDirectory)
    {
        if (filesOrDirectory.isEmpty()) { return ""; }
        for (const QString &f : filesOrDirectory)
        {
            if (f.isEmpty()) { continue; }
            const QString fn(normalizeFilePathToQtStandard(f));
            const QFileInfo fi(fn);
            if (fi.exists()) { return fi.absoluteFilePath(); }
        }
        return "";
    }

    QString CFileUtils::findFirstFile(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories, std::function<bool(const QFileInfo &)> predicate)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QString(); }
        const QFileInfoList result = dir.entryInfoList(nameFilters, QDir::Files);
        if (predicate)
        {
            auto it = std::find_if(result.cbegin(), result.cend(), predicate);
            if (it != result.cend()) { return it->filePath(); }
        }
        else
        {
            if (! result.isEmpty()) { return result.first().filePath(); }
        }
        if (recursive)
        {
            for (const auto &subdir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
            {
                if (isExcludedDirectory(subdir, excludeDirectories)) { continue; }
                const QString first = findFirstFile(subdir.filePath(), true, nameFilters, excludeDirectories, predicate);
                if (! first.isEmpty()) { return first; }
            }
        }
        return {};
    }

    bool CFileUtils::containsFile(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories, std::function<bool(const QFileInfo &)> predicate)
    {
        return ! findFirstFile(dir, recursive, nameFilters, excludeDirectories, predicate).isEmpty();
    }

    QString CFileUtils::findFirstNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        return findFirstFile(dir, recursive, nameFilters, excludeDirectories, [time](const QFileInfo & fi) { return fi.lastModified() > time; });
    }

    bool CFileUtils::containsFileNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        return ! findFirstNewerThan(time, dir, recursive, nameFilters, excludeDirectories).isEmpty();
    }

    QFileInfoList CFileUtils::enumerateFiles(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories, std::function<bool(const QFileInfo &)> predicate)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QFileInfoList(); }
        QFileInfoList result = dir.entryInfoList(nameFilters, QDir::Files);
        if (predicate)
        {
            result.erase(std::remove_if(result.begin(), result.end(), std::not1(predicate)), result.end());
        }
        if (recursive)
        {
            for (const auto &subdir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
            {
                if (isExcludedDirectory(subdir, excludeDirectories)) { continue; }
                result += enumerateFiles(subdir.filePath(), true, nameFilters, excludeDirectories, predicate);
            }
        }
        return result;
    }

    QString CFileUtils::findNewestFile(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QString(); }
        const QFileInfoList files = enumerateFiles(dir, recursive, nameFilters, excludeDirectories);
        if (files.isEmpty()) { return {}; }

        auto it = std::max_element(files.cbegin(), files.cend(), [](const QFileInfo & a, const QFileInfo & b)
        {
            return a.lastModified() < b.lastModified();
        });
        return it->filePath();
    }
} // ns
