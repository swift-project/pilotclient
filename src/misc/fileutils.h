// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_CFILEUTILS_H
#define SWIFT_MISC_CFILEUTILS_H

#include <functional>

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QLockFile>
#include <QString>
#include <QStringList>
#include <Qt>

#include "misc/swiftmiscexport.h"

class QDateTime;

namespace swift::misc
{
    /*!
     * Utility class for file operations
     */
    class SWIFT_MISC_EXPORT CFileUtils
    {
    public:
        //! Our JSON file name appendix
        static const QString &jsonAppendix();

        //! JSON wildcard + appendix
        static const QString &jsonWildcardAppendix();

        //! Write string to text file
        static bool writeStringToFile(const QString &content, const QString &fileNameAndPath);

        //! Write string to file, with a lock so two applications can't access at the same time
        static bool writeStringToLockedFile(const QString &content, const QString &fileNameAndPath);

        //! Read file into string
        static QString readFileToString(const QString &fileNameAndPath);

        //! Read file into string, with a lock so two applications can't access at the same time
        static QString readLockedFileToString(const QString &fileNameAndPath);

        //! Read file into string
        static QString readFileToString(const QString &filePath, const QString &fileName);

        //! Read file into string, with a lock so two applications can't access at the same time
        static QString readLockedFileToString(const QString &filePath, const QString &fileName);

        //! Write byte array to file
        static bool writeByteArrayToFile(const QByteArray &data, const QString &fileNameAndPath);

        //! Append file paths
        //! \sa CNetworkUtils::buildUrl for URLs
        static QString appendFilePaths(const QString &path1, const QString &path2);

        //! Append file paths
        //! \sa CNetworkUtils::buildUrl for URLs
        static QString appendFilePathsAndFixUnc(const QString &path1, const QString &path2);

        //! Append file paths
        //! \sa CNetworkUtils::buildUrl for URLs
        static QString appendFilePaths(const QString &path1, const QString &path2, const QString &path3);

        //! Append file paths
        //! \sa CNetworkUtils::buildUrl for URLs
        static QString appendFilePathsAndFixUnc(const QString &path1, const QString &path2, const QString &path3);

        //! One path up
        static QString pathUp(const QString &path);

        //! Strip file from path a/b/c.json a/b, return path
        static QString stripFileFromPath(const QString &path);

        //! Strip first slash part "/a/b" => "a/b", "h:/foo" => foo
        static QString stripFirstSlashPart(const QString &path);

        //! Strip first slash part "/a/b" => "a/b", "h:/foo" => foo
        static QStringList stripFirstSlashParts(const QStringList &paths);

        //! Strip leading slash or drive letter "/a/b" => "a/b" "H:/Foo" => "Foo"
        static QString stripLeadingSlashOrDriveLetter(const QString &path);

        //! Strip first slash part "/a/b" => "a/b", "h:/foo" => foo
        static QStringList stripLeadingSlashOrDriveLetters(const QStringList &paths);

        //! Last path segment a/b/c => c
        static QString lastPathSegment(const QString &path);

        //! Normalize file path to Qt standard, e.g by turning \ to /
        static QString normalizeFilePathToQtStandard(const QString &filePath);

        //! Make directory paths relative to root directory
        //! \remark unlike QDir::relativePath here reltive paths are only created when a directory is a subdir of
        //! rootDirectory
        static QStringList makeDirectoriesRelative(const QStringList &directories, const QString &rootDirectory,
                                                   Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Same directories, order in list does not matter and lists are cleaned up
        static bool sameDirectories(const QStringList &dirs1, const QStringList &dirs2,
                                    Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Case sensitivity for current OS
        static Qt::CaseSensitivity osFileNameCaseSensitivity();

        //! Case sensitive file names
        static bool isFileNameCaseSensitive();

        //! Is directory path matching the exclude path?
        static bool matchesExcludeDirectory(const QString &directoryPath, const QString &excludePattern,
                                            Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Directory to be excluded?
        static bool isExcludedDirectory(const QDir &directory, const QStringList &excludeDirectories,
                                        Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Directory to be excluded?
        static bool isExcludedDirectory(const QFileInfo &fileInfo, const QStringList &excludeDirectories,
                                        Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Directory to be excluded?
        static bool isExcludedDirectory(const QString &directoryPath, const QStringList &excludeDirectories,
                                        Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Removes sub directories in list: A/B A/B/C B B/D -> A/B B returned
        static QStringList removeSubDirectories(const QStringList &directories,
                                                Qt::CaseSensitivity cs = osFileNameCaseSensitivity());

        //! Find first existing file or directory (means exists on file system)
        static QString findFirstExisting(const QStringList &filesOrDirectory);

        //! Returns path to first file in dir which matches the optional wildcard and predicate, or empty string.
        static QString findFirstFile(const QDir &dir, bool recursive, const QStringList &nameFilters = {},
                                     const QStringList &excludeDirectories = {},
                                     std::function<bool(const QFileInfo &)> predicate = {});

        //! True if there exists a file in dir which matches the optional wildcard and predicate.
        static bool containsFile(const QDir &dir, bool recursive, const QStringList &nameFilters = {},
                                 const QStringList &excludeDirectories = {},
                                 std::function<bool(const QFileInfo &)> predicate = {});

        //! Returns path to first file in dir newer than the given time, optionally matching a wildcard, or empty
        //! string.
        static QString findFirstNewerThan(const QDateTime &time, const QDir &dir, bool recursive,
                                          const QStringList &nameFilters = {},
                                          const QStringList &excludeDirectories = {});

        //! True if there exists a file in dir newer than the given time, optionally matching a wildcard.
        static bool containsFileNewerThan(const QDateTime &time, const QDir &dir, bool recursive,
                                          const QStringList &nameFilters = {},
                                          const QStringList &excludeDirectories = {});

        //! Returns list of all files in dir, optionally matching a wildcard and predicate.
        static QFileInfoList enumerateFiles(const QDir &dir, bool recursive, const QStringList &nameFilters = {},
                                            const QStringList &excludeDirectories = {},
                                            std::function<bool(const QFileInfo &)> predicate = {});

        //! Returns path to the last modifed file in dir, optionally matching a wildcard, or empty string.
        static QFileInfo findLastModified(const QDir &dir, bool recursive, const QStringList &nameFilters = {},
                                          const QStringList &excludeDirectories = {});

        //! Returns path to the last created file in dir, optionally matching a wildcard, or empty string.
        static QFileInfo findLastCreated(const QDir &dir, bool recursive, const QStringList &nameFilters = {},
                                         const QStringList &excludeDirectories = {});

        //! Get all swift executables
        static const QStringList &getSwiftExecutables();

        //! Turn paths and filenames in base names only
        static QStringList getBaseNamesOnly(const QStringList &fileNames);

        //! Turn paths and filenames in file names only
        static QStringList getFileNamesOnly(const QStringList &fileNames);

        //! Error message explaining why a QLockFile failed to lock.
        static QString lockFileError(const QLockFile &lockFile);

        //! UNC file paths on Qt start with "/", but UNC file paths only work when they start with "//"
        //! \remark On Windows starting with "/" means an UNC path, on UNIX it varies, see
        //! http://unix.stackexchange.com/a/12291/19428
        static QString fixWindowsUncPath(const QString &filePath);

        //! Fix UNC file paths
        //! \remark will do nothing on OS other than Windows
        static QStringList fixWindowsUncPaths(const QStringList &filePaths);

        //! Windows UNC path?
        static bool isWindowsUncPath(const QString &filePath);

        //! Machine in Windows UNC path
        static QString windowsUncMachine(const QString &filePath);

        //! All UNC machines from the paths
        static QSet<QString> windowsUncMachines(const QSet<QString> &paths);

        //! To Windows path using "\" delimiter
        static QString toWindowsLocalPath(const QString &path);

        //! Human readable (GB, MB, ..) file size
        static QString humanReadableFileSize(qint64 size);

        //! Executable file name appendixes
        static const QStringList &executableSuffixes();

        //! Executable file (decided by appendix)
        static bool isExecutableFile(const QString &fileName);

        //! swift installer
        static bool isSwiftInstaller(const QString &fileName);
    };
} // namespace swift::misc

#endif // guard
