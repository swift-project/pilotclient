/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DIRECTORYUTILS_H
#define BLACKMISC_DIRECTORYUTILS_H

#include "blackmisc/blackmiscexport.h"
#include <QSet>
#include <QString>
#include <QFileInfoList>

namespace BlackMisc
{
    /*!
     * Utility class for directory operations
     */
    class BLACKMISC_EXPORT CDirectoryUtils
    {
    public:
        //! Returns the directory of the application. In contrast to QCoreApplication::applicationDirPath()
        //! it takes Mac OS X app bundles into account and returns the directory of the bundle.
        static QString applicationDirectoryPath();

        //! swift application data directory, contains 0..n swift installation directories
        static const QString &swiftApplicationDataDirectory();

        //! swift application data sub directories
        static QFileInfoList swiftApplicationDataDirectories();

        //! swift application data sub directories
        static QStringList swiftApplicationDataDirectoryList(bool withoutCurrent = false, bool beautify = false);

        //! swift application data directory for one specific installation (a version)
        static const QString &swiftNormalizedApplicationDataDirectory();

        //! Directory for log files
        //! \remark In BlackMisc so it can also be used from BlackMisc classes
        static const QString &getLogDirectory();

        //! Directory for log files
        static const QString &getCrashpadDirectory();

        //! Virtually the inverse operation of CDirectoryUtils::normalizedApplicationDirectory
        static QString decodeNormalizedDirectory(const QString &directory);

        //! Result of directory comparison
        struct DirComparison
        {
            bool ok = false;                 //!< comparison ok
            QSet<QString> source;            //!< all source files
            QSet<QString> missingInSource;   //!< files not in source, but in target
            QSet<QString> missingInTarget;   //!< files not in target, but in source
            QSet<QString> newerInSource;     //!< file exists in target, but source is newer
            QSet<QString> newerInTarget;     //!< file in target is newer
            QSet<QString> sameNameInSource;  //!< file exists in source and target, source name
            QSet<QString> sameNameInTarget;  //!< file exists in source and target, target name
        };

        //! Compare 2 directories (only files, not subdirectories
        static DirComparison compareTwoDirectories(const QString &dirSource, const QString &dirTarget);

    private:
        //! Returns the application directory of the calling executable as normalized string.
        //! \note There is no trailing '/'.
        //! \warning The normalization rules are implementation specific and could change over time.
        static const QString &normalizedApplicationDirectory();

        //! Convert filenames to set
        static QSet<QString> fileNamesToQSet(const QFileInfoList &fileInfoList);

        //! Convert canoncial filenames to set
        static QSet<QString> canonicalFileNamesToQSet(const QFileInfoList &fileInfoList);

        //! File to canonical names
        static const QSet<QString> filesToCanonicalNames(const QSet<QString> &fileNames, const QSet<QString> &canonicalFileNames);
    };
} // ns

#endif // guard
