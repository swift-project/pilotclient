/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMONUTIL_H
#define BLACKMISC_SIMULATION_FSCOMMONUTIL_H

#include "blackmisc/blackmiscexport.h"

#include <QString>

class QStringList;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! FS9/X/P3D utils
            class BLACKMISC_EXPORT CFsCommonUtil
            {
            public:
                //! Constructor
                CFsCommonUtil() = delete;

                //! FSX directory obtained from registry
                static const QString &fsxDirFromRegistry();

                //! FSX directory from different sources
                static const QString &fsxDir();

                //! FSX's simObject directory from registry
                static const QString &fsxSimObjectsDirFromRegistry();

                //! FSX's simobject dir, resolved from multiple sources
                static const QString &fsxSimObjectsDir();

                //! FSX aircraft dir, relative to simulator directory
                static QString fsxSimObjectsDirFromSimDir(const QString &simDir);

                //! Exclude directories for simObjects
                static const QStringList &fsxSimObjectsExcludeDirectoryPatterns();

                //! P3D directory obtained from registry
                static const QString &p3dDirFromRegistry();

                //! P3D directory from different sources
                static const QString &p3dDir();

                //! P3D's simObject directory from registry
                static const QString &p3dSimObjectsDirFromRegistry();

                //! P3D's sim object dir, resolved from multiple sources
                static const QString &p3dSimObjectsDir();

                //! P3D aircraft dir, relative to simulator directory
                static QString p3dSimObjectsDirFromSimDir(const QString &simDir);

                //! Exclude directories for simObjects
                static const QStringList &p3dSimObjectsExcludeDirectoryPatterns();

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
            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
