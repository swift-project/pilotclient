// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/interpolation/interpolatorlinearpbh.h"

#include "config/buildconfig.h"
#include "misc/simulation/interpolation/interpolatorfunctions.h"
#include "misc/verify.h"

using namespace swift::config;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace swift::misc::simulation
{
    CInterpolatorLinearPbh::CInterpolatorLinearPbh(double simulationTimeFraction,
                                                   const aviation::CAircraftSituation &start,
                                                   const aviation::CAircraftSituation &end)
        : m_simulationTimeFraction(simulationTimeFraction), m_startSituation(start), m_endSituation(end)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            SWIFT_VERIFY_X(isValidTimeFraction(m_simulationTimeFraction), Q_FUNC_INFO,
                           "Time fraction needs to be within [0;1]");
        }
    }

    CAngle CInterpolatorLinearPbh::interpolateAngle(const CAngle &begin, const CAngle &end, double timeFraction0to1)
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
            SWIFT_VERIFY_X(isAcceptableTimeFraction(timeFraction0to1), Q_FUNC_INFO, "0..1 fraction needed");
        }

        //! make sure to not end up we extrapolation
        if (timeFraction0to1 >= 1.0) { return begin + CAngle(deltaDeg, CAngleUnit::deg()); }
        if (timeFraction0to1 <= 0.0) { return begin; }
        return begin + CAngle(timeFraction0to1 * deltaDeg, CAngleUnit::deg());
    }

    CHeading CInterpolatorLinearPbh::getHeading() const
    {
        // HINT: VTOL aircraft can change pitch/bank without changing position, planes cannot
        // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
        const CHeading headingStart = m_startSituation.getHeading();
        const CHeading headingEnd = m_endSituation.getHeading();

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            SWIFT_VERIFY_X(headingStart.getReferenceNorth() == headingEnd.getReferenceNorth(), Q_FUNC_INFO,
                           "Need same reference");
        }
        return { interpolateAngle(headingStart, headingEnd, m_simulationTimeFraction), headingEnd.getReferenceNorth() };
    }

    CAngle CInterpolatorLinearPbh::getPitch() const
    {
        // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
        return interpolateAngle(m_startSituation.getPitch(), m_endSituation.getPitch(), m_simulationTimeFraction);
    }

    CAngle CInterpolatorLinearPbh::getBank() const
    {
        // Interpolate bank: Bank = (BankB - BankA) * t + BankA
        return interpolateAngle(m_startSituation.getBank(), m_endSituation.getBank(), m_simulationTimeFraction);
    }

    CSpeed CInterpolatorLinearPbh::getGroundSpeed() const
    {
        return (m_endSituation.getGroundSpeed() - m_startSituation.getGroundSpeed()) * m_simulationTimeFraction +
               m_startSituation.getGroundSpeed();
    }

    void CInterpolatorLinearPbh::setTimeFraction(double tf)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            SWIFT_VERIFY_X(isValidTimeFraction(tf), Q_FUNC_INFO, "Time fraction needs to be 0-1");
        }
        m_simulationTimeFraction = clampValidTimeFraction(tf);
    }
} // namespace swift::misc::simulation
