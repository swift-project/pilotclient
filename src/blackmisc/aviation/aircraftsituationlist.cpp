/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/verify.h"

#include <QList>
#include <tuple>

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituationList::CAircraftSituationList()
        { }

        CAircraftSituationList::CAircraftSituationList(const CSequence<CAircraftSituation> &other) :
            CSequence<CAircraftSituation>(other)
        { }

        CAircraftSituationList::CAircraftSituationList(std::initializer_list<CAircraftSituation> il) :
            CSequence<CAircraftSituation>(il)
        { }

        int CAircraftSituationList::setGroundElevationChecked(const CElevationPlane &elevationPlane, qint64 newerThanAdjustedMs)
        {
            if (elevationPlane.isNull()) { return 0; }
            int c = 0;
            for (CAircraftSituation &s : *this)
            {
                if (newerThanAdjustedMs >= 0 && s.getAdjustedMSecsSinceEpoch() <= newerThanAdjustedMs) { continue; }
                const bool set = s.setGroundElevationChecked(elevationPlane);
                if (set) { c++; }
            }
            return c;
        }

        int CAircraftSituationList::setGroundElevationCheckedAndGuessGround(const CElevationPlane &elevationPlane, const CAircraftModel &model)
        {
            if (elevationPlane.isNull()) { return 0; }
            if (this->isEmpty()) { return 0; }

            Q_ASSERT_X(this->isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Need sorted situations without NULL positions");
            const CAircraftSituationChange change(*this, true, true);
            int c = 0;
            bool first = true;
            if (this->front().getCallsign().equalsString("AFL2353"))
            {
                c = 0;
            }

            for (CAircraftSituation &s : *this)
            {
                const bool set = s.setGroundElevationChecked(elevationPlane);
                if (set)
                {
                    // change is only valid for the latest situation
                    s.guessOnGround(first ? change : CAircraftSituationChange::null(), model);
                    c++;
                }
                first = false;
            }
            return c;
        }

        int CAircraftSituationList::adjustGroundFlag(const CAircraftParts &parts, double timeDeviationFactor)
        {
            int c = 0;
            for (CAircraftSituation &situation : *this)
            {
                situation.setOnGroundDetails(CAircraftSituation::InFromParts);
                if (situation.adjustGroundFlag(parts, timeDeviationFactor)) { c++; };
            }
            return c;
        }

        int CAircraftSituationList::extrapolateGroundFlag()
        {
            if (this->isEmpty()) { return 0; }
            CAircraftSituationList withInfo = this->findByInboundGroundInformation(true);
            withInfo.sortLatestFirst();
            if (withInfo.isEmpty()) { return 0; }
            const CAircraftSituation latest = withInfo.front();

            int c = 0;
            for (CAircraftSituation &situation : *this)
            {
                if (situation.isNewerThanAdjusted(latest))
                {
                    situation.setOnGround(latest.getOnGround(), latest.getOnGroundDetails());
                    c++;
                }
            }
            return c;
        }

        CAircraftSituationList CAircraftSituationList::findByInboundGroundInformation(bool hasGroundInfo) const
        {
            return this->findBy(&CAircraftSituation::hasInboundGroundDetails, hasGroundInfo);
        }

        bool CAircraftSituationList::hasSituationWithoutGroundElevation() const
        {
            return this->contains(&CAircraftSituation::hasGroundElevation, false);
        }

        bool CAircraftSituationList::hasGroundElevationOutsideRange(const CLength &range) const
        {
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.getGroundElevationPlane().getRadius() > range) { return true; }
            }
            return false;
        }

        bool CAircraftSituationList::containsOnGroundDetails(CAircraftSituation::OnGroundDetails details) const
        {
            return this->contains(&CAircraftSituation::getOnGroundDetails, details);
        }

        bool CAircraftSituationList::areAllOnGroundDetailsSame(CAircraftSituation::OnGroundDetails details) const
        {
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.getOnGroundDetails() != details) { return false; }
            }
            return true;
        }

        bool CAircraftSituationList::isConstOnGround() const
        {
            if (this->isEmpty()) { return false; }
            if (this->containsNullPositionOrHeight()) { return false; }
            for (const CAircraftSituation &situation : *this)
            {
                const CAircraftSituation::IsOnGround og = situation.getOnGround();
                if (og != CAircraftSituation::OnGround) { return false; }
            }
            return true;
        }

        bool CAircraftSituationList::isConstNotOnGround() const
        {
            if (this->isEmpty()) { return false; }
            if (this->containsNullPositionOrHeight()) { return false; }
            for (const CAircraftSituation &situation : *this)
            {
                const CAircraftSituation::IsOnGround og = situation.getOnGround();
                if (og != CAircraftSituation::NotOnGround) { return false; }
            }
            return true;
        }

        bool CAircraftSituationList::isConstDescending(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }
            if (this->containsNullPositionOrHeight()) { return false; }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            CAircraftSituation newerSituation = CAircraftSituation::null();
            for (const CAircraftSituation &situation : sorted)
            {
                if (!newerSituation.isNull())
                {
                    Q_ASSERT_X(situation.getAltitude().getReferenceDatum() == newerSituation.getAltitude().getReferenceDatum(), Q_FUNC_INFO, "Wrong reference");
                    const CLength delta = newerSituation.getAltitude() - situation.getAltitude();
                    if (!delta.isNegativeWithEpsilonConsidered()) { return false; }
                }
                newerSituation = situation;
            }
            return true;
        }

        bool CAircraftSituationList::isConstAscending(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }
            if (this->containsNullPositionOrHeight()) { return false; }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            CAircraftSituation newerSituation = CAircraftSituation::null();
            for (const CAircraftSituation &situation : sorted)
            {
                // latest first
                if (!newerSituation.isNull())
                {
                    Q_ASSERT_X(situation.getAltitude().getReferenceDatum() == newerSituation.getAltitude().getReferenceDatum(), Q_FUNC_INFO, "Wrong reference");
                    const CLength delta = newerSituation.getAltitude() - situation.getAltitude();
                    if (!delta.isPositiveWithEpsilonConsidered()) { return false; }
                }
                newerSituation = situation;
            }
            return true;
        }

        bool CAircraftSituationList::isConstAccelerating(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }
            if (this->containsNullPositionOrHeight()) { return false; }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            CSpeed newerGs = CSpeed::null();
            for (const CAircraftSituation &situation : sorted)
            {
                if (!newerGs.isNull())
                {
                    const CSpeed deltaSpeed = newerGs - situation.getGroundSpeed();
                    if (!deltaSpeed.isPositiveWithEpsilonConsidered()) { return false; }
                }
                newerGs = situation.getGroundSpeed();
            }
            return true;
        }

        bool CAircraftSituationList::isConstDecelarating(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }
            if (this->containsNullPositionOrHeight()) { return false; }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            CSpeed newerGs = CSpeed::null();
            for (const CAircraftSituation &situation : sorted)
            {
                if (!newerGs.isNull())
                {
                    const CSpeed deltaSpeed = newerGs - situation.getGroundSpeed();
                    if (!deltaSpeed.isNegativeWithEpsilonConsidered()) { return false; }
                }
                newerGs = situation.getGroundSpeed();
            }
            return true;
        }

        bool CAircraftSituationList::isGndFlagChanging(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }

            const CAircraftSituationList sorted(this->getLatestAdjustedTwoObjects(alreadySortedLatestFirst));
            const CAircraftSituation s1 = sorted.front();
            const CAircraftSituation s2 = sorted.back();
            return (s1.getOnGround() == CAircraftSituation::OnGround && s2.getOnGround() == CAircraftSituation::NotOnGround) ||
                   (s2.getOnGround() == CAircraftSituation::OnGround && s1.getOnGround() == CAircraftSituation::NotOnGround);
        }

        bool CAircraftSituationList::isJustTakingOff(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }

            const CAircraftSituationList sorted(this->getLatestAdjustedTwoObjects(alreadySortedLatestFirst));
            const CAircraftSituation latest = sorted.front();
            const CAircraftSituation oldest = sorted.back();
            return (latest.getOnGround() == CAircraftSituation::NotOnGround && oldest.getOnGround() == CAircraftSituation::OnGround);
        }

        bool CAircraftSituationList::isJustTouchingDown(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }

            const CAircraftSituationList sorted(this->getLatestAdjustedTwoObjects(alreadySortedLatestFirst));
            const CAircraftSituation latest = sorted.front();
            const CAircraftSituation oldest = sorted.back();
            return (latest.getOnGround() == CAircraftSituation::OnGround && oldest.getOnGround() == CAircraftSituation::NotOnGround);
        }

        bool CAircraftSituationList::isRotatingUp(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }
            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            const QList<double> pitches = sorted.pitchValues(CAngleUnit::deg());
            const QPair<double, double> stdDevAndMean = CMathUtils::standardDeviationAndMean(pitches);
            const double minRotate = stdDevAndMean.first + stdDevAndMean.second; // outside std deviation range
            const bool rotate = pitches.front() > minRotate;
            return rotate;
        }

        bool CAircraftSituationList::containsPushBack() const
        {
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.getGroundSpeed().isNegativeWithEpsilonConsidered()) { return true; }
            }
            return false;
        }

        int CAircraftSituationList::countOnGround(CAircraftSituation::IsOnGround og) const
        {
            int c = 0;
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.getOnGround() == og) { c++; }
            }
            return c;
        }

        int CAircraftSituationList::countOnGroundWithElevation(CAircraftSituation::IsOnGround og) const
        {
            int c = 0;
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.hasGroundElevation()) { continue; }
                if (situation.getOnGround() == og) { c++; }
            }
            return c;
        }

        CAircraftSituationList CAircraftSituationList::findOnGroundWithElevation(CAircraftSituation::IsOnGround og) const
        {
            CAircraftSituationList found;
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.hasGroundElevation()) { continue; }
                if (situation.getOnGround() == og) { found.push_back(situation); }
            }
            return found;
        }

        int CAircraftSituationList::setOnGround(CAircraftSituation::IsOnGround og)
        {
            int c = 0;
            for (CAircraftSituation &situation : *this)
            {
                if (situation.setOnGround(og)) { c++; }
            }
            return c;
        }

        int CAircraftSituationList::setOnGroundDetails(CAircraftSituation::OnGroundDetails details)
        {
            int c = 0;
            for (CAircraftSituation &situation : *this)
            {
                if (situation.setOnGroundDetails(details)) { c++; }
            }
            return c;
        }

        void CAircraftSituationList::addAltitudeOffset(const CLength &offset)
        {
            if (offset.isNull() || this->isEmpty()) { return; }
            for (CAircraftSituation &s : *this)
            {
                s.addAltitudeOffset(offset);
            }
        }

        bool CAircraftSituationList::isSortedAdjustedLatestFirstWithoutNullPositions() const
        {
            return this->isSortedAdjustedLatestFirst() && !this->containsNullPosition();
        }

        CAircraftSituationList CAircraftSituationList::withoutFrontSituation() const
        {
            if (this->empty()) { return CAircraftSituationList(); }
            CAircraftSituationList copy(*this);
            copy.pop_front();
            return copy;
        }

        QList<double> CAircraftSituationList::pitchValues(const CAngleUnit &unit) const
        {
            QList<double> values;
            for (const CAircraftSituation &s : *this)
            {
                values.push_back(s.getPitch().value(unit));
            }
            return values;
        }

        QList<double> CAircraftSituationList::groundSpeedValues(const CSpeedUnit &unit) const
        {
            QList<double> values;
            for (const CAircraftSituation &s : *this)
            {
                if (s.getGroundSpeed().isNull()) { continue; }
                values.push_back(s.getGroundSpeed().value(unit));
            }
            return values;
        }


        QList<double> CAircraftSituationList::elevationValues(const CLengthUnit &unit) const
        {
            QList<double> values;
            for (const CAircraftSituation &s : *this)
            {
                if (s.getGroundElevation().isNull()) { continue; }
                values.push_back(s.getGroundElevation().value(unit));
            }
            return values;
        }

        QList<double> CAircraftSituationList::altitudeValues(const CLengthUnit &unit) const
        {
            QList<double> values;
            for (const CAircraftSituation &s : *this)
            {
                const CAltitude alt(s.getAltitude());
                if (alt.isNull()) { continue; }
                values.push_back(alt.value(unit));
            }
            return values;
        }

        QList<double> CAircraftSituationList::correctedAltitudeValues(const CLengthUnit &unit, const CLength &cg) const
        {
            QList<double> values;
            for (const CAircraftSituation &s : *this)
            {
                const CAltitude alt(s.getCorrectedAltitude(cg));
                if (alt.isNull()) { continue; }
                values.push_back(alt.value(unit));
            }
            return values;
        }

        QPair<CSpeed, CSpeed> CAircraftSituationList::groundSpeedStandardDeviationAndMean() const
        {
            const QList<double> gsValues = this->groundSpeedValues(CSpeedUnit::kts());
            if (gsValues.size() != this->size()) { return QPair<CSpeed, CSpeed>(CSpeed::null(), CSpeed::null()); }
            const QPair<double, double> gsKts = CMathUtils::standardDeviationAndMean(gsValues);
            return QPair<CSpeed, CSpeed>(CSpeed(gsKts.first, CSpeedUnit::kts()), CSpeed(gsKts.second, CSpeedUnit::kts()));
        }

        QPair<CAngle, CAngle> CAircraftSituationList::pitchStandardDeviationAndMean() const
        {
            const QList<double> pitchValues = this->pitchValues(CAngleUnit::deg());
            if (pitchValues.size() != this->size()) { return QPair<CAngle, CAngle>(CAngle::null(), CAngle::null()); }
            const QPair<double, double> pitchDeg = CMathUtils::standardDeviationAndMean(pitchValues);
            return QPair<CAngle, CAngle>(CAngle(pitchDeg.first, CAngleUnit::deg()), CAngle(pitchDeg.second, CAngleUnit::deg()));
        }

        QPair<CAltitude, CAltitude> CAircraftSituationList::elevationStandardDeviationAndMean() const
        {
            const QList<double> elvValues = this->elevationValues(CLengthUnit::ft());
            if (elvValues.size() != this->size()) { return QPair<CAltitude, CAltitude>(CAltitude::null(), CAltitude::null()); }
            const QPair<double, double> elvFt = CMathUtils::standardDeviationAndMean(elvValues);
            return QPair<CAltitude, CAltitude>(CAltitude(elvFt.first, CAltitude::MeanSeaLevel, CLengthUnit::ft()), CAltitude(elvFt.second, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
        }

        QPair<CAltitude, CAltitude> CAircraftSituationList::altitudeStandardDeviationAndMean() const
        {
            const QList<double> altValues = this->altitudeValues(CLengthUnit::ft());
            if (altValues.size() != this->size()) { return QPair<CAltitude, CAltitude>(CAltitude::null(), CAltitude::null()); }
            const QPair<double, double> altFt = CMathUtils::standardDeviationAndMean(altValues);
            return QPair<CAltitude, CAltitude>(CAltitude(altFt.first, CAltitude::MeanSeaLevel, CLengthUnit::ft()), CAltitude(altFt.second, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
        }

        QPair<CAltitude, CAltitude> CAircraftSituationList::altitudeAglStandardDeviationAndMean() const
        {
            const QList<double> altValues = this->altitudeValues(CLengthUnit::ft());
            if (altValues.size() != this->size()) { return QPair<CAltitude, CAltitude>(CAltitude::null(), CAltitude::null()); }

            const QList<double> elvValues = this->elevationValues(CLengthUnit::ft());
            if (elvValues.size() != this->size()) { return QPair<CAltitude, CAltitude>(CAltitude::null(), CAltitude::null()); }

            QList<double> altElvDeltas;
            for (int i = 0; i < altValues.size(); i++)
            {
                const double delta = altValues[i] - elvValues[i];
                altElvDeltas.push_back(delta);
            }
            const QPair<double, double> deltaFt = CMathUtils::standardDeviationAndMean(altElvDeltas);
            return QPair<CAltitude, CAltitude>(CAltitude(deltaFt.first, CAltitude::MeanSeaLevel, CLengthUnit::ft()), CAltitude(deltaFt.second, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
        }
    } // namespace
} // namespace
