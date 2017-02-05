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
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/compare.h"
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
        CAircraftSituation CInterpolatorLinear::getInterpolatedSituation(const CCallsign &callsign, const CAircraftSituationList &situations, qint64 currentTimeMsSinceEpoc,
            const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, InterpolationStatus &status) const
        {
            //
            // function has to be thread safe
            //
            status.reset();

            // any data at all?
            if (situations.isEmpty()) { return {}; }

            // data, split situations by time
            if (currentTimeMsSinceEpoc < 0) { currentTimeMsSinceEpoc = QDateTime::currentMSecsSinceEpoch(); }

            // find the first situation not in the correct order, keep only the situations before that one
            // any updates in wrong chronological order are discounted
            const auto end = std::is_sorted_until(situations.begin(), situations.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
            const auto validSituations = makeRange(situations.begin(), end);

            // find the first situation earlier than the current time
            const auto pivot = std::partition_point(validSituations.begin(), validSituations.end(), [ = ](auto && s) { return s.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoc; });
            const auto situationsNewer = makeRange(validSituations.begin(), pivot);
            const auto situationsOlder = makeRange(pivot, validSituations.end());

            // interpolation situations
            CAircraftSituation oldSituation;
            CAircraftSituation newSituation;
            InterpolationLog log;

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
                if (situationsOlder.isEmpty()) { return *(situationsNewer.end() - 1); } // oldest newest

                // only one before situation
                if (situationsOlder.size() < 2) { return situationsOlder.front(); } // latest older

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
                IInterpolator::setGroundElevationFromHint(hints, oldSituation, false);
                IInterpolator::setGroundElevationFromHint(hints, newSituation, false);
            }

            CAircraftSituation currentSituation(oldSituation); // also sets ground elevation if available
            CCoordinateGeodetic currentPosition;

            // Time between start and end packet
            const double deltaTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - oldSituation.getAdjustedMSecsSinceEpoch();
            Q_ASSERT_X(deltaTimeMs >= 0, Q_FUNC_INFO, "Negative delta time");
            log.deltaTimeMs = deltaTimeMs;

            // Fraction of the deltaTime, ideally [0.0 - 1.0]
            // < 0 should not happen due to the split, > 1 can happen if new values are delayed beyond split time
            // 1) values > 1 mean extrapolation
            // 2) values > 2 mean no new situations coming in
            const double distanceToSplitTimeMs = newSituation.getAdjustedMSecsSinceEpoch() - currentTimeMsSinceEpoc;
            const double simulationTimeFraction = 1.0 - (distanceToSplitTimeMs / deltaTimeMs);
            const double deltaTimeFractionMs = deltaTimeMs * simulationTimeFraction;
            log.simulationTimeFraction = simulationTimeFraction;
            log.deltaTimeFractionMs = deltaTimeFractionMs;

            currentSituation.setTimeOffsetMs(oldSituation.getTimeOffsetMs() + (newSituation.getTimeOffsetMs() - oldSituation.getTimeOffsetMs()) * simulationTimeFraction);
            currentSituation.setMSecsSinceEpoch(oldSituation.getMSecsSinceEpoch() + deltaTimeFractionMs);

            const std::array<double, 3> oldVec(oldSituation.getPosition().normalVectorDouble());
            const std::array<double, 3> newVec(newSituation.getPosition().normalVectorDouble());

            // Interpolate position: pos = (posB - posA) * t + posA
            currentPosition.setNormalVector((newVec[0] - oldVec[0]) * simulationTimeFraction + oldVec[0],
                                            (newVec[1] - oldVec[1]) * simulationTimeFraction + oldVec[1],
                                            (newVec[2] - oldVec[2]) * simulationTimeFraction + oldVec[2]);

            currentSituation.setPosition(currentPosition);

            // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
            // avoid underflow below ground elevation by using getCorrectedAltitude
            const CAltitude oldAlt(oldSituation.getCorrectedAltitude());
            const CAltitude newAlt(newSituation.getCorrectedAltitude());
            Q_ASSERT_X(oldAlt.getReferenceDatum() == CAltitude::MeanSeaLevel && oldAlt.getReferenceDatum() == newAlt.getReferenceDatum(), Q_FUNC_INFO, "mismatch in reference"); // otherwise no calculation is possible
            currentSituation.setAltitude(CAltitude((newAlt - oldAlt)
                                                   * simulationTimeFraction
                                                   + oldAlt,
                                                   oldAlt.getReferenceDatum()));

            // Update current position by hints' elevation
            // * for XP provided by hints.getElevationProvider at current position
            // * for FSX/P3D provided as hints.getElevation which is set to current position of remote aircraft in simulator
            // As XP uses lazy init we will call getGroundElevation only when needed, so default here via getElevationPlane
            CAltitude currentGroundElevation(hints.getElevationPlane().getAltitudeIfWithinRadius(currentSituation));

            // Interpolate between altitude and ground elevation, with proportions weighted according to interpolated onGround flag
            if (hints.hasAircraftParts())
            {
                const double groundFactor = hints.getAircraftParts().isOnGroundInterpolated();
                log.groundFactor = groundFactor;
                if (groundFactor > 0.0)
                {
                    currentGroundElevation = hints.getGroundElevation(currentSituation); // calls provider on XP
                    if (!currentGroundElevation.isNull())
                    {
                        Q_ASSERT_X(currentGroundElevation.getReferenceDatum() == CAltitude::MeanSeaLevel, Q_FUNC_INFO, "Need MSL value");
                        currentSituation.setAltitude(CAltitude(currentSituation.getAltitude() * (1.0 - groundFactor)
                                                               + currentGroundElevation * groundFactor,
                                                               oldAlt.getReferenceDatum()));
                    }
                }
                currentSituation.setGroundElevation(currentGroundElevation);
                IInterpolator::setGroundFlagFromInterpolator(hints, groundFactor, currentSituation);
            }
            else
            {
                // guess ground flag
                constexpr double NoGroundFactor = -1;
                currentSituation.setGroundElevation(currentGroundElevation);
                IInterpolator::setGroundFlagFromInterpolator(hints, NoGroundFactor, currentSituation);
            }

            // full interpolation?
            if (setup.isForcingFullInterpolation() || hints.isVtolAircraft() || newVec != oldVec || oldAlt != newAlt)
            {
                // HINT: VTOL aircraft can change pitch/bank without changing position, planes cannot
                // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
                const CHeading headingBegin = oldSituation.getHeading();
                CHeading headingEnd = newSituation.getHeading();

                if ((headingEnd - headingBegin).value(CAngleUnit::deg()) < -180)
                {
                    headingEnd += CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
                }

                if ((headingEnd - headingBegin).value(CAngleUnit::deg()) > 180)
                {
                    headingEnd -= CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
                }

                currentSituation.setHeading(CHeading((headingEnd - headingBegin)
                                                     * simulationTimeFraction
                                                     + headingBegin,
                                                     headingBegin.getReferenceNorth()));

                // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
                const CAngle pitchBegin = oldSituation.getPitch();
                const CAngle pitchEnd = newSituation.getPitch();
                const CAngle pitch = (pitchEnd - pitchBegin) * simulationTimeFraction + pitchBegin;
                currentSituation.setPitch(pitch);

                // Interpolate bank: Bank = (BankB - BankA) * t + BankA
                const CAngle bankBegin = oldSituation.getBank();
                const CAngle bankEnd = newSituation.getBank();
                const CAngle bank = (bankEnd - bankBegin) * simulationTimeFraction + bankBegin;
                currentSituation.setBank(bank);

                currentSituation.setGroundSpeed((newSituation.getGroundSpeed() - oldSituation.getGroundSpeed())
                                                * simulationTimeFraction
                                                + oldSituation.getGroundSpeed());

                status.setChangedPosition(true);
            }
            status.setInterpolationSucceeded(true);
            if (hints.isLoggingInterpolation())
            {
                log.timestamp = currentTimeMsSinceEpoc;
                log.callsign = callsign;
                log.vtolAircraft = hints.isVtolAircraft();
                log.currentSituation = currentSituation;
                log.oldSituation = oldSituation;
                log.newSituation = newSituation;
                log.useParts = hints.hasAircraftParts();
                log.parts = hints.getAircraftParts();
                this->logInterpolation(log);
            }

            return currentSituation;
        }
    } // namespace
} // namespace
