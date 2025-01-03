// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftsituation.h"

#include <QtGlobal>

#include "QStringBuilder"

#include "config/buildconfig.h"
#include "misc/aviation/aircraftlights.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/comparefunctions.h"
#include "misc/geo/elevationplane.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/propertyindexvariantmap.h" // needed for mixin::Index::apply
#include "misc/stringutils.h"
#include "misc/verify.h"

using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::config;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftSituation)

namespace swift::misc::aviation
{
    const CLength &CAircraftSituation::allowedAltitudeDeviation()
    {
        // approx. 1 meter
        static const CLength allowedStdDev(3, CLengthUnit::ft());
        return allowedStdDev;
    }

    CAircraftLights CAircraftSituation::guessLights() const
    {
        const bool isOnGround = this->isOnGround();
        const double gsKts = getGroundSpeed().value(CSpeedUnit::kts());
        CAircraftLights lights;
        lights.setCabinOn(true);
        lights.setRecognitionOn(true);

        // when first detected moving, lights on
        if (isOnGround)
        {
            lights.setTaxiOn(true);
            lights.setBeaconOn(true);
            lights.setNavOn(true);

            if (gsKts > 30)
            {
                // mode accelaration for takeoff
                lights.setTaxiOn(false);
                lights.setLandingOn(true);
            }
            else if (gsKts > 5)
            {
                // mode taxi
                lights.setTaxiOn(true);
                lights.setLandingOn(false);
            }
            else
            {
                // slow movements or parking
                lights.setTaxiOn(false);
                lights.setLandingOn(false);
            }
        }
        else
        {
            // not on ground
            lights.setTaxiOn(false);
            lights.setBeaconOn(true);
            lights.setNavOn(true);
            // landing lights for < 10000ft (normally MSL, here ignored)
            lights.setLandingOn(getAltitude().value(CLengthUnit::ft()) < 10000);
        }
        return lights;
    }

    void CAircraftSituation::registerMetadata()
    {
        CValueObject<CAircraftSituation>::registerMetadata();
        qRegisterMetaType<CAircraftSituation::AltitudeCorrection>();
        qRegisterMetaType<CAircraftSituation::GndElevationInfo>();
    }

    CAircraftSituation::CAircraftSituation() {}

    CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign)
        : m_correspondingCallsign(correspondingCallsign)
    {}

    CAircraftSituation::CAircraftSituation(const CCoordinateGeodetic &position, const CHeading &heading,
                                           const CAngle &pitch, const CAngle &bank, const CSpeed &gs,
                                           const CElevationPlane &groundElevation)
        : m_position(position), m_groundElevationPlane(groundElevation),
          m_heading(heading.normalizedToPlusMinus180Degrees()), m_pitch(pitch.normalizedToPlusMinus180Degrees()),
          m_bank(bank.normalizedToPlusMinus180Degrees()), m_groundSpeed(gs)
    {
        m_pressureAltitude = position.geodeticHeight().toPressureAltitude(CPressure(1013.25, CPressureUnit::mbar()));
    }

    CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign, const CCoordinateGeodetic &position,
                                           const CHeading &heading, const CAngle &pitch, const CAngle &bank,
                                           const CSpeed &gs, const CElevationPlane &groundElevation)
        : m_correspondingCallsign(correspondingCallsign), m_position(position), m_groundElevationPlane(groundElevation),
          m_heading(heading.normalizedToPlusMinus180Degrees()), m_pitch(pitch.normalizedToPlusMinus180Degrees()),
          m_bank(bank.normalizedToPlusMinus180Degrees()), m_groundSpeed(gs)
    {
        m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        m_pressureAltitude = position.geodeticHeight().toPressureAltitude(CPressure(1013.25, CPressureUnit::mbar()));
    }

    QString CAircraftSituation::convertToQString(bool i18n) const
    {
        return u"ts: " % this->getFormattedTimestampAndOffset(true) % u" | " % m_position.toQString(i18n) %
               u" | alt: " % this->getAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 1) % u' ' %
               this->getCorrectedAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 1) % u"[cor] | og: " %
               this->getOnGroundInfo().toQString(i18n) % u" | CG: " %
               (m_cg.isNull() ? QStringLiteral("null") :
                                m_cg.valueRoundedWithUnit(CLengthUnit::m(), 1) % u' ' %
                                    m_cg.valueRoundedWithUnit(CLengthUnit::ft(), 1)) %
               u" | skip ng: " % boolToYesNo(this->canLikelySkipNearGroundInterpolation()) % u" | bank: " %
               m_bank.toQString(i18n) % u" | pitch: " % m_pitch.toQString(i18n) % u" | heading: " %
               m_heading.toQString(i18n) % u" | GS: " % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::kts(), 1, true) %
               u' ' % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::m_s(), 1, true) % u" | elevation [" %
               this->getGroundElevationInfoAsString() % u"]: " % (m_groundElevationPlane.toQString(i18n));
    }

    const QString &CAircraftSituation::altitudeCorrectionToString(CAircraftSituation::AltitudeCorrection correction)
    {
        static const QString under("underflow");
        static const QString dragged("dragged to gnd");
        static const QString no("no correction");
        static const QString noElv("no elv.");
        static const QString unknown("unknown");
        static const QString agl("AGL");
        switch (correction)
        {
        case Underflow: return under;
        case DraggedToGround: return dragged;
        case NoElevation: return noElv;
        case NoCorrection: return no;
        case AGL: return agl;
        default: break;
        }
        return unknown;
    }

    bool CAircraftSituation::isCorrectedAltitude(CAircraftSituation::AltitudeCorrection correction)
    {
        switch (correction)
        {
        case Underflow:
        case DraggedToGround: return true;
        case NoElevation:
        case NoCorrection:
        case AGL:
        default: break;
        }
        return false;
    }

    const QString &CAircraftSituation::gndElevationInfoToString(GndElevationInfo details)
    {
        static const QString noDetails("no details");
        static const QString unknown("unknown");
        static const QString provider("provider");
        static const QString change("situation change");
        static const QString cache("cached");
        static const QString test("test");
        static const QString interpolated("interpolated");
        static const QString extrapolated("extrapolated");
        static const QString avg("average");

        switch (details)
        {
        case NoElevationInfo: return noDetails;
        case FromProvider: return provider;
        case SituationChange: return change;
        case FromCache: return cache;
        case Test: return test;
        case Interpolated: return interpolated;
        case Extrapolated: return extrapolated;
        case Average: return avg;
        default: break;
        }
        return unknown;
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

    CElevationPlane CAircraftSituation::interpolatedElevation(const CAircraftSituation &situation,
                                                              const CAircraftSituation &oldSituation,
                                                              const CAircraftSituation &newSituation,
                                                              const CLength &distance)
    {
        if (oldSituation.isNull() || newSituation.isNull()) { return CElevationPlane::null(); }
        if (!oldSituation.hasGroundElevation() || !newSituation.hasGroundElevation())
        {
            return CElevationPlane::null();
        }
        if (oldSituation.equalNormalVectorDouble(newSituation)) { return newSituation.getGroundElevationPlane(); }

        const double newElvFt = newSituation.getGroundElevation().value(CLengthUnit::ft());
        const double oldElvFt = oldSituation.getGroundElevation().value(CLengthUnit::ft());
        const double deltaElvFt = newElvFt - oldElvFt;
        if (deltaElvFt > MaxDeltaElevationFt)
        {
            return CElevationPlane::null();
        } // threshold, interpolation not possible

        if (!situation.isNull())
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                Q_ASSERT_X(situation.isValidVectorRange(), Q_FUNC_INFO, "Invalid range");
                Q_ASSERT_X(oldSituation.isValidVectorRange(), Q_FUNC_INFO, "Invalid range");
                Q_ASSERT_X(newSituation.isValidVectorRange(), Q_FUNC_INFO, "Invalid range");
            }

            const double distanceSituationNewM =
                situation.calculateGreatCircleDistance(newSituation).value(CLengthUnit::m());
            if (distanceSituationNewM < 5.0) { return newSituation.getGroundElevationPlane(); }

            const double distanceOldNewM =
                (distance.isNull() ? oldSituation.calculateGreatCircleDistance(newSituation) : distance)
                    .value(CLengthUnit::m());
            if (distanceOldNewM < 5.0) { return oldSituation.getGroundElevationPlane(); }

            const double distRatio = distanceSituationNewM / distanceOldNewM;

            // very close to the situations we return their elevation
            if (distRatio < 0.05) { return newSituation.getGroundElevationPlane(); }
            if (distRatio > 0.95) { return oldSituation.getGroundElevationPlane(); }

            const double situationElvFt = newElvFt - distRatio * deltaElvFt;
            return CElevationPlane(situation, situationElvFt, CElevationPlane::singlePointRadius());
        }
        else
        {
            const double elvSumFt = oldElvFt + newElvFt;
            const double elvFt = 0.5 * elvSumFt;
            return CElevationPlane(newSituation, elvFt, CElevationPlane::singlePointRadius());
        }
    }

    QVariant CAircraftSituation::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            return ITimestampWithOffsetBased::propertyByIndex(index);
        }
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
        case IndexPBHInfo: return QVariant::fromValue(this->getPBHInfo());
        case IndexVelocity: return QVariant::fromValue(this->getVelocity());
        case IndexBank: return m_bank.propertyByIndex(index.copyFrontRemoved());
        case IndexCG: return m_cg.propertyByIndex(index.copyFrontRemoved());
        case IndexGroundSpeed: return m_groundSpeed.propertyByIndex(index.copyFrontRemoved());
        case IndexGroundElevationPlane: return m_groundElevationPlane.propertyByIndex(index.copyFrontRemoved());
        case IndexCallsign: return m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
        case IndexIsOnGroundInfo: return m_onGroundInfo.propertyByIndex(index.copyFrontRemoved());
        case IndexGroundElevationInfo: return QVariant::fromValue(this->getGroundElevationInfo());
        case IndexGroundElevationInfoTransferred: return QVariant::fromValue(this->isGroundElevationInfoTransferred());
        case IndexGroundElevationInfoString: return QVariant::fromValue(this->getGroundElevationInfoAsString());
        case IndexGroundElevationPlusInfo: return QVariant::fromValue(this->getGroundElevationAndInfo());
        case IndexCanLikelySkipNearGroundInterpolation:
            return QVariant::fromValue(this->canLikelySkipNearGroundInterpolation());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAircraftSituation::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAircraftSituation>();
            return;
        }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            ITimestampWithOffsetBased::setPropertyByIndex(index, variant);
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPosition: m_position.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexPitch: m_pitch.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexBank: m_bank.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexVelocity: m_velocity.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexCG: m_cg.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexGroundSpeed: m_groundSpeed.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexGroundElevationPlane:
            m_groundElevationPlane.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        case IndexCallsign: m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexIsOnGroundInfo: m_onGroundInfo.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexGroundElevationInfo: m_elvInfo = variant.toInt(); break;
        case IndexGroundElevationInfoTransferred: m_isElvInfoTransferred = variant.toBool(); break;
        case IndexGroundElevationPlusInfo: break;
        case IndexCanLikelySkipNearGroundInterpolation: break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CAircraftSituation::comparePropertyByIndex(CPropertyIndexRef index,
                                                   const CAircraftSituation &compareValue) const
    {
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue);
        }
        if (ICoordinateGeodetic::canHandleIndex(index))
        {
            return ICoordinateGeodetic::comparePropertyByIndex(index, compareValue);
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPosition:
            return m_position.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPosition());
        case IndexAltitude:
            return this->getAltitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAltitude());
        case IndexVelocity:
            return m_velocity.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getVelocity());
        case IndexPBHInfo: // fall through
        case IndexPitch: return m_pitch.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPitch());
        case IndexBank: return m_bank.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getBank());
        case IndexCG: return m_cg.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCG());
        case IndexGroundSpeed:
            return m_groundSpeed.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundSpeed());
        case IndexGroundElevationPlane:
        case IndexGroundElevationPlusInfo:
        {
            const int c = m_groundElevationPlane.comparePropertyByIndex(index.copyFrontRemoved(),
                                                                        compareValue.getGroundElevationPlane());
            if (c != 0 || i == IndexGroundElevationPlane) { return c; }
            return Compare::compare(this->getGroundElevationInfo(), compareValue.getGroundElevationInfo());
        }
        case IndexCallsign:
            return m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
        case IndexIsOnGroundInfo:
            return m_onGroundInfo.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getOnGroundInfo());
        case IndexGroundElevationInfo:
        case IndexGroundElevationInfoString:
        {
            const int c = Compare::compare(this->getGroundElevationInfo(), compareValue.getGroundElevationInfo());
            if (c != 0) { return c; }
        }
            [[fallthrough]];
        case IndexGroundElevationInfoTransferred:
            return Compare::compare(m_isElvInfoTransferred, compareValue.m_isElvInfoTransferred);
        case IndexCanLikelySkipNearGroundInterpolation:
            return Compare::compare(this->canLikelySkipNearGroundInterpolation(),
                                    compareValue.canLikelySkipNearGroundInterpolation());
        default: break;
        }
        const QString assertMsg("No comparison for index " + index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(assertMsg));
        return 0;
    }

    bool CAircraftSituation::isNull() const { return this->isPositionNull(); }

    bool CAircraftSituation::isOtherElevationInfoBetter(CAircraftSituation::GndElevationInfo otherInfo,
                                                        bool transferred) const
    {
        if (otherInfo == NoElevationInfo || otherInfo == Test) { return false; }
        const int otherInfoInt = static_cast<int>(otherInfo);
        if (otherInfoInt > m_elvInfo) { return true; }
        if (otherInfoInt == m_elvInfo)
        {
            if (m_isElvInfoTransferred == transferred) { return false; } // not better (equal)
            return !transferred; // if not transferred it is better
        }
        return false;
    }

    bool CAircraftSituation::equalPbh(const CAircraftSituation &other) const
    {
        return this->getPitch() == other.getPitch() && this->getBank() == other.getBank() &&
               this->getHeading() == other.getHeading();
    }

    bool CAircraftSituation::equalPbhAndVector(const CAircraftSituation &other) const
    {
        return this->equalNormalVectorDouble(other.normalVectorDouble()) && this->equalPbh(other);
    }

    bool CAircraftSituation::equalPbhVectorAltitude(const CAircraftSituation &other) const
    {
        if (!this->equalPbhAndVector(other)) { return false; }
        const int c = this->getAltitude().compare(other.getAltitude());
        return c == 0;
    }

    bool CAircraftSituation::equalPbhVectorAltitudeElevation(const CAircraftSituation &other) const
    {
        if (!this->equalPbhVectorAltitude(other)) { return false; }
        const int c = this->getGroundElevation().compare(other.getGroundElevation());
        return c == 0;
    }

    void CAircraftSituation::setNull()
    {
        m_position.setNull();
        m_pressureAltitude.setNull();
        m_heading.setNull();
        m_pitch.setNull();
        m_bank.setNull();
        m_velocity = {};
        m_groundElevationPlane.setNull();
        m_groundSpeed.setNull();
        m_onGroundInfo = {};
        m_elvInfo = NoElevationInfo;
        m_isElvInfoTransferred = false;
        m_cg.setNull();
    }

    bool CAircraftSituation::isOnGroundFromParts() const
    {
        return this->isOnGround() && m_onGroundInfo.getGroundDetails() == COnGroundInfo::InFromParts;
    }

    bool CAircraftSituation::isOnGroundFromNetwork() const
    {
        return this->isOnGround() && m_onGroundInfo.getGroundDetails() == COnGroundInfo::InFromNetwork;
    }

    bool CAircraftSituation::isOnGroundInfoAvailable() const
    {
        if (this->hasInboundGroundDetails()) { return true; }
        return m_onGroundInfo.getOnGround() != COnGroundInfo::OnGroundSituationUnknown &&
               m_onGroundInfo.getGroundDetails() != COnGroundInfo::NotSetGroundDetails;
    }

    bool CAircraftSituation::shouldGuessOnGround() const { return !this->hasInboundGroundDetails(); }

    CLength CAircraftSituation::getGroundDistance(const CLength &centerOfGravity) const
    {
        if (centerOfGravity.isNull() || !this->hasGroundElevation()) { return CLength::null(); }
        const CAltitude groundPlusCG = this->getGroundElevation().withOffset(centerOfGravity);
        const CLength groundDistance = (this->getAltitude() - groundPlusCG);
        return groundDistance;
    }

    bool CAircraftSituation::hasGroundDetailsForGndInterpolation() const
    {
        return m_onGroundInfo.getGroundDetails() != COnGroundInfo::NotSetGroundDetails;
    }

    COnGroundInfo CAircraftSituation::getOnGroundInfo() const { return m_onGroundInfo; }

    void CAircraftSituation::setOnGroundDetails(COnGroundInfo::OnGroundDetails details)
    {
        m_onGroundInfo.setOnGroundDetails(details);
    }

    void CAircraftSituation::setOnGroundInfo(const aviation::COnGroundInfo &info) { m_onGroundInfo = info; }

    CAircraftSituation::GndElevationInfo CAircraftSituation::getGroundElevationInfo() const
    {
        if (!this->hasGroundElevation()) { return NoElevationInfo; }
        return static_cast<GndElevationInfo>(m_elvInfo);
    }

    QString CAircraftSituation::getGroundElevationInfoAsString() const
    {
        return m_isElvInfoTransferred ? u"tx: " % gndElevationInfoToString(this->getGroundElevationInfo()) :
                                        gndElevationInfoToString(this->getGroundElevationInfo());
    }

    QString CAircraftSituation::getGroundElevationAndInfo() const
    {
        static const QString n("null");
        if (m_groundElevationPlane.isNull()) { return n; }

        return m_groundElevationPlane.getAltitude().toQString(true) % u" [" % this->getGroundElevationInfoAsString() %
               u']';
    }

    bool CAircraftSituation::canTransferGroundElevation(const CAircraftSituation &transferToSituation,
                                                        const CLength &radius) const
    {
        if (!this->hasGroundElevation()) { return false; }

        // decide if transfer makes sense
        // always transfer from provider, but do not override provider
        if (transferToSituation.getGroundElevationInfo() == CAircraftSituation::FromProvider) { return false; }
        if (this->getGroundElevationInfo() != CAircraftSituation::FromProvider &&
            transferToSituation.getGroundElevationInfo() == CAircraftSituation::FromCache)
        {
            return false;
        }

        // distance
        const CLength distance = this->getGroundElevationPlane().calculateGreatCircleDistance(transferToSituation);
        const bool transferable = (distance <= radius);
        return transferable;
    }

    bool CAircraftSituation::transferGroundElevationFromMe(CAircraftSituation &transferToSituation,
                                                           const CLength &radius) const
    {
        return transferToSituation.transferGroundElevationToMe(*this, radius, true);
    }

    bool CAircraftSituation::transferGroundElevationToMe(const CAircraftSituation &fromSituation, const CLength &radius,
                                                         bool transferred)
    {
        if (!fromSituation.canTransferGroundElevation(*this, radius)) { return false; }
        return this->setGroundElevation(fromSituation.getGroundElevationPlane(), fromSituation.getGroundElevationInfo(),
                                        transferred);
    }

    bool CAircraftSituation::transferGroundElevationToMe(const CAircraftSituation &fromSituation, bool transferred)
    {
        return this->setGroundElevation(fromSituation.getGroundElevationPlane(), fromSituation.getGroundElevationInfo(),
                                        transferred);
    }

    bool CAircraftSituation::interpolateElevation(const CAircraftSituation &oldSituation,
                                                  const CAircraftSituation &newSituation)
    {
        const CElevationPlane ep = CAircraftSituation::interpolatedElevation(*this, oldSituation, newSituation);
        if (ep.isNull()) { return false; }
        this->setGroundElevation(ep, Interpolated);
        return true;
    }

    bool CAircraftSituation::hasGroundElevation() const { return !this->getGroundElevation().isNull(); }

    bool CAircraftSituation::hasInboundGroundDetails() const
    {
        return m_onGroundInfo.getGroundDetails() == COnGroundInfo::InFromParts ||
               m_onGroundInfo.getGroundDetails() == COnGroundInfo::InFromNetwork;
    }

    bool CAircraftSituation::setGroundElevation(const CAltitude &altitude, GndElevationInfo info, bool transferred)
    {
        bool set = false;
        if (altitude.isNull())
        {
            m_groundElevationPlane = CElevationPlane::null();
            m_isElvInfoTransferred = false;
            this->setGroundElevationInfo(NoElevationInfo);
        }
        else
        {
            m_groundElevationPlane = CElevationPlane(*this);
            m_groundElevationPlane.setSinglePointRadius();
            m_isElvInfoTransferred = transferred;
            m_groundElevationPlane.setGeodeticHeight(altitude.switchedUnit(this->getAltitudeUnit()));
            this->setGroundElevationInfo(info);
            set = true;
        }
        return set;
    }

    bool CAircraftSituation::setGroundElevation(const CElevationPlane &elevationPlane, GndElevationInfo info,
                                                bool transferred)
    {
        bool set = false;
        if (elevationPlane.isNull())
        {
            m_groundElevationPlane = CElevationPlane::null();
            m_isElvInfoTransferred = false;
            this->setGroundElevationInfo(NoElevationInfo);
        }
        else
        {
            m_groundElevationPlane = elevationPlane;
            m_groundElevationPlane.fixRadius();
            m_isElvInfoTransferred = transferred;
            this->setGroundElevationInfo(info);
            Q_ASSERT_X(!m_groundElevationPlane.getRadius().isNull(), Q_FUNC_INFO, "Null radius");
            m_groundElevationPlane.switchUnit(
                this->getAltitudeOrDefaultUnit()); // we use ft as internal unit, no "must" but simplification
            set = true;
        }
        return set;
    }

    bool CAircraftSituation::setGroundElevationChecked(const CElevationPlane &elevationPlane, GndElevationInfo info,
                                                       bool transferred)
    {
        if (elevationPlane.isNull()) { return false; }
        const CLength distance = this->calculateGreatCircleDistance(elevationPlane);
        if (distance > elevationPlane.getRadiusOrMinimumRadius()) { return false; }
        if (m_groundElevationPlane.isNull() || this->isOtherElevationInfoBetter(info, transferred))
        {
            // better values
            this->setGroundElevation(elevationPlane, info, transferred);
            m_groundElevationPlane.setRadiusOrMinimumRadius(distance);
            return true;
        }
        return false;
    }

    void CAircraftSituation::resetGroundElevation()
    {
        m_groundElevationPlane = CElevationPlane::null();
        this->setGroundElevationInfo(NoElevationInfo);
    }

    CLength CAircraftSituation::getHeightAboveGround() const
    {
        if (this->getAltitude().isNull()) { return CLength::null(); }
        if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
        {
            // we have a sure value explicitly set
            return this->getAltitude();
        }

        const CLength gh(this->getGroundElevation());
        if (gh.isNull()) { return CLength::null(); }

        // sanity checks
        if (std::isnan(gh.value()))
        {
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "nan ground");
            return CLength::null();
        }
        if (std::isnan(this->getAltitude().value()))
        {
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "nan altitude");
            return CLength::null();
        }

        const CLength ag = this->getAltitude() - gh;
        return ag;
    }

    void CAircraftSituation::setHeading(const CHeading &heading)
    {
        m_heading = heading.normalizedToPlusMinus180Degrees();
    }

    CLengthUnit CAircraftSituation::getAltitudeOrDefaultUnit() const
    {
        if (this->getAltitude().isNull()) { return CAltitude::defaultUnit(); }
        return m_position.geodeticHeight().getUnit();
    }

    CAltitude CAircraftSituation::getCorrectedAltitude(bool enableDragToGround,
                                                       CAircraftSituation::AltitudeCorrection *correction) const
    {
        return this->getCorrectedAltitude(m_cg, enableDragToGround, correction);
    }

    CAltitude CAircraftSituation::getCorrectedAltitude(const CLength &centerOfGravity, bool enableDragToGround,
                                                       AltitudeCorrection *correction) const
    {
        if (correction) { *correction = UnknownCorrection; }
        if (!this->hasGroundElevation())
        {
            if (correction) { *correction = NoElevation; }
            return this->getAltitude();
        }

        // above ground
        if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
        {
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Unsupported");
            if (correction) { *correction = AGL; }
            return this->getAltitude();
        }
        else
        {
            const CAltitude groundPlusCG =
                this->getGroundElevation().withOffset(centerOfGravity).switchedUnit(this->getAltitudeOrDefaultUnit());
            if (groundPlusCG.isNull())
            {
                if (correction) { *correction = NoElevation; }
                return this->getAltitude();
            }
            const CLength groundDistance = this->getAltitude() - groundPlusCG;
            const bool underflow = groundDistance.isNegativeWithEpsilonConsidered();
            if (underflow)
            {
                if (correction) { *correction = Underflow; }
                return groundPlusCG;
            }
            const bool nearGround = groundDistance.abs() < deltaNearGround();
            if (nearGround)
            {
                if (correction) { *correction = NoCorrection; }
                return groundPlusCG;
            }
            const bool forceDragToGround = (enableDragToGround && isOnGround()) &&
                                           (this->hasInboundGroundDetails() ||
                                            m_onGroundInfo.getGroundDetails() == COnGroundInfo::OnGroundByGuessing);
            if (forceDragToGround)
            {
                if (correction) { *correction = DraggedToGround; }
                return groundPlusCG;
            }

            if (correction) { *correction = NoCorrection; }
            return this->getAltitude();
        }
    }

    CAircraftSituation::AltitudeCorrection CAircraftSituation::correctAltitude(bool enableDragToGround)
    {
        return this->correctAltitude(m_cg, enableDragToGround);
    }

    CAircraftSituation::AltitudeCorrection CAircraftSituation::correctAltitude(const CLength &centerOfGravity,
                                                                               bool enableDragToGround)
    {
        CAircraftSituation::AltitudeCorrection altitudeCorrection = CAircraftSituation::UnknownCorrection;
        this->setAltitude(this->getCorrectedAltitude(centerOfGravity, enableDragToGround, &altitudeCorrection));
        this->setCG(centerOfGravity);
        return altitudeCorrection;
    }

    void CAircraftSituation::setAltitude(const CAltitude &altitude)
    {
        m_position.setGeodeticHeight(altitude.switchedUnit(CAltitude::defaultUnit()));
    }

    CAltitude CAircraftSituation::addAltitudeOffset(const CLength &offset)
    {
        if (offset.isNull()) { return this->getAltitude(); }
        const CAltitude alt = this->getAltitude().withOffset(offset);
        this->setAltitude(alt);
        return alt;
    }

    CAircraftSituation CAircraftSituation::withAltitudeOffset(const CLength &offset) const
    {
        if (offset.isNull()) { return *this; }
        CAircraftSituation copy(*this);
        copy.addAltitudeOffset(offset);
        return copy;
    }

    void CAircraftSituation::setPressureAltitude(const CAltitude &altitude)
    {
        Q_ASSERT(altitude.getAltitudeType() == CAltitude::PressureAltitude);
        m_pressureAltitude = altitude;
    }

    void CAircraftSituation::setPitch(const CAngle &pitch) { m_pitch = pitch.normalizedToPlusMinus180Degrees(); }

    void CAircraftSituation::setBank(const CAngle &bank) { m_bank = bank.normalizedToPlusMinus180Degrees(); }

    void CAircraftSituation::setZeroPBH()
    {
        static const CAngle za(0, CAngleUnit::deg());
        static const CHeading zh(za, CHeading::True);
        this->setPitch(za);
        this->setBank(za);
        this->setHeading(zh);
    }

    void CAircraftSituation::setZeroPBHandGs()
    {
        this->setZeroPBH();
        this->setGroundSpeed(CSpeed(0, CSpeedUnit::defaultUnit()));
    }

    QString CAircraftSituation::getPBHInfo() const
    {
        return QStringLiteral("P: %1 %2 B: %3 %4 H: %5 %6")
            .arg(this->getPitch().valueRoundedWithUnit(CAngleUnit::deg(), 1, true),
                 this->getPitch().valueRoundedWithUnit(CAngleUnit::rad(), 5, true),
                 this->getBank().valueRoundedWithUnit(CAngleUnit::deg(), 1, true),
                 this->getBank().valueRoundedWithUnit(CAngleUnit::rad(), 5, true),
                 this->getHeading().valueRoundedWithUnit(CAngleUnit::deg(), 1, true),
                 this->getHeading().valueRoundedWithUnit(CAngleUnit::rad(), 5, true));
    }

    bool CAircraftSituation::isMoving() const
    {
        const double gsKmh = this->getGroundSpeed().value(CSpeedUnit::km_h());
        return gsKmh >= 2.5;
    }

    bool CAircraftSituation::canLikelySkipNearGroundInterpolation() const
    {
        if (this->isNull()) { return true; }

        // those we can exclude, we are ON GROUND not guessed
        if (this->isOnGround() && this->hasInboundGroundDetails()) { return false; }

        // cases where we can skip
        // Concorde had a take-off speed of 220 knots (250 mph) and
        // landing speed was 187 mph
        if (this->getGroundSpeed().value(CSpeedUnit::kts()) > 225.0) { return true; }

        if (this->hasGroundElevation())
        {
            static const CLength threshold(400, CLengthUnit::m());
            const CLength aboveGround = this->getHeightAboveGround();
            if (!aboveGround.isNull() && aboveGround >= threshold) { return true; } // too high for ground
        }
        return false;
    }

    CLength CAircraftSituation::getDistancePerTime(std::chrono::milliseconds ms, const CLength &min) const
    {
        if (this->getGroundSpeed().isNull())
        {
            if (!min.isNull()) { return min; }
            return CLength(0, CLengthUnit::nullUnit());
        }
        const double seconds = ms.count() / 1000.0;
        const double gsMeterSecond = this->getGroundSpeed().value(CSpeedUnit::m_s());
        const CLength d(seconds * gsMeterSecond, CLengthUnit::m());
        if (!min.isNull() && d < min) { return min; }
        return d;
    }

    CLength CAircraftSituation::getDistancePerTime250ms(const CLength &min) const
    {
        using namespace std::chrono_literals;
        return this->getDistancePerTime(250ms, min);
    }

    void CAircraftSituation::setCallsign(const CCallsign &callsign)
    {
        m_correspondingCallsign = callsign;
        m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
    }

    void CAircraftSituation::setCG(const CLength &cg) { m_cg = cg.switchedUnit(this->getAltitudeOrDefaultUnit()); }

    bool CAircraftSituation::adjustGroundFlag(const CAircraftParts &parts, bool alwaysSetDetails,
                                              double timeDeviationFactor, qint64 *differenceMs)
    {
        Q_ASSERT_X(timeDeviationFactor >= 0 && timeDeviationFactor <= 1.0, Q_FUNC_INFO, "Expect 0..1");
        static const qint64 Max = std::numeric_limits<qint64>::max();
        if (differenceMs) { *differenceMs = Max; }

        if (m_onGroundInfo.getGroundDetails() == COnGroundInfo::InFromNetwork) { return false; }
        if (alwaysSetDetails) { m_onGroundInfo.setOnGroundDetails(COnGroundInfo::InFromParts); }
        const qint64 d = this->getAdjustedTimeDifferenceMs(parts.getAdjustedMSecsSinceEpoch());
        const bool adjust = (d >= 0) || qAbs(d) < (timeDeviationFactor *
                                                   parts.getTimeOffsetMs()); // future or past within deviation range
        if (!adjust) { return false; }

        if (differenceMs) { *differenceMs = d; }
        m_onGroundInfo = COnGroundInfo(parts.isOnGround() ? COnGroundInfo::OnGround : COnGroundInfo::NotOnGround,
                                       COnGroundInfo::InFromParts);
        return true;
    }

    bool CAircraftSituation::adjustGroundFlag(const CAircraftPartsList &partsList, bool alwaysSetDetails,
                                              double timeDeviationFactor, qint64 *differenceMs)
    {
        Q_ASSERT_X(timeDeviationFactor >= 0 && timeDeviationFactor <= 1.0, Q_FUNC_INFO, "Expect 0..1");
        static const qint64 Max = std::numeric_limits<qint64>::max();
        if (differenceMs) { *differenceMs = Max; }

        if (m_onGroundInfo.getGroundDetails() == COnGroundInfo::InFromNetwork) { return false; }
        if (alwaysSetDetails) { m_onGroundInfo.setOnGroundDetails(COnGroundInfo::InFromParts); }
        if (partsList.isEmpty()) { return false; }

        CAircraftParts bestParts;
        bool adjust = false;
        qint64 bestDistance = Max;
        for (const CAircraftParts &parts : partsList)
        {
            const qint64 d = this->getAdjustedTimeDifferenceMs(parts.getAdjustedMSecsSinceEpoch());
            const qint64 posD = qAbs(d);
            const bool candidate =
                (d >= 0) ||
                posD < (timeDeviationFactor * parts.getTimeOffsetMs()); // future or past within deviation range
            if (!candidate || bestDistance <= posD) { continue; }
            bestDistance = posD;
            if (differenceMs) { *differenceMs = d; }
            adjust = true;
            bestParts = parts;
            if (bestDistance == 0) { break; }
        }
        if (!adjust) { return false; }

        const COnGroundInfo::IsOnGround og =
            bestParts.isOnGround() ? COnGroundInfo::OnGround : COnGroundInfo::NotOnGround;
        m_onGroundInfo = COnGroundInfo(og, COnGroundInfo::InFromParts);
        return true;
    }
} // namespace swift::misc::aviation
