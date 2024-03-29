// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
