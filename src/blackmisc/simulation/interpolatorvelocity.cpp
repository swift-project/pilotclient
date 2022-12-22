/* Copyright (C) 2022
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/interpolatorvelocity.h"
#include "blackmisc/simulation/interpolatorfunctions.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/verify.h"
#include "blackmisc/range.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessage.h"
#include "blackconfig/buildconfig.h"

#include <QDateTime>
#include <QList>
#include <array>

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc::Simulation
{
    CInterpolatorVelocity::CInterpolant::CInterpolant(const CAircraftSituation &currentSituation) :
        IInterpolant(1, CInterpolatorPbh(0, currentSituation, currentSituation)),
        m_currentSitutation(currentSituation)
    {
        m_creationTimeMsSinceEpoch = currentSituation.getMSecsSinceEpoch();
        m_currentTimeMsSinceEpoch = currentSituation.getMSecsSinceEpoch();
    }

    CInterpolatorVelocity::CInterpolant::CInterpolant(const CInterpolant &old, const CAircraftSituation &currentSituation) :
        IInterpolant(1, CInterpolatorPbh(0, currentSituation, currentSituation)),
        m_currentSitutation(currentSituation)
    {   
        m_creationTimeMsSinceEpoch = currentSituation.getMSecsSinceEpoch();
        m_currentTimeMsSinceEpoch = currentSituation.getMSecsSinceEpoch();

        // Get the situation of the aircraft that the old interpolant would show at the current timestamp
        // and calculate delta to to this position (correction vectors)
        CInterpolant oldCopy(old);
        oldCopy.update(currentSituation.getMSecsSinceEpoch());
        CAircraftSituation unusedSituation;
        const CAircraftSituation lastSitutation = oldCopy.interpolatePositionAndAltitude(unusedSituation, true);

        m_altitudeDelta = lastSitutation.getAltitude().value(CLengthUnit::m()) - currentSituation.getCorrectedAltitude().value(CLengthUnit::m());
        m_latDeltaRad = lastSitutation.getPosition().latitude().value(CAngleUnit::rad()) - currentSituation.getPosition().latitude().value(CAngleUnit::rad());
        m_lonDeltaRad = lastSitutation.getPosition().longitude().value(CAngleUnit::rad()) - currentSituation.getPosition().longitude().value(CAngleUnit::rad());

        auto correctTurnDirection = [](double delta) -> double
        {
            // Always take the shortest turning angle
            if (delta > M_PI)
            {
                // Wrap 180 deg and turning to positiv direction to -180 deg 
                delta = - (2 * M_PI - delta);
            }
            else if (delta < -M_PI)
            {
                // Wrap -180 deg and turning to negative direction to 180 deg 
                delta  = 2 * M_PI - std::abs(delta);
            }
            return delta;
        };

        m_bankDelta = lastSitutation.getBank().value(CAngleUnit::rad()) - currentSituation.getBank().value(CAngleUnit::rad());
        m_bankDelta = correctTurnDirection(m_bankDelta);

        m_headingDelta = lastSitutation.getHeading().value(CAngleUnit::rad()) - currentSituation.getHeading().value(CAngleUnit::rad());
        m_headingDelta = correctTurnDirection(m_headingDelta);

        m_pitchDelta = lastSitutation.getPitch().value(CAngleUnit::rad()) - currentSituation.getPitch().value(CAngleUnit::rad());
        m_pitchDelta = correctTurnDirection(m_pitchDelta);
    }

    void CInterpolatorVelocity::CInterpolant::update(qint64 currentMsSinceEpoch) 
    {
        Q_ASSERT_X(currentMsSinceEpoch >= m_creationTimeMsSinceEpoch, Q_FUNC_INFO, "Updated current time must be greater creation time");
        m_currentTimeMsSinceEpoch = currentMsSinceEpoch;
    }

    void CInterpolatorVelocity::anchor()
    { }

    CAircraftSituation CInterpolatorVelocity::CInterpolant::interpolatePositionAndAltitude(const CAircraftSituation &situation, bool interpolateGndFactor) const
    {
        // Inspired by xPilot
        // https://github.com/xpilot-project/xpilot/blob/97cf83efd650464b997dd3d5dee0ccde198cdf2c/plugin/src/NetworkAircraft.cpp
        Q_ASSERT_X(m_currentTimeMsSinceEpoch >= m_creationTimeMsSinceEpoch, Q_FUNC_INFO, "Current time must be equal or greater than creation time");

        constexpr qint64 maxExtrapolationMs = 7000;
        qint64 diffMs = m_currentTimeMsSinceEpoch - m_creationTimeMsSinceEpoch;
        if (diffMs > maxExtrapolationMs)
        {
            // Do not extrapolate more than maxExtrapolationMs (e.g. user timed out)
            diffMs = maxExtrapolationMs;
        }

        // situation might include ground elevation already -> copy
        CAircraftSituation sit(situation);
        constexpr double correction_time_ms = 1000;

        // 1 -> 0 within correction_time_ms
        const double errorOffsetFraction = (correction_time_ms - std::clamp(static_cast<double>(diffMs), 0.0, correction_time_ms)) / correction_time_ms;
        Q_ASSERT_X(errorOffsetFraction >= 0 && errorOffsetFraction <= 1, Q_FUNC_INFO, "Offset fraction must be between 0 and 1");

        {
            // Heading
            const double currentHeading = m_currentSitutation.getHeading().value(CAngleUnit::rad());
            const double headingVelo = m_currentSitutation.getVelocity().getHeadingVelocity(CAngleUnit::rad(), CTimeUnit::s());
            sit.setHeading(CHeading(currentHeading + diffMs/1000.0 * headingVelo + errorOffsetFraction * m_headingDelta, CAngleUnit::rad()));
        }

        {
            // Pitch
            const double currentPitch = m_currentSitutation.getPitch().value(CAngleUnit::rad());
            const double pitchVelo = m_currentSitutation.getVelocity().getPitchVelocity(CAngleUnit::rad(), CTimeUnit::s());
            sit.setPitch(CHeading(currentPitch + diffMs/1000.0 * pitchVelo + errorOffsetFraction * m_pitchDelta, CAngleUnit::rad()));
        }

        {
            // Bank
            const double currentBank = m_currentSitutation.getBank().value(CAngleUnit::rad());
            const double bankVelo = m_currentSitutation.getVelocity().getRollVelocity(CAngleUnit::rad(), CTimeUnit::s());
            sit.setBank(CHeading(currentBank + diffMs/1000.0 * bankVelo + errorOffsetFraction * m_bankDelta, CAngleUnit::rad()));
        }

        {
            // Altitude
            const double currentAltitude = m_currentSitutation.getCorrectedAltitude().value(CLengthUnit::m());
            const double altVelo = m_currentSitutation.getVelocity().getVelocityY(CSpeedUnit::m_s());

            sit.setAltitude(CAltitude(currentAltitude + diffMs/1000.0 * altVelo + errorOffsetFraction * m_altitudeDelta, CLengthUnit::m()));
        }

        {
            // Position
            CCoordinateGeodetic pos = m_currentSitutation.getPosition();

            // Latitude
            {
                const double latVelo = m_currentSitutation.getVelocity().getVelocityZ(CSpeedUnit::m_s());
                const CLength extrapolateLengthLat = CLength(latVelo * diffMs/1000.0, CLengthUnit::m());
                const double shiftDirectionLat = (extrapolateLengthLat.value(CLengthUnit::m()) >= 0 ? 0 : 180);
                pos = pos.calculatePosition(extrapolateLengthLat.abs(), CAngle(shiftDirectionLat, CAngleUnit::deg()));
            }

            // Longitude
            {
                const double lonVelo = m_currentSitutation.getVelocity().getVelocityX(CSpeedUnit::m_s());
                const CLength extrapolateLengthLon = CLength(lonVelo * diffMs/1000.0, CLengthUnit::m());
                const double shiftDirectionLon = (extrapolateLengthLon.value(CLengthUnit::m()) >= 0 ? 90 : 270);
                pos = pos.calculatePosition(extrapolateLengthLon.abs(), CAngle(shiftDirectionLon, CAngleUnit::deg()));
            }

            // Apply error
            const CLatitude latError(errorOffsetFraction * m_latDeltaRad, CAngleUnit::rad());
            pos.setLatitude(pos.latitude() + latError);
            const CLongitude lonError(errorOffsetFraction * m_lonDeltaRad, CAngleUnit::rad());
            pos.setLongitude(pos.longitude() + lonError);

            // Copy altitude to avoid overriding with new position
            const CAltitude altitude = sit.getAltitude();
            sit.setPosition(pos);
            sit.setAltitude(altitude);
        }

        // Set ground factor directly from update (no extra/interpolation for now)
        if (interpolateGndFactor)
        {
            sit.setOnGroundDetails(CAircraftSituation::OnGroundByInterpolation);
            sit.setOnGround(m_currentSitutation.isOnGround());
            sit.setOnGroundFactor(m_currentSitutation.isOnGround() ? 1.0 : 0.0);
        }

        sit.setMSecsSinceEpoch(getInterpolatedTime());
        return sit;
    }

    CInterpolatorVelocity::CInterpolant CInterpolatorVelocity::getInterpolant(SituationLog &log)
    {
        Q_ASSERT_X(m_currentSituations.size() >= 1, Q_FUNC_INFO, "Velocity interpolator needs at least one situation");
        CAircraftSituation current{};
        // Find first (latest) situation with velocity data
        auto it = std::find_if(m_currentSituations.begin(), m_currentSituations.end(), [](const CAircraftSituation &sit){ return sit.hasVelocity(); });
        if (it != m_currentSituations.end())
        {
            current = *it;
        }
        else
        {
            // No situation with velocity data available. Take the latest full update with velocities set to 0
            current = *m_currentSituations.begin();
            current.setVelocity(CAircraftVelocity(0, 0, 0, CSpeedUnit::m_s(), 0, 0, 0, CAngleUnit::rad(), CTimeUnit::s()));
        }

        Q_ASSERT_X(current.hasVelocity(), Q_FUNC_INFO, "Velocity interpolator needs situation with valid velocities");

        // adjust ground if required
        if (!current.canLikelySkipNearGroundInterpolation() && !current.hasGroundElevation())
        {
            // The elevation needs to be requested again when we do not receive any velocity updates of an aircraft
            // for some time and the elevation is therefore discarded from the cache.
            // This happens when the aircraft is not moving and we only receive a #ST packet once.
            const CElevationPlane planeOld = this->findClosestElevationWithinRange(current, CElevationPlane::singlePointRadius());
            current.setGroundElevationChecked(planeOld, CAircraftSituation::FromCache);
        }

        if(!m_interpolant.isValid()) 
        {
            m_interpolant = CInterpolant{ current };
            m_lastSituation = current;
        }
        else if (m_lastSituation == current)
        {
            m_interpolant.update(m_currentTimeMsSinceEpoch);
        }
        else
        {
            m_interpolant = CInterpolant{ m_interpolant, current };
            m_lastSituation = current;
        }

        // Always changed (at least time)
        m_interpolant.setRecalculated(true);

        if (this->doLogging())
        {
            log.tsCurrent = m_currentTimeMsSinceEpoch;
            // log.deltaSampleTimesMs = sampleDeltaTimeMs;
            // log.simTimeFraction = simulationTimeFraction;
            // log.deltaSampleTimesMs = sampleDeltaTimeMs;
            // log.tsInterpolated = interpolatedTime;
            log.interpolationSituations.clear();
            log.interpolationSituations.push_back(*m_currentSituations.begin()); // current at front
            log.interpolationSituations.push_back(m_interpolant.interpolatePositionAndAltitude(CAircraftSituation{}, true)); // interpolated second
            log.interpolantRecalc = true;
        }

        return m_interpolant;
    }
} // namespace
