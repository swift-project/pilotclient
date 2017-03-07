/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_XPLANE_XPLANEUTIL_H
#define BLACKMISC_SIMULATION_XPLANE_XPLANEUTIL_H

#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            //! XPlane utils
            class BLACKMISC_EXPORT CXPlaneUtil
            {
            public:
                //! Constructor
                CXPlaneUtil() = delete;

                //! XPlane 9 directory
                static QString xplane9Dir();

                //! XPlane 10 directory
                static QString xplane10Dir();

                //! XPlane 11 directory
                static QString xplane11Dir();

                //! XPlane root directory
                //! In case more then one XPlane version is found, the path to the highest version is used
                static QString xplaneRootDir();

                //! Directories with models
                static QStringList xplaneModelDirectories();

                //! Exclude directories for models
                static QStringList xplaneModelExcludeDirectoryPatterns();

                //! XBus legacy directory
                static QString xbusLegacyDir(const QString &rootDir);

            private:
                //! Concatenates dirs for used OS
                static QString xplaneDir(const QString &xplaneInstallFile);
            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
