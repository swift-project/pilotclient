/* Copyright (C) 2022
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLANTVELOCITY_H
#define BLACKMISC_SIMULATION_INTERPOLANTVELOCITY_H

#include "blackmisc/aviation/aircraftsituation.h"

namespace BlackMisc::Simulation
{
    //! Interpolant that uses velocity from visual position updates
    class CInterpolantVelocity
    {
    public:
        //! Set the time to be used for extrapolation
        void setCurrentTime(qint64 msSinceEpoch) { m_time = msSinceEpoch; }

        //! Set the situation to use for extrapolation
        void setLatestSituation(const Aviation::CAircraftSituation& situation);

        //! Is it ready to call extrapolate?
        bool isReady() const;

        //! Extrapolate situation at the given time or the one passed to setCurrentTime
        Aviation::CAircraftSituation extrapolate(qint64 time = -1);

    private:
        qint64 m_time = 0;
        Aviation::CAircraftSituation m_situation;
    };
}

#endif
