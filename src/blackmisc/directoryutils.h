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

#include "applicationinfo.h"
#include "blackmiscexport.h"
#include <QMap>
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
        //! Returns the bin directory. On Windows/Linux this is the same directory as
        //! QCoreApplication::applicationDirPath(), but on MacOS the exceutable is
        //! located deeper in the hierarchy of the bundles
        //! \see https://dev.swift-project.org/w/dev/swiftpc/dirstructure/
        static const QString &binDirectory();

        //! Plugins directory
        static const QString &pluginsDirectory();

        //! The executable file path
        static QString executableFilePath(const QString &executable);

        //! swift application data directory, contains 0..n swift installation directories
        static const QString &applicationDataDirectory();

        //! swift application data sub directories
        static const QFileInfoList &applicationDataDirectories();

        //! number of data directories (including this version)
        static int applicationDataDirectoriesCount();

        //! swift application data sub directories
        static QStringList applicationDataDirectoryList(bool withoutCurrent = false, bool decodedDirName = false);

        //! swift application data sub directories with info if available
        static QMap<QString, CApplicationInfo> applicationDataDirectoryMap(bool withoutCurrent = false);

        //! Is MacOS application bundle?
        //! \remark: Means the currently running executable is a MacOS bundle, but not all our executables are bundles on MacOS
        static bool isMacOSAppBundle();

        //! swift application data directory for one specific installation (a version)
        static const QString &normalizedApplicationDataDirectory();

        //! Where resource files (static DB files, ...) etc are located
        //! \remark share not shared (do no mix)
        static const QString &shareDirectory();

        //! Bootstrap resource file path
        static const QString &bootstrapResourceFilePath();

        //! Where static DB files are located
        static const QString &staticDbFilesDirectory();

        //! Where sound files are located
        static const QString &soundFilesDirectory();

        //! Where qss files are located
        static const QString &stylesheetsDirectory();

        //! Where images are located
        static const QString &imagesDirectory();

        //! Where airline images are located
        static const QString &imagesAirlinesDirectory();

        //! Where flags images are located
        static const QString &imagesFlagsDirectory();

        //! Where HTML files are located
        static const QString &htmlDirectory();

        //! Where Legal files are located
        static const QString &legalDirectory();

        //! The about document file location
        static const QString &aboutFilePath();

        //! Where test files are located
        static const QString &testFilesDirectory();

        //! HTML template
        static const QString &htmlTemplateFilePath();

        //! Directory where data can be stored
        static const QString &documentationDirectory();

        //! Directory for log files
        //! \remark In BlackMisc so it can also be used from BlackMisc classes
        static const QString &logDirectory();

        //! Directory for crashpad files
        static const QString &crashpadDirectory();

        //! Virtually the inverse operation of CDirectoryUtils::normalizedApplicationDirectory
        static QString decodeNormalizedDirectory(const QString &directory);

        //! All sub directories of given dir
        static QStringList getSubDirectories(const QString &rootDir);

        //! Check if the (most important) runtime directories are available
        static QStringList verifyRuntimeDirectoriesAndFiles();

        //! Exists directory and does it contains files
        static bool existsUnemptyDirectory(const QString &testDir);

        //! Get the existing directories
        static QStringList getExistingUnemptyDirectories(const QStringList &directories);

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

            //! Insert values of another comparison
            void insert(const DirComparison &otherComparison);
        };

        //! Compare 2 directories (only files, but with hierarchy)
        static DirComparison compareTwoDirectories(const QString &dirSource, const QString &dirTarget, bool nestedDirs);

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
