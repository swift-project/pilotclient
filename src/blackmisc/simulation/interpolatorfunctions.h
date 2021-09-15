/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORFUNCTIONS_H
#define BLACKMISC_SIMULATION_INTERPOLATORFUNCTIONS_H

namespace BlackMisc::Simulation
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
} // namespace
#endif // guard
