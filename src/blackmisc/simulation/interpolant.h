/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLANT_H
#define BLACKMISC_SIMULATION_INTERPOLANT_H

#include "blackmisc/simulation/interpolatorpbh.h"
#include "blackmisc/simulation/interpolantvelocity.h"

namespace BlackMisc::Simulation
{
    //! Interpolant interface
    class IInterpolant
    {
    public:
        //! "Real time" representing the interpolated situation
        qint64 getInterpolatedTime() const { return m_interpolatedTime; }

        //! Interpolator for pitch, bank, heading, groundspeed
        const CInterpolatorPbh &pbh() const { return m_pbh; }

        //! Situations available
        int getSituationsAvailable() const { return m_situationsAvailable; }

        //! Valid?
        bool isValid() const { return m_valid; }

        //! Valid?
        void setValid(bool valid) { m_valid = valid; }

        //! Is recalculated interpolant?
        bool isRecalculated() const { return m_recalculated; }

        //! Set recalculated interpolant
        void setRecalculated(bool reCalculated) { m_recalculated = reCalculated; }

        //! Get the velocity interpolant
        const CInterpolantVelocity &getVelocity() const { return m_velocity; }

        //! To be used by the velocity interpolant
        //! @{
        void setLatestSituation(const Aviation::CAircraftSituation &situation) { m_velocity.setLatestSituation(situation); }
        void setCurrentTime(qint64 msSinceEpoch) { m_velocity.setCurrentTime(msSinceEpoch); }
        //! @}

    protected:
        //! Default ctor
        IInterpolant() {}

        //! Constructor
        IInterpolant(int situationsAvailable, const CInterpolatorPbh &pbh) : m_situationsAvailable(situationsAvailable), m_pbh(pbh) {}

        //! Constructor
        IInterpolant(qint64 interpolatedTime, int situationsAvailable, const CInterpolantVelocity &velocity) : m_interpolatedTime(interpolatedTime), m_situationsAvailable(situationsAvailable), m_velocity(velocity) {}

        //! Constructor
        IInterpolant(const CInterpolantVelocity &velocity) : m_velocity(velocity) {}

        qint64 m_interpolatedTime = -1; //!< "Real time "of interpolated situation
        int m_situationsAvailable = 0;  //!< used situations
        CInterpolatorPbh m_pbh;         //!< the used PBH interpolator
        CInterpolantVelocity m_velocity;//!< the used velocity interpolant
        bool m_valid = true;            //!< valid?
        bool m_recalculated = false;    //!< recalculated interpolant
    };
} // namespace
#endif // guard
