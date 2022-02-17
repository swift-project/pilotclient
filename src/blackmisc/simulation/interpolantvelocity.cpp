/* Copyright (C) 2022
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/simulation/interpolantvelocity.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc::Aviation;

static constexpr int c_errorCorrectionPeriodMs = 2000;

namespace BlackMisc::Simulation
{
    void CInterpolantVelocity::setLatestSituation(const CAircraftSituation& situation)
    {
        if (situation.hasVelocity())
        {
            if (isReady())
            {
                bool ok = true;
                const CAircraftSituation extrapolated = extrapolate(situation.getMSecsSinceEpoch());
                const CAircraftVelocity error = CAircraftSituation::calculateErrorVelocity(extrapolated, situation, c_errorCorrectionPeriodMs, ok);
                if (ok)
                {
                    m_situation = extrapolated;
                    m_situation.setVelocity(situation.getVelocity() + error);
                }
                else
                {
                    m_situation = situation;

                    CLogMessage(this).debug(u"Error velocity exceeded threshold: %1") << error;
                }
            }
            else
            {
                m_situation = situation;
            }
        }
        else
        {
            m_situation.setNull();
            return;
        }
    }

    bool CInterpolantVelocity::isReady() const
    {
        return !m_situation.isNull();
    }

    CAircraftSituation CInterpolantVelocity::extrapolate(qint64 time)
    {
        Q_ASSERT(isReady());
        Q_ASSERT(m_situation.hasVelocity());
        const qint64 deltaTime = (time < 0 ? m_time : time) - m_situation.getMSecsSinceEpoch();
        return m_situation.extrapolate(static_cast<int>(deltaTime));
    }
}
