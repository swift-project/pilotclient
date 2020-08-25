/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "interpolatorlinear.h"
#include "interpolatorfunctions.h"
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

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolatorLinear::CInterpolant::CInterpolant(const CAircraftSituation &oldSituation) :
            IInterpolant(1, CInterpolatorPbh(0, oldSituation, oldSituation)),
            m_oldSituation(oldSituation)
        { }

        CInterpolatorLinear::CInterpolant::CInterpolant(const CAircraftSituation &oldSituation, const CInterpolatorPbh &pbh) :
            IInterpolant(1, pbh),
            m_oldSituation(oldSituation)
        { }

        CInterpolatorLinear::CInterpolant::CInterpolant(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, double timeFraction, qint64 interpolatedTime) :
            IInterpolant(interpolatedTime, 2),
            m_oldSituation(oldSituation), m_newSituation(newSituation),
            m_simulationTimeFraction(timeFraction)
        {
            m_pbh = CInterpolatorPbh(m_simulationTimeFraction, oldSituation, newSituation);
        }

        void CInterpolatorLinear::anchor()
        { }

        CAircraftSituation CInterpolatorLinear::CInterpolant::interpolatePositionAndAltitude(const CAircraftSituation &situation, bool interpolateGndFactor) const
        {
            const std::array<double, 3> oldVec(m_oldSituation.getPosition().normalVectorDouble());
            const std::array<double, 3> newVec(m_newSituation.getPosition().normalVectorDouble());

            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(CAircraftSituation::isValidVector(oldVec), Q_FUNC_INFO, "Invalid old vector");
                BLACK_VERIFY_X(CAircraftSituation::isValidVector(newVec), Q_FUNC_INFO, "Invalid new vector");
                BLACK_VERIFY_X(isAcceptableTimeFraction(m_simulationTimeFraction), Q_FUNC_INFO, "Invalid fraction");
            }

            // Interpolate position: pos = (posB - posA) * t + posA
            CCoordinateGeodetic newPosition;
            const double tf = clampValidTimeFraction(m_simulationTimeFraction);
            newPosition.setNormalVector((newVec[0] - oldVec[0]) * tf + oldVec[0],
                                        (newVec[1] - oldVec[1]) * tf + oldVec[1],
                                        (newVec[2] - oldVec[2]) * tf + oldVec[2]);

            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(newPosition.isValidVectorRange(), Q_FUNC_INFO, "Invalid vector");
            }

            // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
            // avoid underflow below ground elevation by using getCorrectedAltitude
            const CAltitude oldAlt(m_oldSituation.getCorrectedAltitude());
            const CAltitude newAlt(m_newSituation.getCorrectedAltitude());
            Q_ASSERT_X(oldAlt.getReferenceDatum() == CAltitude::MeanSeaLevel && oldAlt.getReferenceDatum() == newAlt.getReferenceDatum(), Q_FUNC_INFO, "mismatch in reference"); // otherwise no calculation is possible
            const CAltitude altitude((newAlt - oldAlt)
                                     * tf
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
                    if (CAircraftSituation::isGfEqualOnGround(oldGroundFactor, newGroundFactor)) { newSituation.setOnGround(true);  break; }
                    const double groundFactor = (newGroundFactor - oldGroundFactor) * tf + oldGroundFactor;
                    newSituation.setOnGroundFactor(groundFactor);
                    newSituation.setOnGroundFromGroundFactorFromInterpolation(groundInterpolationFactor());
                }
                while (false);
            }
            return newSituation;
        }

        CInterpolatorLinear::CInterpolant CInterpolatorLinear::getInterpolant(SituationLog &log)
        {
            // set default situations
            CAircraftSituation oldSituation = m_interpolant.getOldSituation();
            CAircraftSituation newSituation = m_interpolant.getNewSituation();

            Q_ASSERT_X(newSituation.getAdjustedMSecsSinceEpoch() >= oldSituation.getAdjustedMSecsSinceEpoch(), Q_FUNC_INFO, "Wrong order");

            const bool updated = m_situationsLastModifiedUsed < m_situationsLastModified;
            const bool newSplit = newSituation.getAdjustedMSecsSinceEpoch() < m_currentTimeMsSinceEpoch;
            const bool recalculate = updated || newSplit;

            if (recalculate)
            {
                m_situationsLastModifiedUsed = m_situationsLastModified;

                // find the first situation earlier than the current time
                const auto pivot = std::partition_point(m_currentSituations.begin(), m_currentSituations.end(), [ = ](auto &&s) { return s.getAdjustedMSecsSinceEpoch() > m_currentTimeMsSinceEpoch; });
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
                    oldSituation.setGroundElevationChecked(planeOld, CAircraftSituation::FromCache);
                }
                if (!newSituation.canLikelySkipNearGroundInterpolation() && !newSituation.hasGroundElevation())
                {
                    const CElevationPlane planeNew = this->findClosestElevationWithinRange(newSituation, CElevationPlane::singlePointRadius());
                    newSituation.setGroundElevationChecked(planeNew, CAircraftSituation::FromCache);
                }
            } // modified situations

            CAircraftSituation currentSituation(oldSituation); // also sets ground elevation if available

            // Time between start and end packet
            const qint64 sampleDeltaTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - oldSituation.getAdjustedMSecsSinceEpoch();
            Q_ASSERT_X(sampleDeltaTimeMs >= 0, Q_FUNC_INFO, "Negative delta time");
            log.interpolator = 'l';

            // Fraction of the deltaTime, ideally [0.0 - 1.0]
            // < 0 should not happen due to the split, > 1 can happen if new values are delayed beyond split time
            // 1) values > 1 mean extrapolation
            // 2) values > 2 mean no new situations coming in
            const double distanceToSplitTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - m_currentTimeMsSinceEpoch;
            double simulationTimeFraction = qMax(1.0 - (distanceToSplitTimeMs / sampleDeltaTimeMs), 0.0);
            if (simulationTimeFraction >= 1.0)
            {
                simulationTimeFraction = 1.0;
                if (qAbs(distanceToSplitTimeMs) > 100) { CLogMessage(this).debug(u"Distance to split: %1") << distanceToSplitTimeMs; }
            }

            const double deltaTimeFractionMs = sampleDeltaTimeMs * simulationTimeFraction;
            const qint64 interpolatedTime = oldSituation.getMSecsSinceEpoch() + qRound(deltaTimeFractionMs);

            // Ref T297 adjust offset time, but this already the interpolated situation
            currentSituation.setTimeOffsetMs(oldSituation.getTimeOffsetMs() + qRound((newSituation.getTimeOffsetMs() - oldSituation.getTimeOffsetMs()) * simulationTimeFraction));
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
                log.interpolationSituations.push_back(oldSituation); // oldest at front
                log.interpolationSituations.push_back(newSituation); // latest at back
                log.interpolantRecalc = recalculate;
            }

            m_interpolant = { oldSituation, newSituation, simulationTimeFraction, interpolatedTime };
            m_interpolant.setRecalculated(recalculate);

            return m_interpolant;
        }
    } // namespace
} // namespace
