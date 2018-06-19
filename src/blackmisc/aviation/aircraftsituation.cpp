/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackConfig;

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
                   QStringLiteral(" | ") % m_position.toQString(i18n) %
                   QStringLiteral(" | alt: ") % this->getAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 1) %
                   QStringLiteral(" ") % this->getCorrectedAltitude().valueRoundedWithUnit(CLengthUnit::ft(), 1) %
                   QStringLiteral("[cor] | og: ") % this->getOnGroundInfo() %
                   (m_onGroundGuessingDetails.isEmpty() ? QStringLiteral("") : QStringLiteral(" ") % m_onGroundGuessingDetails) %
                   QStringLiteral(" | cg: ") %
                   (m_cg.isNull() ? QStringLiteral("null") : m_cg.valueRoundedWithUnit(CLengthUnit::m(), 1) % QStringLiteral(" ") % m_cg.valueRoundedWithUnit(CLengthUnit::ft(), 1)) %
                   QStringLiteral(" | factor [0..1]: ") % QString::number(m_onGroundFactor, 'f', 2) %
                   QStringLiteral(" | skip ng: ") % boolToYesNo(this->canLikelySkipNearGroundInterpolation()) %
                   QStringLiteral(" | bank: ") % m_bank.toQString(i18n) %
                   QStringLiteral(" | pitch: ") % m_pitch.toQString(i18n) %
                   QStringLiteral(" | heading: ") % m_heading.toQString(i18n) %
                   QStringLiteral(" | gs: ") % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::kts(), 1, true) %
                   QStringLiteral(" ") % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::m_s(), 1, true) %
                   QStringLiteral(" | elevation [") % this->getGroundElevationInfoAsString() % QStringLiteral("]: ") % (m_groundElevationPlane.toQString(i18n));
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
            static const QString elv("elevation");
            static const QString elvCg("elevation/CG");
            static const QString interpolation("interpolation");
            static const QString guess("guessing");
            static const QString unknown("unknown");
            static const QString outOwnAircraft("own aircraft");
            static const QString inNetwork("from network");
            static const QString inFromParts("from parts");
            static const QString inNoGndInfo("no gnd.info");

            switch (reliability)
            {
            case CAircraftSituation::OnGroundByElevation: return elv;
            case CAircraftSituation::OnGroundByElevationAndCG: return elvCg;
            case CAircraftSituation::OnGroundByGuessing: return guess;
            case CAircraftSituation::OnGroundByInterpolation: return interpolation;
            case CAircraftSituation::OutOnGroundOwnAircraft: return outOwnAircraft;
            case CAircraftSituation::InFromNetwork: return inNetwork;
            case CAircraftSituation::InFromParts: return inFromParts;
            case CAircraftSituation::InNoGroundInfo: return inNoGndInfo;
            case CAircraftSituation::NotSetGroundDetails:
            default: return unknown;
            }
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

        const QString &CAircraftSituation::gndElevationInfoToString(GndElevationInfo details)
        {
            static const QString noDetails("no details");
            static const QString unknown("unknown");
            static const QString transferred("transferred");
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
            case TransferredElevation: return transferred;
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

        bool CAircraftSituation::presetGroundElevation(CAircraftSituation &situationToPreset, const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, const CAircraftSituationChange &change)
        {
            // IMPORTANT: we do not know what the situation will be interpolated to, so we cannot transfer
            situationToPreset.resetGroundElevation();
            do
            {
                if (oldSituation.equalNormalVectorDouble(newSituation))
                {
                    if (oldSituation.hasGroundElevation())
                    {
                        // same positions, we can use existing elevation
                        // means we were not moving between old an new
                        situationToPreset.setGroundElevation(oldSituation.getGroundElevationPlane(), CAircraftSituation::TransferredElevation);
                        break;
                    }
                }

                const CLength distance = newSituation.calculateGreatCircleDistance(oldSituation);
                if (distance < newSituation.getDistancePerTime250ms())
                {
                    if (oldSituation.hasGroundElevation())
                    {
                        // almost same positions, we can use existing elevation
                        situationToPreset.setGroundElevation(oldSituation.getGroundElevationPlane(), CAircraftSituation::TransferredElevation);
                        break;
                    }
                }

                if (change.hasElevationDevWithinAllowedRange())
                {
                    // not much change in known elevations
                    const CAltitudePair elvDevMean = change.getElevationStdDevAndMean();
                    situationToPreset.setGroundElevation(elvDevMean.second, CAircraftSituation::SituationChange);
                    break;
                }

                const CElevationPlane epInterpolated = CAircraftSituation::interpolatedElevation(CAircraftSituation::null(), oldSituation, newSituation, distance);
                if (!epInterpolated.isNull())
                {
                    situationToPreset.setGroundElevation(epInterpolated, CAircraftSituation::Interpolated);
                    break;
                }
            }
            while (false);
            return situationToPreset.hasGroundElevation();
        }

        CElevationPlane CAircraftSituation::interpolatedElevation(const CAircraftSituation &situation, const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, const CLength &distance)
        {
            if (oldSituation.isNull() || newSituation.isNull()) { return CAircraftSituation::null(); }
            if (oldSituation.equalNormalVectorDouble(newSituation)) { return newSituation.getGroundElevationPlane(); }

            const double newElvFt = newSituation.getGroundElevation().value(CLengthUnit::ft());
            const double oldElvFt = oldSituation.getGroundElevation().value(CLengthUnit::ft());
            const double deltaElvFt = newElvFt - oldElvFt;
            if (deltaElvFt > 25) { return CElevationPlane::null(); }

            if (!situation.isNull())
            {
                const double distanceSituationNewM = situation.calculateGreatCircleDistance(newSituation).value(CLengthUnit::m());
                if (distanceSituationNewM < 5.0) { return newSituation.getGroundElevationPlane(); }

                const double distanceOldNewM = (distance.isNull() ? oldSituation.calculateGreatCircleDistance(newSituation) : distance).value(CLengthUnit::m());
                if (distanceOldNewM < 5.0) { return oldSituation.getGroundElevationPlane(); }

                const double distRatio = distanceSituationNewM / distanceOldNewM;

                // very close to the situations we return tehir elevation
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

        bool CAircraftSituation::extrapolateElevation(CAircraftSituation &newSituation, const CAircraftSituation &oldSituation, const CAircraftSituation &olderSituation, const CAircraftSituationChange &oldChange)
        {
            if (newSituation.hasGroundElevation()) { return false; }
            if (oldSituation.transferGroundElevation(newSituation)) { return true; }
            if (oldSituation.isNull() || olderSituation.isNull()) { return false; }

            if (oldChange.isNull()) { return false; }
            if (oldChange.isConstOnGround() && oldChange.hasAltitudeDevWithinAllowedRange() && oldChange.hasElevationDevWithinAllowedRange())
            {
                // we have almost const altitudes and elevations
                const double deltaAltFt = qAbs(newSituation.getAltitude().value(CLengthUnit::ft()) - olderSituation.getAltitude().value(CLengthUnit::ft()));
                if (deltaAltFt <= CAircraftSituationChange::allowedAltitudeDeviation().value(CLengthUnit::ft()))
                {
                    // the ccurrent alt is also not much different
                    newSituation.setGroundElevation(oldSituation.getGroundElevation(), Extrapolated);
                    return true;
                }
            }

            return false;
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
            case IndexCG: return m_cg.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundSpeed: return m_groundSpeed.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundElevationPlane: return m_groundElevationPlane.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign: return m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexIsOnGround: return CVariant::fromValue(m_onGround);
            case IndexIsOnGroundString: return CVariant::fromValue(this->onGroundAsString());
            case IndexOnGroundReliability: return CVariant::fromValue(m_onGroundDetails);
            case IndexOnGroundReliabilityString: return CVariant::fromValue(this->getOnDetailsAsString());
            case IndexGroundElevationInfo: return CVariant::fromValue(this->getGroundElevationInfo());
            case IndexGroundElevationInfoString: return CVariant::fromValue(this->getGroundElevationInfoAsString());
            case IndexGroundElevationPlusInfo: return CVariant::fromValue(this->getGroundElevationAndInfo());
            case IndexCanLikelySkipNearGroundInterpolation: return CVariant::fromValue(this->canLikelySkipNearGroundInterpolation());
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
            case IndexCG: m_cg.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexGroundSpeed: m_groundSpeed.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexGroundElevationPlane: m_groundElevationPlane.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCallsign: m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexIsOnGround: m_onGround = variant.toInt(); break;
            case IndexOnGroundReliability: m_onGroundDetails = variant.toInt(); break;
            case IndexGroundElevationInfo: m_elvInfo = variant.toInt(); break;
            case IndexGroundElevationPlusInfo: break;
            case IndexCanLikelySkipNearGroundInterpolation: break;
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
            case IndexCG: return m_cg.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCG());
            case IndexGroundSpeed: return m_groundSpeed.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundSpeed());
            case IndexGroundElevationPlane:
            case IndexGroundElevationPlusInfo:
                {
                    const int c = m_groundElevationPlane.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundElevationPlane());
                    if (c != 0 || i == IndexGroundElevationPlane) { return c; }
                    return Compare::compare(this->getGroundElevationInfo(), compareValue.getGroundElevationInfo());
                }
            case IndexCallsign: return m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexIsOnGround:
            case IndexIsOnGroundString:
                return Compare::compare(m_onGround, compareValue.m_onGround);
            case IndexOnGroundReliability:
            case IndexOnGroundReliabilityString:
                return Compare::compare(m_onGroundDetails, compareValue.m_onGroundDetails);
            case IndexGroundElevationInfo:
            case IndexGroundElevationInfoString:
                return Compare::compare(this->getGroundElevationInfo(), compareValue.getGroundElevationInfo());
            case IndexCanLikelySkipNearGroundInterpolation: return Compare::compare(this->canLikelySkipNearGroundInterpolation(), compareValue.canLikelySkipNearGroundInterpolation());
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

        bool CAircraftSituation::isBetterInfo(CAircraftSituation::GndElevationInfo info) const
        {
            if (info == NoElevationInfo || info == Test) { return false; }
            const int i = static_cast<int>(info);
            return i > m_onGroundDetails;
        }

        bool CAircraftSituation::equalPbh(const CAircraftSituation &other) const
        {
            return this->getPitch() == other.getPitch() && this->getBank() == other.getBank() && this->getHeading() == other.getHeading();
        }

        bool CAircraftSituation::equalPbhAndVector(const CAircraftSituation &other) const
        {
            return this->equalNormalVectorDouble(other.normalVectorDouble()) && this->equalPbh(other);
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
            m_onGroundDetails = CAircraftSituation::NotSetGroundDetails;
            m_elvInfo = NoElevationInfo;
        }

        bool CAircraftSituation::isOnGroundFromParts() const
        {
            return this->isOnGround() && this->getOnGroundDetails() == InFromParts;
        }

        bool CAircraftSituation::isOnGroundFromNetwork() const
        {
            return this->isOnGround() && this->getOnGroundDetails() == InFromNetwork;
        }

        const QString &CAircraftSituation::onGroundAsString() const
        {
            return CAircraftSituation::isOnGroundToString(this->getOnGround());
        }

        bool CAircraftSituation::isOnGroundInfoAvailable() const
        {
            if (this->hasInboundGroundDetails()) { return true; }
            return this->getOnGround() != CAircraftSituation::OnGroundSituationUnknown &&
                   this->getOnGroundDetails() != CAircraftSituation::NotSetGroundDetails;
        }

        bool CAircraftSituation::setOnGround(bool onGround)
        {
            return this->setOnGround(onGround ? OnGround : NotOnGround);
        }

        bool CAircraftSituation::setOnGround(CAircraftSituation::IsOnGround onGround)
        {
            if (this->getOnGround() == onGround) { return false; }
            const int og = static_cast<int>(onGround);
            m_onGround = og;
            m_onGroundFactor = (onGround == OnGround) ?  1.0 : 0.0;
            return true;
        }

        bool CAircraftSituation::setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundDetails details)
        {
            const bool set = this->setOnGround(onGround);
            this->setOnGroundDetails(details);
            if (details != OnGroundByGuessing)
            {
                m_onGroundGuessingDetails.clear();
            }

            return set;
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

        bool CAircraftSituation::shouldGuessOnGround() const
        {
            return !this->hasInboundGroundDetails();
        }

        bool CAircraftSituation::guessOnGround(const CAircraftSituationChange &change, const CAircraftModel &model)
        {
            if (!this->shouldGuessOnGround()) { return false; }

            // for debugging purposed
            QString *details = CBuildConfig::isLocalDeveloperDebugBuild() ? &m_onGroundGuessingDetails : nullptr;

            // Non VTOL aircraft have to move to be not on ground
            const bool vtol = model.isVtol();
            if (!vtol)
            {
                if (this->getGroundSpeed().isNegativeWithEpsilonConsidered())
                {
                    this->setOnGround(OnGround, CAircraftSituation::OnGroundByGuessing);
                    if (details) { *details = QStringLiteral("No VTOL, push back"); }
                    return true;
                }

                if (!this->isMoving())
                {
                    this->setOnGround(OnGround, CAircraftSituation::OnGroundByGuessing);
                    if (details) { *details = QStringLiteral("No VTOL, not moving => on ground"); }
                    return true;
                }
            }

            // not on ground is default
            this->setOnGround(CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByGuessing);

            CLength cg = m_cg.isNull() ? model.getCG() : m_cg;
            CSpeed guessedRotateSpeed = CSpeed::null();
            CSpeed sureRotateSpeed = CSpeed(130, CSpeedUnit::kts());
            model.getAircraftIcaoCode().guessModelParameters(cg, guessedRotateSpeed);
            if (!guessedRotateSpeed.isNull())
            {
                // does the value make any sense?
                const bool validGuessedSpeed = (guessedRotateSpeed.value(CSpeedUnit::km_h()) > 5.0);
                BLACK_VERIFY_X(validGuessedSpeed, Q_FUNC_INFO, "Wrong guessed value for lift off");
                if (!validGuessedSpeed) { guessedRotateSpeed = CSpeed(80, CSpeedUnit::kts()); } // fix
                sureRotateSpeed = guessedRotateSpeed * 1.25;
            }

            // "extreme" values for which we are surely not on ground
            if (qAbs(this->getPitch().value(CAngleUnit::deg())) > 20)  { if (details) { *details = QStringLiteral("max.pitch"); }; return true; } // some tail wheel aircraft already have 11° pitch on ground
            if (qAbs(this->getBank().value(CAngleUnit::deg()))  > 10)  { if (details) { *details = QStringLiteral("max.bank"); }; return true; }
            if (this->getGroundSpeed() > sureRotateSpeed) { if (details) { *details = QStringLiteral("gs. > vr ") % sureRotateSpeed.valueRoundedWithUnit(1); }; return true; }

            // use the most accurate or reliable guesses here first
            // ------------------------------------------------------
            // by elevation
            // we can detect "on ground" (underflow, near ground), but not "not on ground" because of overflow

            // we can detect on ground for underflow, but not for overflow (so we can not rely on NotOnGround)
            IsOnGround og = this->isOnGroundByElevation(cg);
            if (og == OnGround)
            {
                if (details) { *details = QStringLiteral("elevation on ground"); }
                this->setOnGround(og, CAircraftSituation::OnGroundByGuessing);
                return true;
            }

            if (!change.isNull())
            {
                if (!vtol && change.wasConstOnGround())
                {
                    if (change.isRotatingUp())
                    {
                        // not OG
                        if (details) { *details = QStringLiteral("rotating up detected"); }
                        return true;
                    }

                    // here we stick to ground until we detect rotate up
                    this->setOnGround(CAircraftSituation::OnGround, CAircraftSituation::OnGroundByGuessing);
                    if (details) { *details = QStringLiteral("waiting for rotating up"); }
                    return true;
                }

                if (change.isConstAscending())
                {
                    // not OG
                    if (details) { *details = QStringLiteral("const ascending"); }
                    return true;
                }
            }

            // on VTOL we stop here
            if (vtol)
            {
                // no idea
                this->setOnGround(OnGroundSituationUnknown, NotSetGroundDetails);
                return false;
            }

            // guessed speed null -> vtol
            if (!guessedRotateSpeed.isNull())
            {
                // does the value make any sense?
                if (this->getGroundSpeed() < guessedRotateSpeed)
                {
                    this->setOnGround(OnGround, CAircraftSituation::OnGroundByGuessing);
                    if (details) { *details = QStringLiteral("Guessing, max.guessed gs.") + guessedRotateSpeed.valueRoundedWithUnit(CSpeedUnit::kts(), 1); };
                    return true;
                }
            }

            // not sure, but this is a guess
            if (details) { *details = QStringLiteral("Fall through"); }
            return true;
        }

        CLength CAircraftSituation::getGroundDistance(const CLength &centerOfGravity) const
        {
            if (centerOfGravity.isNull() || !this->hasGroundElevation()) { return CLength::null(); }
            const CAltitude groundPlusCG = this->getGroundElevation().withOffset(centerOfGravity);
            const CLength groundDistance = (this->getAltitude() - groundPlusCG);
            return groundDistance;
        }

        bool CAircraftSituation::hasGroundDetailsForGndInterpolation() const
        {
            return this->getOnGroundDetails() != CAircraftSituation::NotSetGroundDetails;
        }

        const QString &CAircraftSituation::getOnDetailsAsString() const
        {
            return CAircraftSituation::onGroundDetailsToString(this->getOnGroundDetails());
        }

        bool CAircraftSituation::setOnGroundDetails(CAircraftSituation::OnGroundDetails details)
        {
            if (details != OnGroundByGuessing)
            {
                m_onGroundGuessingDetails.clear();
            }
            if (this->getOnGroundDetails() == details) { return false; }
            m_onGroundDetails = static_cast<int>(details);
            return true;
        }

        bool CAircraftSituation::setOnGroundFromGroundFactorFromInterpolation(double threshold)
        {
            this->setOnGroundDetails(OnGroundByInterpolation);
            if (this->getOnGroundFactor() < 0.0)
            {
                this->setOnGround(NotSetGroundDetails);
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

        CAircraftSituation::GndElevationInfo CAircraftSituation::getGroundElevationInfo() const
        {
            if (!this->hasGroundElevation()) { return NoElevationInfo; }
            return static_cast<GndElevationInfo>(m_elvInfo);
        }

        QString CAircraftSituation::getGroundElevationAndInfo() const
        {
            static const QString n("null");
            if (m_groundElevationPlane.isNull()) { return n; };

            return m_groundElevationPlane.getAltitude().toQString(true) %
                   QStringLiteral(" [") % this->getGroundElevationInfoAsString() % QStringLiteral("]");
        }

        bool CAircraftSituation::canTransferGroundElevation(const CAircraftSituation &otherSituation, const CLength &radius) const
        {
            if (!this->hasGroundElevation()) { return false; }
            const CLength distance = this->getGroundElevationPlane().calculateGreatCircleDistance(otherSituation);
            const bool transferable = (distance <= radius);
            return transferable;
        }

        bool CAircraftSituation::transferGroundElevation(CAircraftSituation &transferToSituation, const CLength &radius) const
        {
            if (!this->canTransferGroundElevation(transferToSituation, radius)) { return false; }
            transferToSituation.setGroundElevation(this->getGroundElevationPlane(), TransferredElevation);
            Q_ASSERT_X(!transferToSituation.getGroundElevationRadius().isNull(), Q_FUNC_INFO, "null radius");
            return true;
        }

        bool CAircraftSituation::presetGroundElevation(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, const CAircraftSituationChange &change)
        {
            return CAircraftSituation::presetGroundElevation(*this, oldSituation, newSituation, change);
        }

        bool CAircraftSituation::extrapolateElevation(const CAircraftSituation &oldSituation, const CAircraftSituation &olderSituation, const CAircraftSituationChange &change)
        {
            return CAircraftSituation::extrapolateElevation(*this, oldSituation, olderSituation, change);
        }

        bool CAircraftSituation::interpolateElevation(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation)
        {
            const CElevationPlane ep = CAircraftSituation::interpolatedElevation(*this, oldSituation, newSituation);
            if (ep.isNull()) { return false; }
            this->setGroundElevation(ep, Interpolated);
            return true;
        }

        CAircraftSituation::IsOnGround CAircraftSituation::isOnGroundByElevation() const
        {
            return this->isOnGroundByElevation(m_cg);
        }

        CAircraftSituation::IsOnGround CAircraftSituation::isOnGroundByElevation(const CLength &cg) const
        {
            Q_ASSERT_X(!cg.isNegativeWithEpsilonConsidered(), Q_FUNC_INFO, "CG must not be negative");
            const CLength groundDistance = this->getGroundDistance(cg);
            if (groundDistance.isNull()) { return OnGroundSituationUnknown; }
            if (groundDistance.isNegativeWithEpsilonConsidered()) { return OnGround; }
            if (groundDistance.abs() < deltaNearGround()) { return OnGround; }
            if (!cg.isNull())
            {
                // smaller than percentage from CG
                const CLength cgFactor(cg * 0.1);
                if (groundDistance.abs() < cgFactor) { return OnGround; }
            }
            return NotOnGround;
        }

        bool CAircraftSituation::hasGroundElevation() const
        {
            return !this->getGroundElevation().isNull();
        }

        bool CAircraftSituation::hasInboundGroundDetails() const
        {
            return this->getOnGroundDetails() == CAircraftSituation::InFromParts || this->getOnGroundDetails() == CAircraftSituation::InFromNetwork;
        }

        void CAircraftSituation::setGroundElevation(const CAltitude &altitude, GndElevationInfo info)
        {
            if (altitude.isNull())
            {
                m_groundElevationPlane = CElevationPlane::null();
                this->setGroundElevationInfo(NoElevationInfo);
            }
            else
            {
                m_groundElevationPlane = CElevationPlane(*this);
                m_groundElevationPlane.setSinglePointRadius();
                m_groundElevationPlane.setGeodeticHeight(altitude.switchedUnit(this->getAltitudeUnit()));
                this->setGroundElevationInfo(info);
            }
        }

        void CAircraftSituation::setGroundElevation(const CElevationPlane &elevationPlane, GndElevationInfo info)
        {
            m_groundElevationPlane = elevationPlane;
            if (elevationPlane.isNull())
            {
                this->setGroundElevationInfo(NoElevationInfo);
            }
            else
            {
                this->setGroundElevationInfo(info);
                Q_ASSERT_X(!m_groundElevationPlane.getRadius().isNull(), Q_FUNC_INFO, "Null radius");
                m_groundElevationPlane.switchUnit(this->getAltitudeOrDefaultUnit()); // we use ft as internal unit, no "must" but simplification
            }
        }

        bool CAircraftSituation::setGroundElevationChecked(const CElevationPlane &elevationPlane, GndElevationInfo info)
        {
            if (elevationPlane.isNull()) { return false; }
            const CLength distance =  this->calculateGreatCircleDistance(elevationPlane);
            if (distance > elevationPlane.getRadius()) { return false; }
            if (m_groundElevationPlane.isNull() || this->isBetterInfo(info))
            {
                // better values
                this->setGroundElevation(elevationPlane, info);
                m_groundElevationPlane.setRadiusOrMinimum(distance);
                return true;
            }
            return false;
        }

        void CAircraftSituation::resetGroundElevation()
        {
            m_groundElevationPlane = CElevationPlane::null();
            this->setGroundElevationInfo(NoElevationInfo);
        }

        const CLength &CAircraftSituation::getGroundElevationRadius() const
        {
            if (!this->hasGroundElevation()) { return CLength::null(); }
            return m_groundElevationPlane.getRadius();
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
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "nan ground");
                return CLength::null();
            }
            if (std::isnan(this->getAltitude().value()))
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "nan altitude");
                return CLength::null();
            }

            const CLength ag = this->getAltitude() - gh;
            return ag;
        }

        const CLengthUnit &CAircraftSituation::getAltitudeOrDefaultUnit() const
        {
            if (this->getAltitude().isNull()) { return CAltitude::defaultUnit(); }
            return m_position.geodeticHeight().getUnit();
        }

        CAltitude CAircraftSituation::getCorrectedAltitude(bool enableDragToGround, CAircraftSituation::AltitudeCorrection *correction) const
        {
            return this->getCorrectedAltitude(m_cg, enableDragToGround, correction);
        }

        CAltitude CAircraftSituation::getCorrectedAltitude(const CLength &centerOfGravity, bool enableDragToGround, AltitudeCorrection *correction) const
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
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Unsupported");
                if (correction) { *correction = AGL; }
                return this->getAltitude();
            }
            else
            {
                const CAltitude groundPlusCG = this->getGroundElevation().withOffset(centerOfGravity).switchedUnit(this->getAltitudeOrDefaultUnit());
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
                const bool nearGround =  groundDistance.abs() < deltaNearGround();
                if (nearGround)
                {
                    if (correction) { *correction = NoCorrection; }
                    return groundPlusCG;
                }
                const bool forceDragToGround = (enableDragToGround && this->getOnGround() == OnGround) && (this->hasInboundGroundDetails() || this->getOnGroundDetails() == OnGroundByGuessing);
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

        CAircraftSituation::AltitudeCorrection CAircraftSituation::correctAltitude(const CLength &centerOfGravity, bool enableDragToGround)
        {
            CAircraftSituation::AltitudeCorrection altCor = CAircraftSituation::UnknownCorrection;
            this->setAltitude(this->getCorrectedAltitude(centerOfGravity, enableDragToGround, &altCor));
            this->setCG(centerOfGravity);
            return altCor;
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

        void CAircraftSituation::setPressureAltitude(const CAltitude &altitude)
        {
            Q_ASSERT(altitude.getAltitudeType() == CAltitude::PressureAltitude);
            m_pressureAltitude = altitude;
        }

        bool CAircraftSituation::isMoving() const
        {
            const double gsKmh = this->getGroundSpeed().value(CSpeedUnit::km_h());
            return gsKmh >= 2.5;
        }

        bool CAircraftSituation::canLikelySkipNearGroundInterpolation() const
        {
            // those we can exclude
            if (this->isOnGround() && this->hasInboundGroundDetails()) { return false; }

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

        CLength CAircraftSituation::getDistancePerTime250ms() const
        {
            return this->getDistancePerTime(250);
        }

        void CAircraftSituation::setCallsign(const CCallsign &callsign)
        {
            m_correspondingCallsign = callsign;
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }

        void CAircraftSituation::setCG(const CLength &cg)
        {
            m_cg = cg.switchedUnit(this->getAltitudeOrDefaultUnit());
        }

        bool CAircraftSituation::adjustGroundFlag(const CAircraftParts &parts, bool alwaysSetDetails, double timeDeviationFactor, qint64 *differenceMs)
        {
            Q_ASSERT_X(timeDeviationFactor >= 0 && timeDeviationFactor <= 1.0, Q_FUNC_INFO, "Expect 0..1");
            static const qint64 Max = std::numeric_limits<qint64>::max();
            if (differenceMs) { *differenceMs = Max; }

            if (this->getOnGroundDetails() == CAircraftSituation::InFromNetwork) { return false; }
            if (alwaysSetDetails) { this->setOnGroundDetails(InFromParts); }
            const qint64 d = this->getAdjustedTimeDifferenceMs(parts.getAdjustedMSecsSinceEpoch());
            const bool adjust = (d >= 0) || qAbs(d) < (timeDeviationFactor * parts.getTimeOffsetMs()); // future or past within deviation range
            if (!adjust) { return false; }

            if (differenceMs) { *differenceMs = d; }
            this->setOnGround(parts.isOnGround() ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround, CAircraftSituation::InFromParts);
            return true;
        }

        bool CAircraftSituation::adjustGroundFlag(const CAircraftPartsList &partsList, bool alwaysSetDetails, double timeDeviationFactor, qint64 *differenceMs)
        {
            Q_ASSERT_X(timeDeviationFactor >= 0 && timeDeviationFactor <= 1.0, Q_FUNC_INFO, "Expect 0..1");
            static const qint64 Max = std::numeric_limits<qint64>::max();
            if (differenceMs) { *differenceMs = Max; }

            if (this->getOnGroundDetails() == CAircraftSituation::InFromNetwork) { return false; }
            if (alwaysSetDetails) { this->setOnGroundDetails(InFromParts); }
            if (partsList.isEmpty()) { return false; }

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
