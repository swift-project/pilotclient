// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSDIRECTORIES_H
#define SWIFT_MISC_SIMULATION_FSDIRECTORIES_H

#include <atomic>

#include <QSet>
#include <QStringList>

#include "misc/logcategories.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation::fscommon
{
    //! FS9/FSX/P3D directories
    class SWIFT_MISC_EXPORT CFsDirectories
    {
    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CFsDirectories() = delete;

        //! FSX directory obtained from registry
        static const QString &fsxDirFromRegistry();

        //! FSX directory from different sources
        static const QString &fsxDir();

        //! FSX's simObject directory from registry
        static const QString &fsxSimObjectsDirFromRegistry();

        //! FSX's simobject dir, resolved from multiple sources
        static const QString &fsxSimObjectsDir();

        //! MSFS's simobject dir, resolved from multiple sources
        static const QString &msfsSimObjectsDir();

        //! MSFS's simobject dir, resolved from multiple sources
        static const QString &msfs2024SimObjectsDir();

        //! FSX aircraft dir, relative to simulator directory
        static QString fsxSimObjectsDirFromSimDir(const QString &simDir);

        //! Exclude directories for simObjects
        static const QStringList &fsxSimObjectsExcludeDirectoryPatterns();

        //! Exclude directories for simObjects
        static const QStringList &msfs20SimObjectsExcludeDirectoryPatterns();

        //! Exclude directories for simObjects
        static const QStringList &msfs2024SimObjectsExcludeDirectoryPatterns();

        //! FSX's simObject dir and the add on dirs
        static QStringList fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths(const QString &simObjectsDir = "");

        //! MSFS's simObject dir and the add on dirs
        static QStringList msfsSimObjectsDirPath(const QString &simObjectsDir = "");

        //! MSFS2024's simObject dir and the add on dirs
        static QStringList msfs2024SimObjectsDirPath(const QString &simObjectsDir = "");

        //! P3D's simObject dir and the add on dirs
        static QStringList p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(const QString &simObjectsDir,
                                                                       const QString &versionHint);

        //! Guess the P3D version such as v4, v5
        static QString guessP3DVersion(const QString &candidate);

        //! P3D directory obtained from registry
        static const QString &p3dDirFromRegistry();

        //! P3D directory from different sources
        static const QString &p3dDir();

        //! P3D's simObject directory from registry
        static const QString &p3dSimObjectsDirFromRegistry();

        //! P3D's simObject dir, resolved from multiple sources
        static const QString &p3dSimObjectsDir();

        //! P3D aircraft dir, relative to simulator directory
        static QString p3dSimObjectsDirFromSimDir(const QString &simDir);

        //! Exclude directories for simObjects
        static const QStringList &p3dSimObjectsExcludeDirectoryPatterns();

        //! MSFS directory from different sources
        static const QString &msfsDir();

        //! MSFS's packages dir
        static const QString &msfsPackagesDir();

        //! MSFS directory from different sources
        static const QString &msfs2024Dir();

        //! MSFS's packages dir
        static const QString &msfs2024PackagesDir();

        //! FS9 directory obtained from registry
        static const QString &fs9DirFromRegistry();

        //! FS9 directory obtained from multiple sources
        static const QString &fs9Dir();

        //! FS9's aircraft directory from registry
        static const QString &fs9AircraftDirFromRegistry();

        //! FS9's aircraft directory
        static const QString &fs9AircraftDir();

        //! FS9 aircraft dir, relative to simulator directory
        static QString fs9AircraftDirFromSimDir(const QString &simDir);

        //! Exclude directories for aircraft objects
        static const QStringList &fs9AircraftObjectsExcludeDirectoryPatterns();

        //! Find the config files (add-ons.cfg)
        //! \note "C:/Users/Joe Doe/AppData/Roaming/Lockheed Martin/Prepar3D v4"
        //! \param versionHint like "v5"
        static QSet<QString> findP3dAddOnConfigFiles(const QString &versionHint = "v5");

        //! Find the config files (simobjects.cfg)
        //! \note "C:/Users/Joe Doe/AppData/Roaming/Lockheed Martin/Prepar3D v4"
        //! \param versionHint like "v5"
        static QSet<QString> findP3dSimObjectsConfigFiles(const QString &versionHint = "v5");

        //! All PATH values from the config files
        static QSet<QString> allConfigFilesPathValues(const QStringList &configFiles, bool checked,
                                                      const QString &pathPrefix);

        //! All add-on paths from the XML add-on files "add-on.xml"
        static QSet<QString> allP3dAddOnXmlSimObjectPaths(const QStringList &addOnPaths, bool checked);

        //! All add-on paths from the XML add-on files "add-on.xml" files, use CFsCommonUtil::findP3dAddOnConfigFiles to
        //! find config files
        static QSet<QString> allP3dAddOnXmlSimObjectPaths(const QString &versionHint = "v4");

        //! Get all the SimObjects paths from all config files
        static QSet<QString> allFsxSimObjectPaths();

        // TODO TZ
        //! Get all the SimObjects paths from all config files
        // static QSet<QString> allMsfsSimObjectPaths();

        //! Find the config files (fsx.cfg)
        // C:/Users/Joe Doe/AppData/Roaming/Microsoft/FSX/fsx.cfg
        static QStringList findFsxConfigFiles();

        // TODO TZ
        //! Find the config files (fsx.cfg)
        // C:/Users/Joe Doe/AppData/Roaming/Microsoft/FSX/fsx.cfg
        // static QStringList findMsfsConfigFiles();

        //! Get all the SimObjects paths from fsx.cfg
        // SimObjectPaths.0=SimObjects\Airplanes
        static QSet<QString> fsxSimObjectsPaths(const QStringList &fsxFiles, bool checked);

        //! Get all the SimObjects paths from msfs.cfg
        // SimObjectPaths.0=SimObjects\Airplanes
        static QSet<QString> msfsSimObjectsPaths(const QStringList &msfsFiles, bool checked);

        //! Get all the SimObjects files from fsx.cfg
        // SimObjectPaths.0=SimObjects\Airplanes
        static QSet<QString> fsxSimObjectsPaths(const QString &fsxFile, bool checked);

        //! Get all the SimObjects files from fsx.cfg
        // SimObjectPaths.0=SimObjects\Airplanes
        static QSet<QString> msfsSimObjectsPaths(const QString &msfsFile, bool checked);

        //! .air file filter
        static const QString &airFileFilter();

    private:
        //! @{
        //! Utility functions
        static QSet<QString> findP3dConfigFiles(const QString &configFile, const QString &versionHint = "v5");
        //! @}

        //! Log the reading of config files
        static bool logConfigPathReading();
    };
} // namespace swift::misc::simulation::fscommon

#endif // guard
