/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FLIGHTGEAR_FLIGHTGEARUTIL_H
#define BLACKMISC_SIMULATION_FLIGHTGEAR_FLIGHTGEARUTIL_H

#include "blackmisc/blackmiscexport.h"

#include <QStringList>

namespace BlackMisc::Simulation::Flightgear
{
    //! Flightgear utils
    class BLACKMISC_EXPORT CFlightgearUtil
    {
    public:
        //! Constructor
        CFlightgearUtil() = delete;

        //! Flightgear root directory
        //! Currently only available for windows and if Flightgear is installed through installer.
        static const QString &flightgearRootDir();

        //! Is the flightgearRootDir existing?
        static bool isFlightgearRootDirExisting();

        //! Model directories from simulator directory
        static QStringList modelDirectoriesFromSimDir(const QString &simulatorDir);

        //! Directories with models
        static const QStringList &flightgearModelDirectories();

        //! Exclude directories for models
        static const QStringList &flightgearModelExcludeDirectoryPatterns();
    };
} // namespace

#endif // guard
