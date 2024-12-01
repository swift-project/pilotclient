// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FLIGHTGEAR_FLIGHTGEARUTIL_H
#define SWIFT_MISC_SIMULATION_FLIGHTGEAR_FLIGHTGEARUTIL_H

#include <QStringList>

#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation::flightgear
{
    //! Flightgear utils
    class SWIFT_MISC_EXPORT CFlightgearUtil
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
} // namespace swift::misc::simulation::flightgear

#endif // SWIFT_MISC_SIMULATION_FLIGHTGEAR_FLIGHTGEARUTIL_H
