/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CFILEUTILS_H
#define BLACKMISC_CFILEUTILS_H

#include "blackmiscexport.h"
#include <QObject>
#include <QDebug>
#include <QDir>

namespace BlackMisc
{
    /*!
     * Utility class for file operations
     */
    class BLACKMISC_EXPORT CFileUtils
    {
    public:
        //! Write string to text file
        static bool writeStringToFile(const QString &content, const QString &fileNameAndPath);

        //! Read file into string
        static QString readFileToString(const QString &fileNameAndPath);

        //! Read file into string
        static QString readFileToString(const QString &filePath, const QString &fileName);

        //! Write string to text file in background
        static bool writeStringToFileInBackground(const QString &content, const QString &fileNameAndPath);

        //! Append file paths
        //! \sa CNetworkUtils::buildUrl for URLs
        static QString appendFilePaths(const QString &path1, const QString &path2);

        //! If `sourceDir` is a directory, copies it recursively, so that `sourceDir` becomes `destinationDir`.
        //! If it is a file, just copies the file.
        static bool copyRecursively(const QString &sourceDir, const QString &destinationDir);

        //! Returns path to first file in dir which matches the optional wildcard and predicate, or empty string.
        static QString findFirstFile(const QDir &dir, bool recursive, const QString &wildcard = {}, std::function<bool(const QFileInfo &)> predicate = {});

        //! True if there exists a file in dir which matches the optional wildcard and predicate.
        static bool containsFile(const QDir &dir, bool recursive, const QString &wildcard = {}, std::function<bool(const QFileInfo &)> predicate = {});

        //! Returns path to first file in dir newer than the given time, optionally matching a wildcard, or empty string.
        static QString findFirstNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QString &wildcard = {});

        //! True if there exists a file in dir newer than the given time, optionally matching a wildcard.
        static bool containsFileNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QString &wildcard = {});

        //! Returns list of all files in dir, optionally matching a wildcard and predicate.
        static QFileInfoList enumerateFiles(const QDir &dir, bool recursive, const QString &wildcard = {}, std::function<bool(const QFileInfo &)> predicate = {});

        //! Returns path to the newest file in dir, optionally matching a wildcard, or empty string.
        static QString findNewestFile(const QDir &dir, bool recursive, const QString &wildcard = {});
    };
} // ns

#endif // guard
