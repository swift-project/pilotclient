// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/interpolation/interpolatorlinear.h"
#include "blackmisc/simulation/interpolation/interpolatorfunctions.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackmisc/range.h"
#include "config/buildconfig.h"

#include <array>

using namespace swift::config;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc::Simulation
{
    CInterpolatorLinear::CInterpolant::CInterpolant(const CAircraftSituation &startSituation) : m_startSituation(startSituation), m_pbh(0, startSituation, startSituation)
    {}

    CInterpolatorLinear::CInterpolant::CInterpolant(const CAircraftSituation &startSituation, const CInterpolatorLinearPbh &pbh) : m_startSituation(startSituation), m_pbh(pbh)
    {}

    CInterpolatorLinear::CInterpolant::CInterpolant(const CAircraftSituation &startSituation, const CAircraftSituation &endSituation, double timeFraction, qint64 interpolatedTime) : IInterpolant(interpolatedTime),
                                                                                                                                                                                      m_startSituation(startSituation), m_endSituation(endSituation),
                                                                                                                                                                                      m_simulationTimeFraction(timeFraction)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            SWIFT_VERIFY_X(isValidTimeFraction(m_simulationTimeFraction), Q_FUNC_INFO, "Time fraction needs to be within [0;1]");
        }
        m_pbh = CInterpolatorLinearPbh(m_simulationTimeFraction, startSituation, endSituation);
    }

    void CInterpolatorLinear::anchor()
    {}

    std::tuple<Geo::CCoordinateGeodetic, Aviation::CAltitude> CInterpolatorLinear::CInterpolant::interpolatePositionAndAltitude() const
    {
        const std::array<double, 3> startVec(m_startSituation.getPosition().normalVectorDouble());
        const std::array<double, 3> endVec(m_endSituation.getPosition().normalVectorDouble());

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            SWIFT_VERIFY_X(CAircraftSituation::isValidVector(startVec), Q_FUNC_INFO, "Invalid start vector");
            SWIFT_VERIFY_X(CAircraftSituation::isValidVector(endVec), Q_FUNC_INFO, "Invalid end vector");
            SWIFT_VERIFY_X(isAcceptableTimeFraction(m_simulationTimeFraction), Q_FUNC_INFO, "Invalid fraction");
        }

        // Interpolate position: pos = (posB - posA) * t + posA
        CCoordinateGeodetic interpolatedPosition;
        const double tf = clampValidTimeFraction(m_simulationTimeFraction);
        interpolatedPosition.setNormalVector((endVec[0] - startVec[0]) * tf + startVec[0],
                                             (endVec[1] - startVec[1]) * tf + startVec[1],
                                             (endVec[2] - startVec[2]) * tf + startVec[2]);

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            SWIFT_VERIFY_X(interpolatedPosition.isValidVectorRange(), Q_FUNC_INFO, "Invalid vector");
        }

        // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
        // avoid underflow below ground elevation by using getCorrectedAltitude
        const CAltitude oldAlt(m_startSituation.getCorrectedAltitude());
        const CAltitude newAlt(m_endSituation.getCorrectedAltitude());
        Q_ASSERT_X(oldAlt.getReferenceDatum() == CAltitude::MeanSeaLevel && oldAlt.getReferenceDatum() == newAlt.getReferenceDatum(), Q_FUNC_INFO, "mismatch in reference"); // otherwise no calculation is possible
        const CAltitude altitude((newAlt - oldAlt) * tf + oldAlt,
                                 oldAlt.getReferenceDatum());

        return { interpolatedPosition, altitude };
    }

    Aviation::COnGroundInfo CInterpolatorLinear::CInterpolant::interpolateGroundFactor() const
    {
        const double startGroundFactor = m_startSituation.getOnGroundInfo().getGroundFactor();
        const double endGroundFactor = m_endSituation.getOnGroundInfo().getGroundFactor();
        if (CAircraftSituation::isGfEqualAirborne(startGroundFactor, endGroundFactor))
        {
            return { COnGroundInfo::NotOnGround, COnGroundInfo::OnGroundByInterpolation };
        }
        else if (CAircraftSituation::isGfEqualOnGround(startGroundFactor, endGroundFactor))
        {
            return { COnGroundInfo::OnGround, COnGroundInfo::OnGroundByInterpolation };
        }
        else
        {
            const double tf = clampValidTimeFraction(m_simulationTimeFraction);
            const double interpolatedGroundFactor = (endGroundFactor - startGroundFactor) * tf + startGroundFactor;
            return COnGroundInfo(interpolatedGroundFactor);
        }
    }

    const IInterpolant &CInterpolatorLinear::getInterpolant(SituationLog &log)
    {
        // set default situations
        CAircraftSituation startSituation = m_interpolant.getStartSituation();
        CAircraftSituation endSituation = m_interpolant.getEndSituation();

        Q_ASSERT_X(endSituation.getAdjustedMSecsSinceEpoch() >= startSituation.getAdjustedMSecsSinceEpoch(), Q_FUNC_INFO, "Wrong order");

        const bool updated = m_situationsLastModifiedUsed < m_situationsLastModified;
        const bool newSplit = endSituation.getAdjustedMSecsSinceEpoch() < m_currentTimeMsSinceEpoch;
        const bool recalculate = updated || newSplit;

        if (recalculate)
        {
            m_situationsLastModifiedUsed = m_situationsLastModified;

            // find the first situation earlier than the current time
            const auto pivot = std::partition_point(m_currentSituations.begin(), m_currentSituations.end(), [=](auto &&s) { return s.getAdjustedMSecsSinceEpoch() > m_currentTimeMsSinceEpoch; });
            const auto situationsNewer = makeRange(m_currentSituations.begin(), pivot);
            const auto situationsOlder = makeRange(pivot, m_currentSituations.end());

            // latest first, now 00:20 split time
            // time     pos
            // 00:25    10    newer
            // 00:20    11    newer
            // <----- split
            // 00:15    12    older
            // 00:10    13    older
            // 00:05    14    older

            // The first condition covers a situation, when there are no before / after situations.
            // We just place at the last position until we get before / after situations
            if (situationsOlder.isEmpty() || situationsNewer.isEmpty())
            {
                // no before situations
                if (situationsOlder.isEmpty())
                {
                    const CAircraftSituation currentSituation(*(situationsNewer.end() - 1)); // oldest newest
                    m_currentInterpolationStatus.setInterpolatedAndCheckSituation(false, currentSituation);
                    m_interpolant = { currentSituation };
                    return m_interpolant;
                }

                // only one before situation
                if (situationsOlder.size() < 2)
                {
                    const CAircraftSituation currentSituation(situationsOlder.front()); // latest oldest
                    m_currentInterpolationStatus.setInterpolatedAndCheckSituation(false, currentSituation);
                    m_interpolant = { currentSituation };
                    return m_interpolant;
                }

                // extrapolate from two before situations
                startSituation = *(situationsOlder.begin() + 1); // before newest
                endSituation = situationsOlder.front(); // newest
            }
            else
            {
                startSituation = situationsOlder.front(); // first oldest (aka newest oldest)
                endSituation = *(situationsNewer.end() - 1); // latest newest (aka oldest of newer block)
                Q_ASSERT(startSituation.getAdjustedMSecsSinceEpoch() < endSituation.getAdjustedMSecsSinceEpoch());
            }

            // adjust ground if required
            if (!startSituation.canLikelySkipNearGroundInterpolation() && !startSituation.hasGroundElevation())
            {
                const CElevationPlane planeOld = this->findClosestElevationWithinRange(startSituation, CElevationPlane::singlePointRadius());
                startSituation.setGroundElevationChecked(planeOld, CAircraftSituation::FromCache);
            }
            if (!endSituation.canLikelySkipNearGroundInterpolation() && !endSituation.hasGroundElevation())
            {
                const CElevationPlane planeNew = this->findClosestElevationWithinRange(endSituation, CElevationPlane::singlePointRadius());
                endSituation.setGroundElevationChecked(planeNew, CAircraftSituation::FromCache);
            }
        } // modified situations

        CAircraftSituation currentSituation(startSituation); // also sets ground elevation if available

        // Time between start and end packet
        const qint64 sampleDeltaTimeMs = endSituation.getAdjustedMSecsSinceEpoch() - startSituation.getAdjustedMSecsSinceEpoch();
        Q_ASSERT_X(sampleDeltaTimeMs >= 0, Q_FUNC_INFO, "Negative delta time");
        log.interpolator = 'l';

        // Fraction of the deltaTime, ideally [0.0 - 1.0]
        // < 0 should not happen due to the split, > 1 can happen if new values are delayed beyond split time
        // 1) values > 1 mean extrapolation
        // 2) values > 2 mean no new situations coming in
        const double distanceToSplitTimeMs = endSituation.getAdjustedMSecsSinceEpoch() - m_currentTimeMsSinceEpoch;
        double simulationTimeFraction = qMax(1.0 - (distanceToSplitTimeMs / sampleDeltaTimeMs), 0.0);
        if (simulationTimeFraction >= 1.0)
        {
            simulationTimeFraction = 1.0;
            if (qAbs(distanceToSplitTimeMs) > 100) { CLogMessage(this).debug(u"Distance to split: %1") << distanceToSplitTimeMs; }
        }

        const double deltaTimeFractionMs = sampleDeltaTimeMs * simulationTimeFraction;
        const qint64 interpolatedTime = startSituation.getMSecsSinceEpoch() + qRound(deltaTimeFractionMs);

        // Ref T297 adjust offset time, but this already the interpolated situation
        currentSituation.setTimeOffsetMs(startSituation.getTimeOffsetMs() + qRound((endSituation.getTimeOffsetMs() - startSituation.getTimeOffsetMs()) * simulationTimeFraction));
        currentSituation.setMSecsSinceEpoch(interpolatedTime);
        m_currentInterpolationStatus.setInterpolatedAndCheckSituation(true, currentSituation);

        if (this->doLogging())
        {
            log.tsCurrent = m_currentTimeMsSinceEpoch;
            log.deltaSampleTimesMs = sampleDeltaTimeMs;
            log.simTimeFraction = simulationTimeFraction;
            log.deltaSampleTimesMs = sampleDeltaTimeMs;
            log.tsInterpolated = interpolatedTime;
            log.interpolationSituations.clear();
            log.interpolationSituations.push_back(startSituation); // oldest at front
            log.interpolationSituations.push_back(endSituation); // latest at back
            log.interpolantRecalc = recalculate;
        }

        m_interpolant = { startSituation, endSituation, simulationTimeFraction, interpolatedTime };
        m_interpolant.setRecalculated(recalculate);

        return m_interpolant;
    }
} // namespace
