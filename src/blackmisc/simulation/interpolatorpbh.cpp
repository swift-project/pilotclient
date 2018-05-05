/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolatorpbh.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        CHeading CInterpolatorPbh::getHeading() const
        {
            // HINT: VTOL aircraft can change pitch/bank without changing position, planes cannot
            // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
            const CHeading headingBegin = m_oldSituation.getHeading();
            CHeading headingEnd = m_newSituation.getHeading();

            if ((headingEnd - headingBegin).value(CAngleUnit::deg()) < -180)
            {
                headingEnd += CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
            }

            if ((headingEnd - headingBegin).value(CAngleUnit::deg()) > 180)
            {
                headingEnd -= CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
            }

            return CHeading((headingEnd - headingBegin)
                            * m_simulationTimeFraction
                            + headingBegin,
                            headingBegin.getReferenceNorth());
        }

        CAngle CInterpolatorPbh::getPitch() const
        {
            // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
            const CAngle pitchBegin = m_oldSituation.getPitch();
            const CAngle pitchEnd = m_newSituation.getPitch();
            const CAngle pitch = (pitchEnd - pitchBegin) * m_simulationTimeFraction + pitchBegin;
            return pitch;
        }

        CAngle CInterpolatorPbh::getBank() const
        {
            // Interpolate bank: Bank = (BankB - BankA) * t + BankA
            const CAngle bankBegin = m_oldSituation.getBank();
            const CAngle bankEnd = m_newSituation.getBank();
            const CAngle bank = (bankEnd - bankBegin) * m_simulationTimeFraction + bankBegin;
            return bank;
        }

        CSpeed CInterpolatorPbh::getGroundSpeed() const
        {
            return (m_newSituation.getGroundSpeed() - m_oldSituation.getGroundSpeed())
                   * m_simulationTimeFraction
                   + m_oldSituation.getGroundSpeed();
        }
    } // namespace
} // namespace
