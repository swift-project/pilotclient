// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SWIFTDIRECTORIES_H
#define SWIFT_MISC_SWIFTDIRECTORIES_H

#include <QDateTime>
#include <QDir>
#include <QFileInfoList>
#include <QMap>
#include <QSet>
#include <QString>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Locations of important directories for swift files
     */
    class SWIFT_MISC_EXPORT CSwiftDirectories
    {
    public:
        //! Returns the bin directory. On Windows/Linux this is the same directory as
        //! QCoreApplication::applicationDirPath(), but on MacOS the exceutable is
        //! located deeper in the hierarchy of the bundles
        //! \see https://dev.swift-project.org/w/dev/swiftpc/dirstructure/
        static const QString &binDirectory();

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

        //! swift application data directory for one specific installation (a version)
        //! \remark use CDirectoryUtils::applicationDataDirectory for one all swift versions
        static const QString &normalizedApplicationDataDirectory();

        //! Where resource files (static DB files, ...) etc are located
        //! \remark share not shared (do no mix)
        static const QString &shareDirectory();

        //! The test data directory
        static const QString &shareTestDirectory();

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
        //! \remark In swift::misc so it can also be used from swift::misc classes
        static const QString &logDirectory();

        //! Directory for crashpad files
        static const QString &crashpadDirectory();

        //! Directory for crashpad database files
        static const QString &crashpadDatabaseDirectory();

        //! Directory for crashpad metrics files
        static const QString &crashpadMetricsDirectory();

        //! Check if the (most important) runtime directories are available
        static QStringList verifyRuntimeDirectoriesAndFiles();

        //! File path (with file name) of file name and
        static QString soundFilePathOrDefaultPath(const QString &directory, const QString &fileName);

    private:
        //! Returns the application directory of the calling executable as normalized string.
        //! \note There is no trailing '/'.
        //! \warning The normalization rules are implementation specific and could change over time.
        static const QString &normalizedApplicationDirectory();
    };
} // namespace swift::misc

#endif // guard
