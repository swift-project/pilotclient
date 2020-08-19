/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DIRECTORYUTILS_H
#define BLACKMISC_DIRECTORYUTILS_H

#include "blackmisc/applicationinfo.h"
#include "blackmisc/blackmiscexport.h"
#include <QMap>
#include <QSet>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QFileInfoList>

namespace BlackMisc
{
    /*!
     * Utility class for directory operations
     */
    class BLACKMISC_EXPORT CDirectoryUtils
    {
    public:
        //! File path and swift application
        using FilePerApplication = QMap<QString, CApplicationInfo>;

        //! Returns the bin directory. On Windows/Linux this is the same directory as
        //! QCoreApplication::applicationDirPath(), but on MacOS the exceutable is
        //! located deeper in the hierarchy of the bundles
        //! \see https://dev.swift-project.org/w/dev/swiftpc/dirstructure/
        static const QString &binDirectory();

        //! Path in application directory
        static bool isInApplicationDirectory(const QString &path);

        //! Plugins directory
        static const QString &pluginsDirectory();

        //! Audio plugins directory for Qt audio
        //! \remark contains the audio plugins
        static const QString &audioPluginDirectory();

        //! The build directory
        //! \remark if is a local build
        static const QString &getXSwiftBusBuildDirectory();

        //! The executable file path
        static QString executableFilePath(const QString &executable);

        //! swift application data directory, contains 0..n swift installation directories
        //! \remark use CDirectoryUtils::normalizedApplicationDataDirectory for one specific version
        static const QString &applicationDataDirectory();

        //! swift application data sub directories
        static const QFileInfoList &applicationDataDirectories();

        //! number of data directories (including this version)
        static int applicationDataDirectoriesCount();

        //! swift application data sub directories
        static QFileInfoList currentApplicationDataDirectories();

        //! swift application data sub directories
        static QStringList applicationDataDirectoryList(bool withoutCurrent = false, bool decodedDirName = false);

        //! swift application data sub directories with info if available
        static const FilePerApplication &applicationDataDirectoryMapWithoutCurrentVersion();

        //! swift application data sub directories with info if available
        static FilePerApplication currentApplicationDataDirectoryMapWithoutCurrentVersion();

        //! Other swift data directories
        static bool hasOtherSwiftDataDirectories();

        //! Is MacOS application bundle?
        //! \remark: Means the currently running executable is a MacOS bundle, but not all our executables are bundles on MacOS
        static bool isMacOSAppBundle();

        //! swift application data directory for one specific installation (a version)
        //! \remark use CDirectoryUtils::applicationDataDirectory for one all swift versions
        static const QString &normalizedApplicationDataDirectory();

        //! Where resource files (static DB files, ...) etc are located
        //! \remark share not shared (do no mix)
        static const QString &shareDirectory();

        //! The test data directory
        static const QString &shareTestDirectory();

        //! The misc data directory
        static const QString &shareMiscDirectory();

        //! FSX/P3D terrain probe
        static const QString &shareTerrainProbeDirectory();

        //! Matching script examples directories
        static const QString &shareMatchingScriptDirectory();

        //! Bootstrap file name
        static const QString &bootstrapFileName();

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

        //! Directory for crashpad database files
        static const QString &crashpadDatabaseDirectory();

        //! Directory for crashpad metrics files
        static const QString &crashpadMetricsDirectory();

        //! Virtually the inverse operation of CDirectoryUtils::normalizedApplicationDirectory
        static QString decodeNormalizedDirectory(const QString &directory);

        //! All sub directories of given dir
        static QStringList getRelativeSubDirectories(const QString &rootDir);

        //! Any file with filter like "*.txt"
        static bool containsFileInDir(const QString &dir, const QString &filter, bool recursively);

        //! Check if the (most important) runtime directories are available
        static QStringList verifyRuntimeDirectoriesAndFiles();

        //! Exists directory and does it contains files
        static bool existsUnemptyDirectory(const QString &testDir);

        //! Make directory if not already existing
        //! \remark returns true if path exists or was created
        static bool mkPathIfNotExisting(const QString &dir);

        //! Copy directory recursively
        static int copyDirectoryRecursively(const QString &fromDir, const QString &toDir, bool replaceOnConflict);

        //! Get the existing directories
        static QStringList getExistingUnemptyDirectories(const QStringList &directories);

        //! Directory existing? Also checking UNC paths upfront.
        //! \remark Motivation: if an UNC cannot be accessed (e.g. machine is down) it can take very long before functions like QDir respond
        //! \remark for non-UNC paths it is the same as the QDir checks
        //! @{
        static bool isDirExisting(const QString &path);
        static bool isDirExisting(const QDir &dir);
        //! @}

        //! Same existing directories
        static bool isSameExistingDirectory(const QString &dir1, const QString &dir2);

        //! Is "testDir" a subdirectory (possibly nested) of "parentDir" or the same directory
        //! \remark testing on real directory structure
        static bool isSameOrSubDirectoryOf(const QString &testDir, const QString &parentDir);

        //! Is "testDir" a subdirectory (possibly nested) of "parentDir" or the same directory
        //! \remark testing on real directory structure
        static bool isSameOrSubDirectoryOf(const QString &testDir, const QDir &parentDir);

        //! Is "testDir" a subdirectory (possibly nested) of "parentDir" or the same directory
        //! \remark String based only
        static bool isSameOrSubDirectoryOfStringBased(const QString &testDir, const QString &parentDir);

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
