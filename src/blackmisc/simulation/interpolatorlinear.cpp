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
        CAircraftSituation CInterpolatorLinear::getInterpolatedSituation(const CAircraftSituationList &situations, qint64 currentTimeMsSinceEpoc, const CInterpolationHints &hints, InterpolationStatus &status) const
        {
            // has to be thread safe
            const CInterpolationAndRenderingSetup setup = this->getInterpolatorSetup();
            status.reset();

            // any data at all?
            if (situations.isEmpty()) { return {}; }

            // data, split situations by time
            if (currentTimeMsSinceEpoc < 0) { currentTimeMsSinceEpoc = QDateTime::currentMSecsSinceEpoch(); }

            // find the first situation not in the correct order, keep only the situations before that one
            const auto end = std::is_sorted_until(situations.begin(), situations.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
            const auto validSituations = makeRange(situations.begin(), end);

            // find the first situation earlier than the current time
            const auto pivot = std::partition_point(validSituations.begin(), validSituations.end(), [ = ](auto && s) { return s.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoc; });
            const auto situationsNewer = makeRange(validSituations.begin(), pivot);
            const auto situationsOlder = makeRange(pivot, validSituations.end());

            // interpolation situations
            CAircraftSituation oldSituation;
            CAircraftSituation newSituation;
            status.setInterpolationSucceeded(true);
            status.setChangedPosition(true); //! \fixme efficiently determine whether the position has changed

            // latest first, now 00:20 split time
            // time     pos
            // 00:25    10    newer
            // 00:20    11    newer
            // <----- split
            // 00:15    12    older
            // 00:10    13    older
            // 00:05    14    older

            // The first condition covers a situation, when there are no before / after situations.
            // We just place at he last position until we get before / after situations
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
            if (!hints.getElevation().isNull())
            {
                setGroundElevationFromHint(hints, oldSituation);
                setGroundElevationFromHint(hints, newSituation);
            }

            CAircraftSituation currentSituation(oldSituation);
            CCoordinateGeodetic currentPosition;

            // Time between start and end packet
            const double deltaTime = std::abs(oldSituation.getAdjustedMSecsSinceEpoch() - newSituation.getAdjustedMSecsSinceEpoch());

            // Fraction of the deltaTime, ideally [0.0 - 1.0]
            // < 0 should not happen due to the split, > 1 can happen if new values are delayed beyond split time
            // 1) values > 1 mean extrapolation
            // 2) values > 2 mean no new situations coming in
            const double distanceToSplitTime = newSituation.getAdjustedMSecsSinceEpoch() - currentTimeMsSinceEpoc;
            const double simulationTimeFraction = 1.0 - (distanceToSplitTime / deltaTime);
            if (simulationTimeFraction > 2.0)
            {
                if (setup.showInterpolatorDebugMessages())
                {
                    CLogMessage(this).warning("Extrapolation, fraction > 1: %1 for callsign: %2") << simulationTimeFraction << oldSituation.getCallsign();
                }
            }

            const std::array<double, 3> oldVec(oldSituation.getPosition().normalVectorDouble());
            const std::array<double, 3> newVec(newSituation.getPosition().normalVectorDouble());

            // Interpolate position: pos = (posB - posA) * t + posA
            currentPosition.setNormalVector((newVec[0] - oldVec[0]) * simulationTimeFraction + oldVec[0],
                                            (newVec[1] - oldVec[1]) * simulationTimeFraction + oldVec[1],
                                            (newVec[2] - oldVec[2]) * simulationTimeFraction + oldVec[2]);

            currentSituation.setPosition(currentPosition);

            // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
            const CAltitude oldAlt(oldSituation.getCorrectedAltitude(hints.getCGAboveGround()));
            const CAltitude newAlt(newSituation.getCorrectedAltitude(hints.getCGAboveGround()));
            Q_ASSERT_X(oldAlt.getReferenceDatum() == newAlt.getReferenceDatum(), Q_FUNC_INFO, "mismatch in reference"); // otherwise no calculation is possible
            currentSituation.setAltitude(CAltitude((newAlt - oldAlt)
                                                   * simulationTimeFraction
                                                   + oldAlt,
                                                   oldAlt.getReferenceDatum()));

            // Interpolate between altitude and ground elevation, with proportions weighted according to interpolated onGround flag
            if (hints.hasAircraftParts())
            {
                const double groundFactor = hints.getAircraftParts().isOnGroundInterpolated();
                if (groundFactor > 0.0)
                {
                    const auto &groundElevation = hints.getElevationProvider();
                    if (groundElevation)
                    {
                        currentSituation.setAltitude(CAltitude(currentSituation.getAltitude() * (1.0 - groundFactor)
                                                               + groundElevation(currentSituation) * groundFactor,
                                                               oldAlt.getReferenceDatum()));
                    }
                }
            }

            if (!setup.isForcingFullInterpolation() && !hints.isVtolAircraft() && newVec == oldVec && oldAlt == newAlt)
            {
                // stop interpolation here, does not work for VTOL aircraft. We need a flag for VTOL aircraft
                return currentSituation;
            }

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
            return currentSituation;
        }
    } // namespace
} // namespace
