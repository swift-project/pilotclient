/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodel.h"
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

        CAircraftSituation CAircraftSituationList::frontOrNull() const
        {
            if (this->isEmpty()) { return CAircraftSituation::null(); }
            return this->front();
        }

        CAircraftSituation CAircraftSituationList::backOrNull() const
        {
            if (this->isEmpty()) { return CAircraftSituation::null(); }
            return this->back();
        }

        CAircraftSituation CAircraftSituationList::indexOrNull(int index) const
        {
            if (this->size() > index) { return (*this)[index]; }
            return CAircraftSituation::null();
        }

        int CAircraftSituationList::setGroundElevationChecked(const CElevationPlane &elevationPlane, CAircraftSituation::GndElevationInfo info, qint64 newerThanAdjustedMs)
        {
            if (elevationPlane.isNull()) { return 0; }
            int c = 0;
            for (CAircraftSituation &s : *this)
            {
                if (newerThanAdjustedMs >= 0 && s.getAdjustedMSecsSinceEpoch() <= newerThanAdjustedMs) { continue; }
                const bool set = s.setGroundElevationChecked(elevationPlane, info);
                if (set) { c++; }
            }
            return c;
        }

        int CAircraftSituationList::adjustGroundFlag(const CAircraftParts &parts, double timeDeviationFactor)
        {
            int c = 0;
            for (CAircraftSituation &situation : *this)
            {
                situation.setOnGroundDetails(CAircraftSituation::InFromParts);
                if (situation.adjustGroundFlag(parts, true, timeDeviationFactor)) { c++; };
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

        bool CAircraftSituationList::containsSituationWithoutGroundElevation() const
        {
            return this->contains(&CAircraftSituation::hasGroundElevation, false);
        }

        bool CAircraftSituationList::containsGroundElevationOutsideRange(const CLength &range) const
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

        QPair<bool, CAircraftSituation::IsOnGround> CAircraftSituationList::isGndFlagStableChanging(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return  QPair<bool, CAircraftSituation::IsOnGround>(false, CAircraftSituation::OnGroundSituationUnknown); }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            const CAircraftSituation::IsOnGround f = sorted.front().getOnGround();
            const CAircraftSituation::IsOnGround t = sorted.back().getOnGround();
            QPair<bool, CAircraftSituation::IsOnGround> ret(false, f); // changing to front (latest)
            if (f == t) { return ret; }

            bool changed = false;

            for (const CAircraftSituation &s : sorted)
            {
                if (!changed && s.getOnGround() == f) { continue; } // find 1st changing
                if (!changed) { changed = true; continue; } // just changed
                if (s.getOnGround() != t) { return ret; } // jitter, something like gnd, no gnd, gnd
            }
            ret.first = changed;
            return ret;
        }

        bool CAircraftSituationList::isJustTakingOff(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            const CAircraftSituation latest = sorted.front();
            if (latest.getOnGround() != CAircraftSituation::NotOnGround) { return false; }
            const int c = this->countOnGround(CAircraftSituation::OnGround);
            return this->size() - 1 == c; // all others on ground
        }

        bool CAircraftSituationList::isJustTouchingDown(bool alreadySortedLatestFirst) const
        {
            if (this->size() < 2) { return false; }

            const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
            const CAircraftSituation latest = sorted.front();
            if (latest.getOnGround() != CAircraftSituation::OnGround) { return false; }
            const int c = this->countOnGround(CAircraftSituation::NotOnGround);
            return this->size() - 1 == c; // all others not on ground
        }

        bool CAircraftSituationList::isTakingOff(bool alreadySortedLatestFirst) const
        {
            const QPair<bool, CAircraftSituation::IsOnGround> r = this->isGndFlagStableChanging(alreadySortedLatestFirst);
            return r.first && r.second == CAircraftSituation::NotOnGround;
        }

        bool CAircraftSituationList::isTouchingDown(bool alreadySortedLatestFirst) const
        {
            const QPair<bool, CAircraftSituation::IsOnGround> r = this->isGndFlagStableChanging(alreadySortedLatestFirst);
            return r.first && r.second == CAircraftSituation::OnGround;
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

        bool CAircraftSituationList::containsOnGroundFromNetwork() const
        {
            return this->contains(&CAircraftSituation::isOnGroundFromNetwork, true);
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

        CAircraftSituation CAircraftSituationList::findClosestElevationWithinRange(const ICoordinateGeodetic &coordinate, const CLength &range) const
        {
            const CLength r = range.isNull() || range < CElevationPlane::singlePointRadius() ? CElevationPlane::singlePointRadius() : range;
            CAircraftSituation situationWithElevation = CAircraftSituation::null();

            CLength bestDistance = CLength::null();
            for (const CAircraftSituation &s : *this)
            {
                if (!s.hasGroundElevation()) { continue; }

                // we need to calculate distance to coordinates of the plane
                // not the situation using the coordinate
                // const CLength distance = s.calculateGreatCircleDistance(coordinate);
                const CLength distance = s.getGroundElevationPlane().calculateGreatCircleDistance(coordinate);

                if (distance > r) { continue; }
                if (bestDistance.isNull() || bestDistance > distance)
                {
                    situationWithElevation = s;
                    bestDistance = distance;
                }
            }
            return situationWithElevation;
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

        int CAircraftSituationList::addAltitudeOffset(const CLength &offset)
        {
            if (offset.isNull() || this->isEmpty()) { return 0; }
            int c = 0;
            for (CAircraftSituation &s : *this)
            {
                s.addAltitudeOffset(offset);
                c++;
            }
            return c;
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

        QList<double> CAircraftSituationList::groundDistanceValues(const CLengthUnit &unit, const CLength &cg) const
        {
            QList<double> values;
            for (const CAircraftSituation &s : *this)
            {
                const CLength distance(s.getGroundDistance(cg));
                if (distance.isNull()) { continue; }
                values.push_back(distance.value(unit));
            }
            return values;
        }

        CSpeedPair CAircraftSituationList::groundSpeedStandardDeviationAndMean() const
        {
            const QList<double> gsValues = this->groundSpeedValues(CSpeedUnit::kts());
            if (gsValues.size() != this->size()) { return QPair<CSpeed, CSpeed>(CSpeed::null(), CSpeed::null()); }
            const QPair<double, double> gsKts = CMathUtils::standardDeviationAndMean(gsValues);
            return CSpeedPair(CSpeed(gsKts.first, CSpeedUnit::kts()), CSpeed(gsKts.second, CSpeedUnit::kts()));
        }

        CAnglePair CAircraftSituationList::pitchStandardDeviationAndMean() const
        {
            const QList<double> pitchValues = this->pitchValues(CAngleUnit::deg());
            if (pitchValues.size() != this->size()) { return QPair<CAngle, CAngle>(CAngle::null(), CAngle::null()); }
            const QPair<double, double> pitchDeg = CMathUtils::standardDeviationAndMean(pitchValues);
            return CAnglePair(CAngle(pitchDeg.first, CAngleUnit::deg()), CAngle(pitchDeg.second, CAngleUnit::deg()));
        }

        CAltitudePair CAircraftSituationList::elevationStandardDeviationAndMean() const
        {
            const QList<double> elvValues = this->elevationValues(CAltitude::defaultUnit());
            if (elvValues.size() != this->size()) { return CAltitudePair(CAltitude::null(), CAltitude::null()); }
            const QPair<double, double> elvFt = CMathUtils::standardDeviationAndMean(elvValues);
            return CAltitudePair(CAltitude(elvFt.first, CAltitude::MeanSeaLevel, CAltitude::defaultUnit()), CAltitude(elvFt.second, CAltitude::MeanSeaLevel, CAltitude::defaultUnit()));
        }

        CAltitudePair CAircraftSituationList::altitudeStandardDeviationAndMean() const
        {
            const QList<double> altValues = this->altitudeValues(CAltitude::defaultUnit());
            if (altValues.size() != this->size()) { return CAltitudePair(CAltitude::null(), CAltitude::null()); }
            const QPair<double, double> altFt = CMathUtils::standardDeviationAndMean(altValues);
            return CAltitudePair(CAltitude(altFt.first, CAltitude::MeanSeaLevel, CAltitude::defaultUnit()), CAltitude(altFt.second, CAltitude::MeanSeaLevel, CAltitude::defaultUnit()));
        }

        QPair<CLength, CLength> CAircraftSituationList::minMaxGroundDistance(const CLength &cg) const
        {
            const QList<double> gndDistance = this->groundDistanceValues(CAltitude::defaultUnit(), cg);
            if (gndDistance.size() != this->size()) { return QPair<CLength, CLength>(CLength::null(), CLength::null()); }
            const auto gndDistMinMax = std::minmax_element(gndDistance.constBegin(), gndDistance.constEnd());
            const double gndDistMin = *gndDistMinMax.first;
            const double gndDistMax = *gndDistMinMax.second;
            return QPair<CLength, CLength>(CLength(gndDistMin, CAltitude::defaultUnit()), CLength(gndDistMax, CAltitude::defaultUnit()));
        }

        CAltitudePair CAircraftSituationList::altitudeAglStandardDeviationAndMean() const
        {
            const QList<double> altValues = this->altitudeValues(CLengthUnit::ft());
            if (altValues.size() != this->size()) { return CAltitudePair(CAltitude::null(), CAltitude::null()); }

            const QList<double> elvValues = this->elevationValues(CLengthUnit::ft());
            if (elvValues.size() != this->size()) { return CAltitudePair(CAltitude::null(), CAltitude::null()); }

            QList<double> altElvDeltas;
            for (int i = 0; i < altValues.size(); i++)
            {
                const double delta = altValues[i] - elvValues[i];
                altElvDeltas.push_back(delta);
            }
            const QPair<double, double> deltaFt = CMathUtils::standardDeviationAndMean(altElvDeltas);
            return CAltitudePair(CAltitude(deltaFt.first, CAltitude::MeanSeaLevel, CAltitude::defaultUnit()), CAltitude(deltaFt.second, CAltitude::MeanSeaLevel, CAltitude::defaultUnit()));
        }

        int CAircraftSituationList::transferElevationForward(const CLength &radius)
        {
            if (this->size() < 2) { return 0; }
            Q_ASSERT_X(m_tsAdjustedSortHint == CAircraftSituationList::AdjustedTimestampLatestFirst, Q_FUNC_INFO, "need latest first");
            int c = 0;
            for (int i = 1; i < this->size(); ++i)
            {
                const CAircraftSituation &oldSituation = (*this)[i];
                CAircraftSituation &newSituation = (*this)[i - 1];
                if (oldSituation.transferGroundElevationFromMe(newSituation, radius)) { c++; }
            }
            return c;
        }

        CElevationPlane CAircraftSituationList::averageElevationOfTaxiingOnGroundAircraft(const CAircraftSituation &reference, const CLength &range, int minValues, int sufficientValues) const
        {
            if (this->size() < minValues) { return CElevationPlane::null(); } // no change to succeed

            const CAircraftSituationList sorted = this->findWithGeodeticMSLHeight().findWithinRange(reference, range).sortedByEuclideanDistanceSquared(reference);
            if (sorted.size() < minValues) { return CElevationPlane::null(); }
            QList<double> valuesInFt;
            for (const CAircraftSituation &situation : *this)
            {
                if (situation.getGroundElevationInfo() != CAircraftSituation::FromProvider) { continue; }
                const bool canUse = !situation.isMoving() || (situation.isOnGroundFromNetwork() || situation.isOnGroundFromParts());
                if (!canUse) { continue; }

                const double elvFt = situation.getGroundElevationPlane().getAltitude().value(CLengthUnit::ft());
                valuesInFt.push_back(elvFt);
                if (valuesInFt.size() >= sufficientValues) { break; }
            }

            if (valuesInFt.size() < minValues) { return CElevationPlane::null(); }

            static const double MaxDevFt = CAircraftSituation::allowedAltitudeDeviation().value(CLengthUnit::ft());
            const QPair<double, double> elvStdDevMean = CMathUtils::standardDeviationAndMean(valuesInFt);
            if (elvStdDevMean.first > MaxDevFt) { return CElevationPlane::null(); }
            return CElevationPlane(reference, elvStdDevMean.second, CElevationPlane::singlePointRadius());
        }
    } // namespace
} // namespace
