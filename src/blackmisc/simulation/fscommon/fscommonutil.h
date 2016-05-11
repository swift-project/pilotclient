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
            //! FS9/X utils
            class BLACKMISC_EXPORT CFsCommonUtil
            {
            public:
                //! Constructor
                CFsCommonUtil() = delete;

                //! FSX directory obtained from registry
                static QString fsxDirFromRegistry();

                //! FSX directory from different sources
                static QString fsxDir();

                //! P3D directory from different sources
                static QString p3dDir();

                //! FSX's simObject directory from registry
                static QString fsxSimObjectsDirFromRegistry();

                //! FSX's sim object dir, resolved from multiple sources
                static QString fsxSimObjectsDir();

                //! Exclude directories for simObjects
                static const QStringList &fsxSimObjectsExcludeDirectories();

                //! P3D's sim object dir, resolved from multiple sources
                static QString p3dSimObjectsDir();

                //! Exclude directories for simObjects
                static const QStringList &p3dSimObjectsExcludeDirectories();

                //! FS9 directory obtained from registry
                static QString fs9DirFromRegistry();

                //! FS9 directory obtained from multiple sources
                static QString fs9Dir();

                //! FS9's aircraft directory from registry
                static QString fs9AircraftDirFromRegistry();

                //! FS9's aircraft directory
                static QString fs9AircraftDir();

                //! Exclude directories for aircraft objects
                static const QStringList &fs9AircraftObjectsExcludeDirectories();
            };

        } // namespace
    } // namespace
} // namespace

#endif // guard
