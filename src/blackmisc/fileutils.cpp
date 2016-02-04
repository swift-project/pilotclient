/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/worker.h"
#include "fileutils.h"

#include <QFile>
#include <QCoreApplication>

namespace BlackMisc
{
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
            for (const QString &fileName: fileNames)
            {
                if (!copyRecursively(originDir.absoluteFilePath(fileName), targetDir.absoluteFilePath(fileName)))
                    return false;
            }
        }
        else
        {
            if (!QFile::copy(sourceDir, destinationDir))
                return false;
        }

        return true;
    }

    QString CFileUtils::findFirstFile(const QDir &dir, bool recursive, const QString &wildcard, std::function<bool(const QFileInfo &)> predicate)
    {
        QFileInfoList result = dir.entryInfoList({ wildcard }, QDir::Files);
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
                QString first = findFirstFile(subdir.filePath(), true, wildcard, predicate);
                if (! first.isEmpty()) { return first; }
            }
        }
        return {};
    }

    bool CFileUtils::containsFile(const QDir &dir, bool recursive, const QString &wildcard, std::function<bool(const QFileInfo &)> predicate)
    {
        return ! findFirstFile(dir, recursive, wildcard, predicate).isEmpty();
    }

    QString CFileUtils::findFirstNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QString &wildcard)
    {
        return findFirstFile(dir, recursive, wildcard, [time](const QFileInfo &fi) { return fi.lastModified() > time; });
    }

    bool CFileUtils::containsFileNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QString &wildcard)
    {
        return ! findFirstNewerThan(time, dir, recursive, wildcard).isEmpty();
    }

    QFileInfoList CFileUtils::enumerateFiles(const QDir &dir, bool recursive, const QString &wildcard, std::function<bool(const QFileInfo &)> predicate)
    {
        QFileInfoList result = dir.entryInfoList({ wildcard }, QDir::Files);
        if (predicate)
        {
            result.erase(std::remove_if(result.begin(), result.end(), std::not1(predicate)), result.end());
        }
        if (recursive)
        {
            for (const auto &subdir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
            {
                result += enumerateFiles(subdir.filePath(), true, wildcard, predicate);
            }
        }
        return result;
    }

    QString CFileUtils::findNewestFile(const QDir &dir, bool recursive, const QString &wildcard)
    {
        const QFileInfoList files = enumerateFiles(dir, recursive, wildcard);
        if (files.isEmpty()) { return {}; }

        auto it = std::max_element(files.cbegin(), files.cend(), [](const QFileInfo &a, const QFileInfo &b)
        {
            return a.lastModified() < b.lastModified();
        });
        return it->filePath();
    }
} // ns
