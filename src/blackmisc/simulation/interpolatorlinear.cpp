/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolatorlinear.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/compare.h"
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

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolatorLinear::Interpolant::Interpolant(const CAircraftSituation &situation) :
            m_situationsAvailable(1), m_oldSituation(situation),
            m_pbh(0, situation, situation)
        {}

        CInterpolatorLinear::Interpolant::Interpolant(const CAircraftSituation &situation1, const CAircraftSituation &situation2, double timeFraction, qint64 interpolatedTime) :
            m_situationsAvailable(2),
            m_oldSituation(situation1), m_newSituation(situation2),
            m_simulationTimeFraction(timeFraction),
            m_interpolatedTime(interpolatedTime),
            m_pbh(m_simulationTimeFraction, situation1, situation2)
        {}

        CAircraftSituation CInterpolatorLinear::Interpolant::interpolatePositionAndAltitude(const CAircraftSituation &situation, bool interpolateGndFactor) const
        {
            const std::array<double, 3> oldVec(m_oldSituation.getPosition().normalVectorDouble());
            const std::array<double, 3> newVec(m_newSituation.getPosition().normalVectorDouble());

            // Interpolate position: pos = (posB - posA) * t + posA
            CCoordinateGeodetic newPosition;
            newPosition.setNormalVector((newVec[0] - oldVec[0]) * m_simulationTimeFraction + oldVec[0],
                                        (newVec[1] - oldVec[1]) * m_simulationTimeFraction + oldVec[1],
                                        (newVec[2] - oldVec[2]) * m_simulationTimeFraction + oldVec[2]);

            // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
            // avoid underflow below ground elevation by using getCorrectedAltitude
            const CAltitude oldAlt(m_oldSituation.getCorrectedAltitude());
            const CAltitude newAlt(m_newSituation.getCorrectedAltitude());
            Q_ASSERT_X(oldAlt.getReferenceDatum() == CAltitude::MeanSeaLevel && oldAlt.getReferenceDatum() == newAlt.getReferenceDatum(), Q_FUNC_INFO, "mismatch in reference"); // otherwise no calculation is possible
            const CAltitude altitude((newAlt - oldAlt)
                                     * m_simulationTimeFraction
                                     + oldAlt,
                                     oldAlt.getReferenceDatum());

            CAircraftSituation newSituation(situation);
            newSituation.setPosition(newPosition);
            newSituation.setAltitude(altitude);
            newSituation.setMSecsSinceEpoch(this->getInterpolatedTime());

            if (interpolateGndFactor)
            {
                const double oldGroundFactor = m_oldSituation.getOnGroundFactor();
                const double newGroundFactor = m_newSituation.getOnGroundFactor();
                do
                {
                    if (CAircraftSituation::isGfEqualAirborne(oldGroundFactor, newGroundFactor)) { newSituation.setOnGround(false); break; }
                    if (CAircraftSituation::isGfEqualOnGround(oldGroundFactor, newGroundFactor)) { newSituation.setOnGround(true); break; }
                    const double groundFactor = (newGroundFactor - oldGroundFactor) * m_simulationTimeFraction + oldGroundFactor;
                    newSituation.setOnGroundFactor(groundFactor);
                    newSituation.setOnGroundFromGroundFactorFromInterpolation(groundInterpolationFactor());
                }
                while (false);
            }
            return newSituation;
        }

        CInterpolatorLinear::Interpolant CInterpolatorLinear::getInterpolant(
            qint64 currentTimeMsSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CInterpolationStatus &status, SituationLog &log)
        {
            Q_UNUSED(setup);
            status.reset();

            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            const CAircraftSituationList validSituations = this->remoteAircraftSituations(m_callsign); // if needed, we could also copy here
            if (!CBuildConfig::isReleaseBuild())
            {
                BLACK_VERIFY_X(validSituations.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Wrong sort order");
                Q_ASSERT_X(validSituations.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");
            }

            const qint64 tsLastModified = this->situationsLastModified(m_callsign);
            if (m_situationsLastModifiedUsed < tsLastModified || m_situationChange.isNull())
            {
                m_situationsLastModifiedUsed = tsLastModified;
                m_situationChange = CAircraftSituationChange(validSituations, true, true);
            }

            // find the first situation earlier than the current time
            const auto pivot = std::partition_point(validSituations.begin(), validSituations.end(), [ = ](auto &&s) { return s.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoc; });
            const auto situationsNewer = makeRange(validSituations.begin(), pivot);
            const auto situationsOlder = makeRange(pivot, validSituations.end());

            // interpolation situations
            CAircraftSituation oldSituation;
            CAircraftSituation newSituation;

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
                    status.setInterpolatedAndCheckSituation(false, currentSituation);
                    return currentSituation;
                }

                // only one before situation
                if (situationsOlder.size() < 2)
                {
                    const CAircraftSituation currentSituation(situationsOlder.front()); // latest oldest
                    status.setInterpolatedAndCheckSituation(false, currentSituation);
                    return currentSituation;
                }

                // extrapolate from two before situations
                oldSituation = *(situationsOlder.begin() + 1); // before newest
                newSituation = situationsOlder.front(); // newest
            }
            else
            {
                oldSituation = situationsOlder.front(); // first oldest (aka newest oldest)
                newSituation = *(situationsNewer.end() - 1); // latest newest (aka oldest of newer block)
                Q_ASSERT(oldSituation.getAdjustedMSecsSinceEpoch() < newSituation.getAdjustedMSecsSinceEpoch());
            }

            // adjust ground if required
            if (!oldSituation.canLikelySkipNearGroundInterpolation() && !oldSituation.hasGroundElevation())
            {
                const CElevationPlane planeOld = this->findClosestElevationWithinRange(oldSituation, CElevationPlane::singlePointRadius());
                oldSituation.setGroundElevationChecked(planeOld);
            }
            if (!newSituation.canLikelySkipNearGroundInterpolation() && !newSituation.hasGroundElevation())
            {
                const CElevationPlane planeNew = this->findClosestElevationWithinRange(newSituation, CElevationPlane::singlePointRadius());
                newSituation.setGroundElevationChecked(planeNew);
            }

            CAircraftSituation currentSituation(oldSituation); // also sets ground elevation if available

            // Time between start and end packet
            const qint64 sampleDeltaTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - oldSituation.getAdjustedMSecsSinceEpoch();
            Q_ASSERT_X(sampleDeltaTimeMs >= 0, Q_FUNC_INFO, "Negative delta time");
            log.interpolator = 'l';

            // Fraction of the deltaTime, ideally [0.0 - 1.0]
            // < 0 should not happen due to the split, > 1 can happen if new values are delayed beyond split time
            // 1) values > 1 mean extrapolation
            // 2) values > 2 mean no new situations coming in
            const double distanceToSplitTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - currentTimeMsSinceEpoc;
            const double simulationTimeFraction = qMax(1.0 - (distanceToSplitTimeMs / sampleDeltaTimeMs), 0.0);
            const double deltaTimeFractionMs = sampleDeltaTimeMs * simulationTimeFraction;
            const qint64 interpolatedTime = oldSituation.getMSecsSinceEpoch() + deltaTimeFractionMs;

            currentSituation.setTimeOffsetMs(oldSituation.getTimeOffsetMs() + (newSituation.getTimeOffsetMs() - oldSituation.getTimeOffsetMs()) * simulationTimeFraction);
            currentSituation.setMSecsSinceEpoch(interpolatedTime);
            status.setInterpolatedAndCheckSituation(true, currentSituation);

            if (this->hasAttachedLogger() && setup.logInterpolation())
            {
                log.tsCurrent = currentTimeMsSinceEpoc;
                log.deltaSampleTimesMs = sampleDeltaTimeMs;
                log.simTimeFraction = simulationTimeFraction;
                log.deltaSampleTimesMs = sampleDeltaTimeMs;
                log.tsInterpolated = interpolatedTime;
                log.interpolationSituations.clear();
                log.interpolationSituations.push_back(newSituation); // newest at front
                log.interpolationSituations.push_back(oldSituation); // oldest at back
            }

            return { oldSituation, newSituation, simulationTimeFraction, interpolatedTime };
        }
    } // namespace
} // namespace
