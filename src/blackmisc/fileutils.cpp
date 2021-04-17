/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QFlags>
#include <QIODevice>
#include <QList>
#include <QLockFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QtGlobal>
#include <QMap>
#include <algorithm>
#include <QStringBuilder>

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

    bool CFileUtils::writeByteArrayToFile(const QByteArray &data, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::WriteOnly)) { return false; }
        const qint64 c = file.write(data);
        file.close();
        return c == data.count();
    }

    bool CFileUtils::writeStringToLockedFile(const QString &content, const QString &fileNameAndPath)
    {
        QLockFile lock(fileNameAndPath + ".lock");
        lock.lock();
        return writeStringToFile(content, fileNameAndPath);
    }

    QString CFileUtils::readFileToString(const QString &fileNameAndPath)
    {
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return {}; }
        QTextStream stream(&file);
        const QString content(stream.readAll());
        file.close();
        return content;
    }

    QString CFileUtils::readLockedFileToString(const QString &fileNameAndPath)
    {
        QLockFile lock(fileNameAndPath + ".lock");
        lock.lock();
        return readFileToString(fileNameAndPath);
    }

    QString CFileUtils::readFileToString(const QString &filePath, const QString &fileName)
    {
        return readFileToString(appendFilePaths(filePath, fileName));
    }

    QString CFileUtils::readLockedFileToString(const QString &filePath, const QString &fileName)
    {
        return readLockedFileToString(appendFilePaths(filePath, fileName));
    }

    QString CFileUtils::appendFilePaths(const QString &path1, const QString &path2)
    {
        if (path1.isEmpty()) { return QDir::cleanPath(path2); }
        if (path2.isEmpty()) { return QDir::cleanPath(path1); }
        if (path1.endsWith('/'))
        {
            // avoid double /
            if (path2.startsWith('/')) { return QDir::cleanPath(path1 % path2.mid(1)); }
            return QDir::cleanPath(path1 % path2);
        }
        return QDir::cleanPath(path1 % QChar('/') % path2);
    }

    QString CFileUtils::appendFilePathsAndFixUnc(const QString &path1, const QString &path2)
    {
        static const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        return win ? CFileUtils::fixWindowsUncPath(appendFilePaths(path1, path2)) : appendFilePaths(path1, path2);
    }

    QString CFileUtils::stripFileFromPath(const QString &path)
    {
        if (path.endsWith('/'))  { return path; }
        if (!path.contains('/')) { return path; }
        return path.left(path.lastIndexOf('/'));
    }

    QString CFileUtils::stripFirstSlashPart(const QString &path)
    {
        QString p = normalizeFilePathToQtStandard(path);
        int i = p.indexOf('/');
        if (i < 0) { return p; }
        if ((i + 1) >= path.length()) { return {}; }
        return path.mid(i + 1);
    }

    QStringList CFileUtils::stripFirstSlashParts(const QStringList &paths)
    {
        QStringList stripped;
        for (const QString &path : paths)
        {
            stripped.push_back(stripFileFromPath(path));
        }
        return stripped;
    }

    QString CFileUtils::stripLeadingSlashOrDriveLetter(const QString &path)
    {
        thread_local const QRegularExpression re("^\\/+|^[a-zA-Z]:\\/*");
        QString p(path);
        return p.replace(re, "");
    }

    QStringList CFileUtils::stripLeadingSlashOrDriveLetters(const QStringList &paths)
    {
        QStringList stripped;
        for (const QString &path : paths)
        {
            stripped.push_back(stripLeadingSlashOrDriveLetter(path));
        }
        return stripped;
    }

    QString CFileUtils::lastPathSegment(const QString &path)
    {
        if (path.isEmpty()) { return {}; }
        if (path.endsWith('/'))  { return CFileUtils::lastPathSegment(path.left(path.length() - 1)); }
        if (!path.contains('/')) { return path; }
        return path.mid(path.lastIndexOf('/') + 1);
    }

    QString CFileUtils::appendFilePaths(const QString &path1, const QString &path2, const QString &path3)
    {
        return CFileUtils::appendFilePaths(CFileUtils::appendFilePaths(path1, path2), path3);
    }

    QString CFileUtils::appendFilePathsAndFixUnc(const QString &path1, const QString &path2, const QString &path3)
    {
        static const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        return win ?
               CFileUtils::fixWindowsUncPath(CFileUtils::appendFilePaths(CFileUtils::appendFilePaths(path1, path2), path3)) :
               CFileUtils::appendFilePaths(CFileUtils::appendFilePaths(path1, path2), path3);
    }

    QString CFileUtils::pathUp(const QString &path)
    {
        const int i = path.lastIndexOf('/');
        if (i < 0) { return path; }
        return path.left(i);
    }

    QString CFileUtils::normalizeFilePathToQtStandard(const QString &filePath)
    {
        if (filePath.isEmpty()) { return {}; }
        QString n = QDir::cleanPath(filePath);
        n = n.replace('\\', '/').replace("//", "/"); // should be done alreay by cleanPath, paranoia
        return n;
    }

    QStringList CFileUtils::makeDirectoriesRelative(const QStringList &directories, const QString &rootDirectory, Qt::CaseSensitivity cs)
    {
        // not using QDir::relativePath because I do not want "../xyz" paths
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

    bool CFileUtils::sameDirectories(const QStringList &dirs1, const QStringList &dirs2, Qt::CaseSensitivity cs)
    {
        // clean up
        QStringList dirs1Cleaned(dirs1);
        QStringList dirs2Cleaned(dirs2);
        dirs1Cleaned.removeAll("");
        dirs1Cleaned.removeDuplicates();
        dirs2Cleaned.removeAll("");
        dirs2Cleaned.removeDuplicates();
        if (dirs1Cleaned.size() != dirs2Cleaned.size()) { return false; }

        int d2 = 0;
        dirs1Cleaned.sort(cs);
        dirs2Cleaned.sort(cs);
        for (const QString &d1 : dirs1)
        {
            if (!stringCompare(d1, dirs2.at(d2), cs)) { return false; }
        }
        return true;
    }

    Qt::CaseSensitivity CFileUtils::osFileNameCaseSensitivity()
    {
        return CBuildConfig::isRunningOnWindowsNtPlatform() ? Qt::CaseInsensitive : Qt::CaseSensitive;
    }

    bool CFileUtils::isFileNameCaseSensitive()
    {
        return CFileUtils::osFileNameCaseSensitivity() == Qt::CaseSensitive;
    }

    bool CFileUtils::matchesExcludeDirectory(const QString &directoryPath, const QString &excludePattern, Qt::CaseSensitivity cs)
    {
        if (directoryPath.isEmpty() || excludePattern.isEmpty()) { return false; }
        const QString normalizedExcludePattern(normalizeFilePathToQtStandard(excludePattern));
        return directoryPath.contains(normalizedExcludePattern, cs);
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
            if (matchesExcludeDirectory(directoryPath, ex, cs)) { return true; }
        }
        return false;
    }

    QStringList CFileUtils::removeSubDirectories(const QStringList &directories, Qt::CaseSensitivity cs)
    {
        if (directories.size() < 2) { return directories; }
        QStringList dirs(directories);
        dirs.removeDuplicates();
        dirs.sort(cs);
        if (dirs.size() < 2) { return dirs; }

        QString last;
        QStringList result;
        for (const QString &path : std::as_const(dirs))
        {
            if (path.isEmpty()) { continue; }
            if (last.isEmpty() || !path.startsWith(last, cs))
            {
                result.append(path);
            }
            last = path;
        }
        return result;
    }

    QString CFileUtils::findFirstExisting(const QStringList &filesOrDirectory)
    {
        if (filesOrDirectory.isEmpty()) { return {}; }
        for (const QString &f : filesOrDirectory)
        {
            if (f.isEmpty()) { continue; }
            const QString fn(normalizeFilePathToQtStandard(f));
            const QFileInfo fi(fn);
            if (fi.exists()) { return fi.absoluteFilePath(); }
        }
        return {};
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
            result.erase(std::remove_if(result.begin(), result.end(), [ = ](const auto &f) { return !predicate(f); }), result.end());
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

    QFileInfo CFileUtils::findLastModified(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QString(); }
        const QFileInfoList files = enumerateFiles(dir, recursive, nameFilters, excludeDirectories);
        if (files.isEmpty()) { return {}; }

        auto it = std::max_element(files.cbegin(), files.cend(), [](const QFileInfo & a, const QFileInfo & b)
        {
            return a.lastModified() < b.lastModified();
        });
        return *it;
    }

    QFileInfo CFileUtils::findLastCreated(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QString(); }
        const QFileInfoList files = enumerateFiles(dir, recursive, nameFilters, excludeDirectories);
        if (files.isEmpty()) { return {}; }

        auto it = std::max_element(files.cbegin(), files.cend(), [](const QFileInfo & a, const QFileInfo & b)
        {
            return a.birthTime() < b.birthTime();
        });
        return *it;
    }

    const QStringList &CFileUtils::getSwiftExecutables()
    {
        static const QStringList executables(
            QFileInfo(QCoreApplication::applicationFilePath())
            .dir()
            .entryList(QDir::Executable | QDir::Files)
        );
        return executables;
    }

    QStringList CFileUtils::getBaseNamesOnly(const QStringList &fileNames)
    {
        QStringList baseNames;
        for (const QString &fn : fileNames)
        {
            const QFileInfo fi(fn);
            baseNames.push_back(fi.baseName());
        }
        return baseNames;
    }

    QStringList CFileUtils::getFileNamesOnly(const QStringList &fileNames)
    {
        QStringList fns;
        for (const QString &fn : fileNames)
        {
            const QFileInfo fi(fn);
            fns.push_back(fi.fileName());
        }
        return fns;
    }

    QString CFileUtils::lockFileError(const QLockFile &lockFile)
    {
        switch (lockFile.error())
        {
        case QLockFile::NoError:         return QStringLiteral("No error");
        case QLockFile::PermissionError: return QStringLiteral("Insufficient permission");
        case QLockFile::UnknownError:    return QStringLiteral("Unknown error");
        case QLockFile::LockFailedError:
            {
                QString hostname, appname;
                qint64 pid = 0;
                lockFile.getLockInfo(&pid, &hostname, &appname);
                return QStringLiteral("Lock open in another process (%1 %2 on %3)").arg(hostname, QString::number(pid), appname);
            }
        default: return QStringLiteral("Bad error number");
        }
    }

    QString CFileUtils::fixWindowsUncPath(const QString &filePath)
    {
        static const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        if (!win) { return filePath; }
        if (!filePath.startsWith('/')) { return filePath; }
        if (filePath.startsWith("//")) { return filePath; }
        return QStringLiteral("/%1").arg(filePath);
    }

    QStringList CFileUtils::fixWindowsUncPaths(const QStringList &filePaths)
    {
        static const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        if (!win) { return filePaths; }

        QStringList fixedPaths;
        for (const QString &path : filePaths)
        {
            fixedPaths << fixWindowsUncPath(path);
        }
        return fixedPaths;
    }

    bool CFileUtils::isWindowsUncPath(const QString &filePath)
    {
        if (filePath.startsWith("//") || filePath.startsWith("\\\\")) { return true; }
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; } // "/tmp" is valid on Unix/Mac

        // Windows here
        const QString fp = fixWindowsUncPath(filePath);
        return (fp.startsWith("//") || fp.startsWith("\\\\"));
    }

    QString CFileUtils::windowsUncMachine(const QString &filePath)
    {
        if (!CFileUtils::isWindowsUncPath(filePath)) { return {}; }
        QString f = filePath;
        f.replace("\\", "/");
        f.replace("//", "");
        if (f.startsWith("/")) { f = f.mid(1); }
        const int i = f.indexOf('/');
        if (i < 0) { return f; }
        return f.left(i);
    }

    QSet<QString> CFileUtils::windowsUncMachines(const QSet<QString> &paths)
    {
        if (paths.isEmpty()) { return {}; }

        const Qt::CaseSensitivity cs = osFileNameCaseSensitivity();
        const bool isCs = isFileNameCaseSensitive();

        QSet<QString> machines;
        QString lastMachine;

        for (const QString &p : paths)
        {
            if (!lastMachine.isEmpty() && p.contains(lastMachine, cs))
            {
                // shortcut
                continue;
            }
            const QString m = isCs ? windowsUncMachine(p) : windowsUncMachine(p).toLower();
            if (m.isEmpty()) { continue; }
            lastMachine = m;
            machines.insert(m);
        }
        return machines;
    }

    QString CFileUtils::toWindowsLocalPath(const QString &path)
    {
        QString p = CFileUtils::fixWindowsUncPath(path);
        return p.replace('/', '\\');
    }

    QString CFileUtils::humanReadableFileSize(qint64 size)
    {
        // from https://stackoverflow.com/a/30958189/356726
        // fell free to replace it by something better
        static const QStringList units({ "KB", "MB", "GB", "TB" });
        if (size <= 1024) { return QString::number(size); }

        QStringListIterator i(units);
        double currentSize = size;
        QString unit;
        while (currentSize >= 1024.0 && i.hasNext())
        {
            unit = i.next();
            currentSize /= 1024.0;
        }
        return QStringLiteral("%1 %2").arg(currentSize, 0, 'f', 2).arg(unit);
    }

    const QStringList &CFileUtils::executableSuffixes()
    {
        // incomplete list of file name appendixes
        // dmg is not a executable. It is a MacOS container. If you open it, a new virtual drive will be mapped which includes a executable.
        static const QStringList appendixes({".exe", ".dmg", ".run"});
        return appendixes;
    }

    bool CFileUtils::isExecutableFile(const QString &fileName)
    {
        for (const QString &app : CFileUtils::executableSuffixes())
        {
            if (fileName.endsWith(app, Qt::CaseInsensitive)) { return true; }
        }
        return CFileUtils::isSwiftInstaller(fileName);
    }

    bool CFileUtils::isSwiftInstaller(const QString &fileName)
    {
        if (fileName.isEmpty()) { return false; }
        return fileName.contains("swift", Qt::CaseInsensitive) && fileName.contains("installer");
    }
} // ns
