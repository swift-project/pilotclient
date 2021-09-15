/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/interpolatorpbh.h"
#include "blackmisc/simulation/interpolatorfunctions.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Simulation
{
    CAngle CInterpolatorPbh::interpolateAngle(const CAngle &begin, const CAngle &end, double timeFraction0to1)
    {
        // determine the right direction (to left, to right) we interpolate towards to
        //  -30 ->   30 =>    60 (via 0)
        //   30 ->  -30 =>   -60 (via 0)
        //  170 -> -170 =>  -340 (via 180)
        // -170 ->  170 =>   340 (via 180)
        double deltaDeg = (end - begin).value(CAngleUnit::deg());
        if (deltaDeg > 180.0) { deltaDeg -= 360; }
        else if (deltaDeg < -180.0) { deltaDeg += 360; }

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(isAcceptableTimeFraction(timeFraction0to1), Q_FUNC_INFO, "0..1 fraction needed");
        }

        //! make sure to not end up we extrapolation
        if (timeFraction0to1 >= 1.0) { return begin + CAngle(deltaDeg, CAngleUnit::deg()); }
        if (timeFraction0to1 <= 0.0) { return begin; }
        return begin + CAngle(timeFraction0to1 * deltaDeg, CAngleUnit::deg());
    }

    CHeading CInterpolatorPbh::getHeading() const
    {
        // HINT: VTOL aircraft can change pitch/bank without changing position, planes cannot
        // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
        const CHeading headingBegin = m_oldSituation.getHeading();
        const CHeading headingEnd   = m_newSituation.getHeading();

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(headingBegin.getReferenceNorth() == headingEnd.getReferenceNorth(), Q_FUNC_INFO, "Need same reference");
        }
        return CHeading(interpolateAngle(headingBegin, headingEnd, m_simulationTimeFraction), headingEnd.getReferenceNorth());
    }

    CAngle CInterpolatorPbh::getPitch() const
    {
        // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
        return interpolateAngle(m_oldSituation.getPitch(), m_newSituation.getPitch(), m_simulationTimeFraction);
    }

    CAngle CInterpolatorPbh::getBank() const
    {
        // Interpolate bank: Bank = (BankB - BankA) * t + BankA
        return interpolateAngle(m_oldSituation.getBank(), m_newSituation.getBank(), m_simulationTimeFraction);
    }

    CSpeed CInterpolatorPbh::getGroundSpeed() const
    {
        return (m_newSituation.getGroundSpeed() - m_oldSituation.getGroundSpeed())
                * m_simulationTimeFraction
                + m_oldSituation.getGroundSpeed();
    }

    void CInterpolatorPbh::setSituations(const CAircraftSituation &older, const CAircraftSituation &newer)
    {
        m_oldSituation = older;
        m_newSituation = newer;
    }

    void CInterpolatorPbh::setTimeFraction(double tf)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(isValidTimeFraction(tf), Q_FUNC_INFO, "Time fraction needs to be 0-1");
        }
        m_simulationTimeFraction = clampValidTimeFraction(tf);
    }
} // namespace
