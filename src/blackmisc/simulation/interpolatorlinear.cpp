/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolatorlinear.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/compare.h"
#include "blackmisc/verify.h"
#include "blackmisc/range.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessage.h"

#include <QDateTime>
#include <QList>
#include <array>

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

        CInterpolatorLinear::Interpolant CInterpolatorLinear::getInterpolant(qint64 currentTimeMsSinceEpoc,
                const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, CInterpolationStatus &status, SituationLog &log) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);
            status.reset();

            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            BLACK_VERIFY_X(m_aircraftSituations.isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
            Q_ASSERT_X(m_aircraftSituations.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");

            // Ref T243, KB 2018-02, can be removed in future, we verify situations above
            // Situations are supposed to be in correct order
            // const auto end = std::is_sorted_until(m_aircraftSituations.begin(), m_aircraftSituations.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
            // const auto validSituations = makeRange(m_aircraftSituations.begin(), end);

            // find the first situation earlier than the current time
            const CAircraftSituationList &validSituations = m_aircraftSituations; // if needed, we could also copy here
            const auto pivot = std::partition_point(validSituations.begin(), validSituations.end(), [ = ](auto && s) { return s.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoc; });
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

            // take hint into account to calculate elevation and above ground level
            // do not call for XP (lazy init)
            if (!hints.hasElevationProvider())
            {
                CInterpolator::setGroundElevationFromHint(hints, oldSituation, false);
                CInterpolator::setGroundElevationFromHint(hints, newSituation, false);
            }

            CAircraftSituation currentSituation(oldSituation); // also sets ground elevation if available

            // Time between start and end packet
            const double sampleDeltaTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - oldSituation.getAdjustedMSecsSinceEpoch();
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

            if (this->hasAttachedLogger() && hints.isLoggingInterpolation())
            {
                log.tsCurrent = currentTimeMsSinceEpoc;
                log.deltaSampleTimesMs = sampleDeltaTimeMs;
                log.simulationTimeFraction = simulationTimeFraction;
                log.deltaSampleTimesMs = sampleDeltaTimeMs;
                log.tsInterpolated = interpolatedTime;
                log.interpolationSituations.clear();
                log.interpolationSituations.push_back(newSituation); // newest at front
                log.interpolationSituations.push_back(oldSituation); // oldest at back
            }

            return { oldSituation, newSituation, simulationTimeFraction, interpolatedTime };
        }

        CCoordinateGeodetic CInterpolatorLinear::Interpolant::interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);

            const std::array<double, 3> oldVec(m_oldSituation.getPosition().normalVectorDouble());
            const std::array<double, 3> newVec(m_newSituation.getPosition().normalVectorDouble());

            // Interpolate position: pos = (posB - posA) * t + posA
            CCoordinateGeodetic currentPosition;
            currentPosition.setNormalVector((newVec[0] - oldVec[0]) * m_simulationTimeFraction + oldVec[0],
                                            (newVec[1] - oldVec[1]) * m_simulationTimeFraction + oldVec[1],
                                            (newVec[2] - oldVec[2]) * m_simulationTimeFraction + oldVec[2]);
            return currentPosition;
        }

        CAltitude CInterpolatorLinear::Interpolant::interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);

            // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
            // avoid underflow below ground elevation by using getCorrectedAltitude
            const CAltitude oldAlt(m_oldSituation.getCorrectedAltitude());
            const CAltitude newAlt(m_newSituation.getCorrectedAltitude());
            Q_ASSERT_X(oldAlt.getReferenceDatum() == CAltitude::MeanSeaLevel && oldAlt.getReferenceDatum() == newAlt.getReferenceDatum(), Q_FUNC_INFO, "mismatch in reference"); // otherwise no calculation is possible
            return CAltitude((newAlt - oldAlt)
                             * m_simulationTimeFraction
                             + oldAlt,
                             oldAlt.getReferenceDatum());
        }
    } // namespace
} // namespace
