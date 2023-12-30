// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/interpolatorpbh.h"
#include "blackmisc/simulation/interpolatorfunctions.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Simulation
{
    CInterpolatorPbh::CInterpolatorPbh(double simulationTimeFraction, const Aviation::CAircraftSituation &start, const Aviation::CAircraftSituation &end) : m_simulationTimeFraction(simulationTimeFraction),
                                                                                                                                                            m_startSituation(start),
                                                                                                                                                            m_endSituation(end)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(isValidTimeFraction(m_simulationTimeFraction), Q_FUNC_INFO, "Time fraction needs to be within [0;1]");
        }
    }

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
        const CHeading headingStart = m_startSituation.getHeading();
        const CHeading headingEnd = m_endSituation.getHeading();

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(headingStart.getReferenceNorth() == headingEnd.getReferenceNorth(), Q_FUNC_INFO, "Need same reference");
        }
        return CHeading(interpolateAngle(headingStart, headingEnd, m_simulationTimeFraction), headingEnd.getReferenceNorth());
    }

    CAngle CInterpolatorPbh::getPitch() const
    {
        // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
        return interpolateAngle(m_startSituation.getPitch(), m_endSituation.getPitch(), m_simulationTimeFraction);
    }

    CAngle CInterpolatorPbh::getBank() const
    {
        // Interpolate bank: Bank = (BankB - BankA) * t + BankA
        return interpolateAngle(m_startSituation.getBank(), m_endSituation.getBank(), m_simulationTimeFraction);
    }

    CSpeed CInterpolatorPbh::getGroundSpeed() const
    {
        return (m_endSituation.getGroundSpeed() - m_startSituation.getGroundSpeed()) * m_simulationTimeFraction + m_startSituation.getGroundSpeed();
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
