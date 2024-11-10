// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DIRECTORYUTILS_H
#define SWIFT_MISC_DIRECTORYUTILS_H

#include "misc/swiftmiscexport.h"
#include <QMap>
#include <QSet>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QFileInfoList>

namespace swift::misc
{
    /*!
     * Utility class for directory operations
     */
    class SWIFT_MISC_EXPORT CDirectoryUtils
    {
    public:
        //! Path in application directory
        static bool isInApplicationDirectory(const QString &path);

        //! Is MacOS application bundle?
        //! \remark: Means the currently running executable is a MacOS bundle, but not all our executables are bundles on MacOS
        static bool isMacOSAppBundle();

        //! Virtually the inverse operation of CDirectoryUtils::normalizedApplicationDirectory
        static QString decodeNormalizedDirectory(const QString &directory);

        //! All sub directories of given dir
        static QStringList getRelativeSubDirectories(const QString &rootDir);

        //! Any file with filter like "*.txt"
        static bool containsFileInDir(const QString &dir, const QString &filter, bool recursively);

        //! Exists directory and does it contains files
        static bool existsUnemptyDirectory(const QString &testDir);

        //! Make directory if not already existing
        //! \remark returns true if path exists or was created
        static bool mkPathIfNotExisting(const QString &dir);

        //! Copy directory recursively
        static int copyDirectoryRecursively(const QString &fromDir, const QString &toDir, bool replaceOnConflict);

        //! Get the existing directories
        static QStringList getExistingUnemptyDirectories(const QStringList &directories);

        //! @{
        //! Directory existing? Also checking UNC paths upfront.
        //! \remark Motivation: if an UNC cannot be accessed (e.g. machine is down) it can take very long before functions like QDir respond
        //! \remark for non-UNC paths it is the same as the QDir checks
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
            bool ok = false; //!< comparison ok
            QSet<QString> source; //!< all source files
            QSet<QString> missingInSource; //!< files not in source, but in target
            QSet<QString> missingInTarget; //!< files not in target, but in source
            QSet<QString> newerInSource; //!< file exists in target, but source is newer
            QSet<QString> newerInTarget; //!< file in target is newer
            QSet<QString> sameNameInSource; //!< file exists in source and target, source name
            QSet<QString> sameNameInTarget; //!< file exists in source and target, target name

            //! Insert values of another comparison
            void insert(const DirComparison &otherComparison);
        };

        //! Compare 2 directories (only files, but with hierarchy)
        static DirComparison compareTwoDirectories(const QString &dirSource, const QString &dirTarget, bool nestedDirs);

    private:
        //! Convert filenames to set
        static QSet<QString> fileNamesToQSet(const QFileInfoList &fileInfoList);

        //! Convert canoncial filenames to set
        static QSet<QString> canonicalFileNamesToQSet(const QFileInfoList &fileInfoList);

        //! File to canonical names
        static QSet<QString> filesToCanonicalNames(const QSet<QString> &fileNames, const QSet<QString> &canonicalFileNames);

        //! Can connect the UNC machine
        static bool canPingUncMachine(const QString &machine);
    };
} // ns

#endif // guard
