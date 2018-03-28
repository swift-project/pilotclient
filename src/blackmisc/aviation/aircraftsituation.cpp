/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituation::CAircraftSituation() {}

        CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign) : m_correspondingCallsign(correspondingCallsign)
        {}

        CAircraftSituation::CAircraftSituation(const CCoordinateGeodetic &position, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs, const CElevationPlane &groundElevation)
            : m_position(position), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs), m_groundElevationPlane(groundElevation)
        {
            m_pressureAltitude = position.geodeticHeight().toPressureAltitude(CPressure(1013.25, CPressureUnit::mbar()));
        }

        CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign, const CCoordinateGeodetic &position, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs, const CElevationPlane &groundElevation)
            : m_correspondingCallsign(correspondingCallsign),
              m_position(position), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs), m_groundElevationPlane(groundElevation)
        {
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
            m_pressureAltitude = position.geodeticHeight().toPressureAltitude(CPressure(1013.25, CPressureUnit::mbar()));
        }

        QString CAircraftSituation::convertToQString(bool i18n) const
        {
            return QStringLiteral("ts: ") % this->getFormattedTimestampAndOffset(true) %
                   QStringLiteral(" ") % m_position.toQString(i18n) %
                   QStringLiteral(" bank: ") % (m_bank.toQString(i18n)) %
                   QStringLiteral(" pitch: ") % (m_pitch.toQString(i18n)) %
                   QStringLiteral(" heading: ") % (m_heading.toQString(i18n)) %
                   QStringLiteral(" og: ") % this->getOnGroundInfo() %
                   QStringLiteral(" gs: ") % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::kts(), 1, true) %
                   QStringLiteral(" ") % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::m_s(), 1, true) %
                   QStringLiteral(" elevation: ") % (m_groundElevationPlane.toQString(i18n));
        }

        const QString &CAircraftSituation::isOnGroundToString(CAircraftSituation::IsOnGround onGround)
        {
            static const QString notog("not on ground");
            static const QString og("on ground");
            static const QString unknown("unknown");

            switch (onGround)
            {
            case CAircraftSituation::NotOnGround: return notog;
            case CAircraftSituation::OnGround: return og;
            case CAircraftSituation::OnGroundSituationUnknown:
            default: return unknown;
            }
        }

        const QString &CAircraftSituation::onGroundDetailsToString(CAircraftSituation::OnGroundDetails reliability)
        {
            static const QString intElv("elevation");
            static const QString intElvCg("elevation/CG");
            static const QString intInter("interpolation");
            static const QString intGuess("guessing");
            static const QString unknown("unknown");
            static const QString outOwnAircraft("own aircraft");
            static const QString inNetwork("from network");
            static const QString inFromParts("from parts");
            static const QString InNoGroundInfo("no gnd.info");

            switch (reliability)
            {
            case CAircraftSituation::OnGroundByElevation: return intElv;
            case CAircraftSituation::OnGroundByElevationAndCG: return intElvCg;
            case CAircraftSituation::OnGroundByGuessing: return intGuess;
            case CAircraftSituation::OnGroundByInterpolation: return intInter;
            case CAircraftSituation::OutOnGroundOwnAircraft: return outOwnAircraft;
            case CAircraftSituation::InFromNetwork: return inNetwork;
            case CAircraftSituation::InFromParts: return inFromParts;
            case CAircraftSituation::InNoGroundInfo: return InNoGroundInfo;
            case CAircraftSituation::NotSet:
            default: return unknown;
            }
        }

        const CLength &CAircraftSituation::deltaNearGround()
        {
            static const CLength small(0.5, CLengthUnit::m());
            return small;
        }

        const CAircraftSituation &CAircraftSituation::null()
        {
            static const CAircraftSituation n;
            return n;
        }

        const CLength &CAircraftSituation::defaultCG()
        {
            static const CLength cg(2.5, CLengthUnit::m());
            return cg;
        }

        CVariant CAircraftSituation::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::propertyByIndex(index); }
            if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition: return m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexLatitude: return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude: return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexAltitude: return this->getAltitude().propertyByIndex(index.copyFrontRemoved());
            case IndexHeading: return m_heading.propertyByIndex(index.copyFrontRemoved());
            case IndexPitch: return m_pitch.propertyByIndex(index.copyFrontRemoved());
            case IndexBank: return m_bank.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundSpeed: return m_groundSpeed.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundElevationPlane: return m_groundElevationPlane.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign: return m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexIsOnGround: return CVariant::fromValue(m_onGround);
            case IndexIsOnGroundString: return CVariant::fromValue(this->onGroundAsString());
            case IndexOnGroundReliability: return CVariant::fromValue(m_onGroundDetails);
            case IndexOnGroundReliabilityString: return CVariant::fromValue(this->getOnDetailsAsString());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftSituation::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftSituation>(); return; }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { ITimestampWithOffsetBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition: m_position.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexPitch: m_pitch.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexBank: m_bank.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexGroundSpeed: m_groundSpeed.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexGroundElevationPlane: m_groundElevationPlane.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCallsign: m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexIsOnGround: m_onGround = variant.toInt(); break;
            case IndexOnGroundReliability: m_onGroundDetails = variant.toInt(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftSituation::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftSituation &compareValue) const
        {
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue); }
            if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition: return m_position.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPosition());
            case IndexAltitude: return this->getAltitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAltitude());
            case IndexPitch: return m_pitch.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPitch());
            case IndexBank: return m_bank.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getBank());
            case IndexGroundSpeed: return m_groundSpeed.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundSpeed());
            case IndexGroundElevationPlane: return m_groundElevationPlane.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundElevationPlane());
            case IndexCallsign: return m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexIsOnGround:
            case IndexIsOnGroundString:
                return Compare::compare(m_onGround, compareValue.m_onGround);
            case IndexOnGroundReliability:
            case IndexOnGroundReliabilityString:
                return Compare::compare(m_onGroundDetails, compareValue.m_onGroundDetails);
            default: break;
            }
            const QString assertMsg("No comparison for index " + index.toQString());
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(assertMsg));
            return 0;
        }

        bool CAircraftSituation::isNull() const
        {
            return this->isPositionNull();
        }

        void CAircraftSituation::setNull()
        {
            m_position.setNull();
            m_pressureAltitude.setNull();
            m_heading.setNull();
            m_pitch.setNull();
            m_bank.setNull();
            m_groundElevationPlane.setNull();
            m_groundSpeed.setNull();
            m_onGroundDetails = CAircraftSituation::NotSet;
        }

        const QString &CAircraftSituation::onGroundAsString() const
        {
            return CAircraftSituation::isOnGroundToString(this->getOnGround());
        }

        bool CAircraftSituation::isOnGroundInfoAvailable() const
        {
            return this->getOnGround() != CAircraftSituation::OnGroundSituationUnknown &&
                   this->getOnGroundDetails() != CAircraftSituation::NotSet;
        }

        void CAircraftSituation::setOnGround(bool onGround)
        {
            this->setOnGround(onGround ? OnGround : NotOnGround);
        }

        void CAircraftSituation::setOnGround(CAircraftSituation::IsOnGround onGround)
        {
            m_onGround = static_cast<int>(onGround);
            m_onGroundFactor = (onGround == OnGround) ?  1.0 : 0.0;
        }

        void CAircraftSituation::setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundDetails details)
        {
            this->setOnGround(onGround);
            this->setOnGroundDetails(details);
        }

        void CAircraftSituation::setOnGroundFactor(double groundFactor)
        {
            double gf = groundFactor;
            do
            {
                if (groundFactor < 0.0)   { gf = -1.0; break; }
                if (groundFactor < 0.001) { gf =  0.0; break; }
                if (groundFactor > 0.999) { gf =  1.0; break; }
            }
            while (false);
            m_onGroundFactor = gf;
        }

        bool CAircraftSituation::guessOnGround(bool vtol, const PhysicalQuantities::CLength &cg)
        {
            if (this->getOnGroundDetails() == NotSet) { return false; }
            IsOnGround og = this->isOnGroundByElevation(cg);
            if (og != OnGroundSituationUnknown)
            {
                this->setOnGround(og, CAircraftSituation::OnGroundByGuessing);
                return true;
            }

            // we guess on speed, pitch and bank by excluding situations
            this->setOnGround(CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByGuessing);
            if (qAbs(this->getPitch().value(CAngleUnit::deg())) > 10)  { return true; }
            if (qAbs(this->getBank().value(CAngleUnit::deg())) > 10)   { return true; }
            if (this->getGroundSpeed().value(CSpeedUnit::km_h()) > 50) { return true; }

            // on VTOL we stop here
            if (vtol) { return false; }

            // not sure, but this is a guess
            this->setOnGround(CAircraftSituation::OnGround, CAircraftSituation::OnGroundByGuessing);
            return true;
        }

        CLength CAircraftSituation::getGroundDistance(const CLength &centerOfGravity) const
        {
            if (centerOfGravity.isNull() || !this->hasGroundElevation()) { return CLength::null(); }
            const CAltitude groundPlusCG = this->getGroundElevation().withOffset(centerOfGravity);
            const CLength groundDistance = (this->getAltitude() - groundPlusCG);
            return groundDistance;
        }

        const QString &CAircraftSituation::getOnDetailsAsString() const
        {
            return CAircraftSituation::onGroundDetailsToString(this->getOnGroundDetails());
        }

        bool CAircraftSituation::setOnGroundFromGroundFactorFromInterpolation(double threshold)
        {
            this->setOnGroundDetails(OnGroundByInterpolation);
            if (this->getOnGroundFactor() < 0.0)
            {
                this->setOnGround(NotSet);
                return false;
            }

            // set on ground but leave factor untouched
            const bool og = this->getOnGroundFactor() > threshold; // 1.0 means on ground
            m_onGround = og ? OnGround : NotOnGround;
            return true;
        }

        bool CAircraftSituation::setOnGroundByUnderflowDetection(const CLength &cg)
        {
            IsOnGround og = this->isOnGroundByElevation(cg);
            if (og == OnGroundSituationUnknown) { return false; }
            this->setOnGround(og, OnGroundByElevationAndCG);
            return true;
        }

        QString CAircraftSituation::getOnGroundInfo() const
        {
            return this->onGroundAsString() % QLatin1Char(' ') % this->getOnDetailsAsString();
        }

        CAircraftSituation::IsOnGround CAircraftSituation::isOnGroundByElevation(const CLength &cg) const
        {
            const CLength groundDistance = this->getGroundDistance(cg);
            if (groundDistance.isNull()) { return OnGroundSituationUnknown; }
            if (groundDistance.isNegativeWithEpsilonConsidered() || groundDistance.abs() < deltaNearGround()) { return OnGround; }
            return NotOnGround;
        }

        bool CAircraftSituation::hasGroundElevation() const
        {
            return !this->getGroundElevation().isNull();
        }

        bool CAircraftSituation::hasInboundGroundInformation() const
        {
            return this->getOnGroundDetails() == CAircraftSituation::InFromParts || this->getOnGroundDetails() == CAircraftSituation::InFromNetwork;
        }

        void CAircraftSituation::setGroundElevation(const CAltitude &altitude)
        {
            if (altitude.isNull())
            {
                m_groundElevationPlane = CElevationPlane::null();
            }
            else
            {
                m_groundElevationPlane = CElevationPlane(*this);
                m_groundElevationPlane.setSinglePointRadius();
                m_groundElevationPlane.setGeodeticHeight(altitude);
            }
        }

        bool CAircraftSituation::setGroundElevationChecked(const CElevationPlane &elevationPlane)
        {
            if (elevationPlane.isNull()) { return false; }
            const CLength distance =  this->calculateGreatCircleDistance(elevationPlane);
            if (distance > elevationPlane.getRadius()) { return false; }
            if (m_groundElevationPlane.isNull() || distance < m_groundElevationPlane.getRadius())
            {
                // better values
                m_groundElevationPlane = elevationPlane;
                m_groundElevationPlane.setRadius(distance);
                return true;
            }
            return false;
        }

        const CLength &CAircraftSituation::getGroundElevationDistance() const
        {
            if (!this->hasGroundElevation()) { return CLength::null(); }
            return m_groundElevationPlane.getRadius();
        }

        CLength CAircraftSituation::getHeightAboveGround() const
        {
            if (this->getAltitude().isNull()) { return { 0, nullptr }; }
            if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
            {
                // we have a sure value explicitly set
                return this->getAltitude();
            }
            const CLength gh(this->getGroundElevation());
            if (gh.isNull()) { return { 0, nullptr }; }
            return this->getAltitude() - gh;
        }

        CAltitude CAircraftSituation::getCorrectedAltitude(const CLength &centerOfGravity, bool dragToGround, bool *corrected) const
        {
            if (corrected) { *corrected = false; }
            if (!this->hasGroundElevation()) { return this->getAltitude(); }

            // above ground
            if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Unsupported");
                return this->getAltitude();
            }
            else
            {
                const CAltitude groundPlusCG = this->getGroundElevation().withOffset(centerOfGravity);
                if (groundPlusCG.isNull()) { return this->getAltitude(); }
                const CLength groundDistance = this->getAltitude() - groundPlusCG;
                const bool underOrNearGround = groundDistance.isNegativeWithEpsilonConsidered() || groundDistance.abs() < deltaNearGround();
                const bool forceDragGnd = (dragToGround && this->getOnGround() == OnGround) && (this->hasInboundGroundInformation() || this->getOnGroundDetails() == OnGroundByGuessing);
                const bool toGround = underOrNearGround || forceDragGnd;
                if (!toGround) { return this->getAltitude(); }

                // underflow or overflow forced to ground
                if (corrected) { *corrected = true; }
                return groundPlusCG;
            }
        }

        void CAircraftSituation::setPressureAltitude(const CAltitude &altitude)
        {
            Q_ASSERT(altitude.getAltitudeType() == CAltitude::PressureAltitude);
            m_pressureAltitude = altitude;
        }

        bool CAircraftSituation::isMoving() const
        {
            const double gsKmh = this->getGroundSpeed().value(CSpeedUnit::km_h());
            return gsKmh >= 1.0;
        }

        bool CAircraftSituation::canLikelySkipNearGroundInterpolation() const
        {
            // those we can exclude
            if (this->isOnGround() && this->hasInboundGroundInformation()) { return false; }

            // cases where we can skip
            if (this->isNull()) { return true; }
            if (this->getGroundSpeed().value(CSpeedUnit::kts()) > 250) { return true; }

            if (this->hasGroundElevation())
            {
                static const CLength threshold(400, CLengthUnit::m());
                const CLength a = this->getHeightAboveGround();
                if (!a.isNull() && a >= threshold) { return true; } // too high for ground
            }
            return false;
        }

        CLength CAircraftSituation::getDistancePerTime(const CTime &time) const
        {
            if (this->getGroundSpeed().isNull()) { return CLength(0, CLengthUnit::nullUnit()); }
            const int ms = time.valueInteger(CTimeUnit::ms());
            return this->getDistancePerTime(ms);
        }

        CLength CAircraftSituation::getDistancePerTime(int milliseconds) const
        {
            if (this->getGroundSpeed().isNull()) { return CLength(0, CLengthUnit::nullUnit()); }
            const double seconds = milliseconds / 1000;
            const double gsMeterSecond = this->getGroundSpeed().value(CSpeedUnit::m_s());
            const CLength d(seconds * gsMeterSecond, CLengthUnit::m());
            return d;
        }

        void CAircraftSituation::setCallsign(const CCallsign &callsign)
        {
            m_correspondingCallsign = callsign;
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }

        bool CAircraftSituation::adjustGroundFlag(const CAircraftParts &parts, double timeDeviationFactor, qint64 *differenceMs)
        {
            Q_ASSERT_X(timeDeviationFactor >= 0 && timeDeviationFactor <= 1.0, Q_FUNC_INFO, "Expect 0..1");
            static const qint64 Max = std::numeric_limits<qint64>::max();
            if (differenceMs) { *differenceMs = Max; }
            if (this->getOnGroundDetails() == CAircraftSituation::InFromNetwork) { return false; }
            const qint64 d = this->getAdjustedTimeDifferenceMs(parts.getAdjustedMSecsSinceEpoch());
            const bool adjust = (d >= 0) || qAbs(d) < (timeDeviationFactor * parts.getTimeOffsetMs()); // future or past within deviation range
            if (!adjust) { return false; }

            if (differenceMs) { *differenceMs = d; }
            this->setOnGround(parts.isOnGround() ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround, CAircraftSituation::InFromParts);
            return true;
        }

        bool CAircraftSituation::adjustGroundFlag(const CAircraftPartsList &partsList, double timeDeviationFactor, qint64 *differenceMs)
        {
            Q_ASSERT_X(timeDeviationFactor >= 0 && timeDeviationFactor <= 1.0, Q_FUNC_INFO, "Expect 0..1");
            if (this->getOnGroundDetails() == CAircraftSituation::InFromNetwork) { return false; }
            if (partsList.isEmpty()) { return false; }

            static const qint64 Max = std::numeric_limits<qint64>::max();
            if (differenceMs) { *differenceMs = Max; }
            CAircraftParts bestParts;
            bool adjust = false;
            qint64 bestDistance = Max;
            for (const CAircraftParts &parts : partsList)
            {
                const qint64 d = this->getAdjustedTimeDifferenceMs(parts.getAdjustedMSecsSinceEpoch());
                const qint64 posD = qAbs(d);
                const bool candidate = (d >= 0) || posD < (timeDeviationFactor * parts.getTimeOffsetMs()); // future or past within deviation range
                if (!candidate || bestDistance <= posD) { continue; }
                bestDistance = posD;
                if (differenceMs) { *differenceMs = d; }
                adjust = true;
                bestParts = parts;
                if (bestDistance == 0) { break; }
            }
            if (!adjust) { return false; }

            const CAircraftSituation::IsOnGround og = bestParts.isOnGround() ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround;
            this->setOnGround(og, CAircraftSituation::InFromParts);
            return true;
        }
    } // namespace
} // namespace
