/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_XPLANE_XPLANEUTIL_H
#define BLACKMISC_SIMULATION_XPLANE_XPLANEUTIL_H

#include "blackmisc/logcategories.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"
#include <QStringList>

namespace BlackMisc::Simulation::XPlane
{
    //! XPlane utils
    class BLACKMISC_EXPORT CXPlaneUtil
    {
    public:
        //! Constructor
        CXPlaneUtil() = delete;

        //! Log categories
        static const QStringList &getLogCategories();

        //! XPlane 9 directory
        static QString xplane9Dir();

        //! XPlane 10 directory
        static QString xplane10Dir();

        //! XPlane 11 directory
        static QString xplane11Dir();

        //! XPlane root directory
        //! In case more then one XPlane version is found, the path to the highest version is used
        static const QString &xplaneRootDir();

        //! Is the xplaneRootDir existing?
        static bool isXplaneRootDirExisting();

        //! XPlane plugin directory
        //! In case more then one XPlane version is found, the path to the highest version is used
        static const QString &xplanePluginDir();

        //! Plugin directory from given simulator directory
        static QString pluginDirFromRootDir(const QString &rootDir = CXPlaneUtil::xplaneRootDir());

        //! Model directories from simultaor directory
        static QStringList modelDirectoriesFromSimDir(const QString &simulatorDir);

        //! Is the xplanePluginDir existing?
        static bool isXplanePluginDirDirExisting();

        //! All sub directories of the plugin directory
        //! \remark uses CXPlaneUtil::xplanePluginPath as default
        static QStringList pluginSubdirectories(const QString &pluginDir = {});

        //! Directories with models
        static const QStringList &xplaneModelDirectories();

        //! Exclude directories for models
        static const QStringList &xplaneModelExcludeDirectoryPatterns();

        //! XSwiftBus plugin directory
        static QString xswiftbusPluginDir(const QString &xplaneRootDir = CXPlaneUtil::xplaneRootDir());

        //! XSwiftBus legacy directory
        static QString xswiftbusLegacyDir(const QString &xplaneRootDir = CXPlaneUtil::xplaneRootDir());

        //! Both directories, plugin and
        static bool hasXSwiftBusBuildAndPluginDir(const QString &xplaneRootDir);

        //! Finds conflicting plugins
        //! \remark uses CXPlaneUtil::xplanePluginPath as default
        static QStringList findConflictingPlugins(const QString &pluginDir = {});

        //! \remark uses CXPlaneUtil::xplanePluginPath as default
        static QStringList findAllXplFiles(const QString &pluginDir = {});

        //! Newer XSwiftBus build
        static bool hasNewerXSwiftBusBuild(const QString &xplaneRootDir = CXPlaneUtil::xplaneRootDir());

        //! Copy a XSwiftBus build
        //! \remark only copying in a local build environment
        static int copyXSwiftBusBuildFiles(const QString &xplaneRootDir = CXPlaneUtil::xplaneRootDir());

        //! XPlane relative plugin path
        static const QString &xplanePluginPathName();

        //! XSwiftBus path name
        static const QString &xswiftbusPathName();

        //! Filter filter for xpl files
        static const QStringList &xplFileFilter();

        //! Validate the model directories
        static CStatusMessageList validateModelDirectories(const QString &simDir, const QStringList &modelDirectories);

    private:
        //! Concatenates dirs for used OS
        static QString xplaneDir(const QString &xplaneInstallFile);
    };
} // namespace

#endif // guard
