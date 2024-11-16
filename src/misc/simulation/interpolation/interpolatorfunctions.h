// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORFUNCTIONS_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORFUNCTIONS_H

namespace swift::misc::simulation
{
    //! Valid time fraction [0,1]
    inline bool isValidTimeFraction(double timeFraction)
    {
        return timeFraction >= 0.0 && timeFraction <= 1.0;
    }

    //! Valid time fraction [0,1], this allows minor overshooting
    inline bool isAcceptableTimeFraction(double timeFraction)
    {
        return timeFraction >= 0.0 && timeFraction <= 1.01;
    }

    //! Clamp time fraction [0,1]
    inline double clampValidTimeFraction(double timeFraction)
    {
        if (timeFraction > 1.0) { return 1.0; }
        if (timeFraction < 0.0) { return 0.0; }
        return timeFraction;
    }
} // namespace swift::misc::simulation
#endif // guard
