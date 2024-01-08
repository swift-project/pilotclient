// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/pq/speed.h"

#include <QList>

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftSituation, CAircraftSituationList)

namespace BlackMisc::Aviation
{
    CAircraftSituationList::CAircraftSituationList(const CSequence<CAircraftSituation> &other) : CSequence<CAircraftSituation>(other)
    {}

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
            situation.setOnGroundDetails(COnGroundInfo::InFromParts);
            if (situation.adjustGroundFlag(parts, true, timeDeviationFactor)) { c++; };
        }
        return c;
    }

    bool CAircraftSituationList::containsOnGroundDetails(COnGroundInfo::OnGroundDetails details) const
    {
        return std::any_of(begin(), end(), [&details](const CAircraftSituation &sit) { return sit.getOnGroundInfo().getGroundDetails() == details; });
    }

    bool CAircraftSituationList::areAllOnGroundDetailsSame(COnGroundInfo::OnGroundDetails details) const
    {
        for (const CAircraftSituation &situation : *this)
        {
            if (situation.getOnGroundInfo().getGroundDetails() != details) { return false; }
        }
        return true;
    }

    bool CAircraftSituationList::isConstOnGround() const
    {
        if (this->isEmpty()) { return false; }
        if (this->containsNullPositionOrHeight()) { return false; }
        return std::all_of(begin(), end(), [](const CAircraftSituation &situation) { return situation.isOnGround(); });
    }

    bool CAircraftSituationList::isConstNotOnGround() const
    {
        if (this->isEmpty()) { return false; }
        if (this->containsNullPositionOrHeight()) { return false; }
        return std::all_of(begin(), end(), [](const CAircraftSituation &situation) { return situation.getOnGroundInfo().getOnGround() == COnGroundInfo::NotOnGround; });
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

    QPair<bool, COnGroundInfo::IsOnGround> CAircraftSituationList::isGndFlagStableChanging(bool alreadySortedLatestFirst) const
    {
        if (this->size() < 2) { return QPair<bool, COnGroundInfo::IsOnGround>(false, COnGroundInfo::OnGroundSituationUnknown); }

        const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
        const COnGroundInfo::IsOnGround f = sorted.front().getOnGroundInfo().getOnGround();
        const COnGroundInfo::IsOnGround t = sorted.back().getOnGroundInfo().getOnGround();
        QPair<bool, COnGroundInfo::IsOnGround> ret(false, f); // changing to front (latest)
        if (f == t) { return ret; }

        bool changed = false;

        for (const CAircraftSituation &s : sorted)
        {
            if (!changed && s.getOnGroundInfo().getOnGround() == f) { continue; } // find 1st changing
            if (!changed)
            {
                changed = true;
                continue;
            } // just changed
            if (s.getOnGroundInfo().getOnGround() != t) { return ret; } // jitter, something like gnd, no gnd, gnd
        }
        ret.first = changed;
        return ret;
    }

    bool CAircraftSituationList::isJustTakingOff(bool alreadySortedLatestFirst) const
    {
        if (this->size() < 2) { return false; }

        const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
        const CAircraftSituation latest = sorted.front();
        if (latest.getOnGroundInfo().getOnGround() != COnGroundInfo::NotOnGround) { return false; }
        const int c = this->countOnGround(COnGroundInfo::OnGround);
        return this->size() - 1 == c; // all others on ground
    }

    bool CAircraftSituationList::isJustTouchingDown(bool alreadySortedLatestFirst) const
    {
        if (this->size() < 2) { return false; }

        const CAircraftSituationList sorted(alreadySortedLatestFirst ? (*this) : this->getSortedAdjustedLatestFirst());
        const CAircraftSituation latest = sorted.front();
        if (latest.getOnGroundInfo().getOnGround() != COnGroundInfo::OnGround) { return false; }
        const int c = this->countOnGround(COnGroundInfo::NotOnGround);
        return this->size() - 1 == c; // all others not on ground
    }

    bool CAircraftSituationList::isTakingOff(bool alreadySortedLatestFirst) const
    {
        const QPair<bool, COnGroundInfo::IsOnGround> r = this->isGndFlagStableChanging(alreadySortedLatestFirst);
        return r.first && r.second == COnGroundInfo::NotOnGround;
    }

    bool CAircraftSituationList::isTouchingDown(bool alreadySortedLatestFirst) const
    {
        const QPair<bool, COnGroundInfo::IsOnGround> r = this->isGndFlagStableChanging(alreadySortedLatestFirst);
        return r.first && r.second == COnGroundInfo::OnGround;
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

    int CAircraftSituationList::countOnGround(COnGroundInfo::IsOnGround og) const
    {
        return std::count_if(begin(), end(), [&og](const CAircraftSituation &situation) { return situation.getOnGroundInfo().getOnGround() == og; });
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

    void CAircraftSituationList::setOnGroundInfo(const COnGroundInfo &info)
    {
        for (CAircraftSituation &situation : *this)
        {
            situation.setOnGroundInfo(info);
        }
    }

    void CAircraftSituationList::setOnGroundDetails(COnGroundInfo::OnGroundDetails details)
    {
        for (CAircraftSituation &situation : *this)
        {
            situation.setOnGroundDetails(details);
        }
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
