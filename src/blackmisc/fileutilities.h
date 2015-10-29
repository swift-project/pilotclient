/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CFILEUTILITIES_H
#define BLACKMISC_CFILEUTILITIES_H

#include "blackmiscexport.h"
#include <QObject>
#include <QDebug>

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
    };
} // ns

#endif // guard
