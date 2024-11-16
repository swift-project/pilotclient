// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftsituationchange.h"

#include <QPair>
#include <QtGlobal>

#include "QStringBuilder"

#include "config/buildconfig.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/comparefunctions.h"
#include "misc/math/mathutils.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/stringutils.h"
#include "misc/verify.h"

using namespace swift::misc::physical_quantities;
using namespace swift::misc::math;
using namespace swift::config;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftSituationChange)

namespace swift::misc::aviation
{
    void CAircraftSituationChange::registerMetadata()
    {
        CValueObject<CAircraftSituationChange>::registerMetadata();
        qRegisterMetaType<CAircraftSituationChange::GuessedSceneryDeviation>();
    }

    CAircraftSituationChange::CAircraftSituationChange(const CAircraftSituationList &situations,
                                                       const physical_quantities::CLength &cg, bool isVtol,
                                                       bool alreadySortedLatestFirst, bool calcStdDeviations)
    {
        if (situations.size() < 2) { return; }
        const CAircraftSituationList sorted(alreadySortedLatestFirst ? situations :
                                                                       situations.getSortedAdjustedLatestFirst());

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            Q_ASSERT_X(sorted.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO,
                       "Wrong sort order or NULL position");
        }

        const CAircraftSituation latest(sorted.front());
        const CAircraftSituation oldest(sorted.back());
        const CCallsign cs(latest.getCallsign());

        m_situationsCount = situations.size();
        m_correspondingCallsign = cs;
        m_timestampMSecsSinceEpoch = latest.getMSecsSinceEpoch();
        m_timeOffsetMs = latest.getTimeOffsetMs();
        m_oldestTimestampMSecsSinceEpoch = oldest.getMSecsSinceEpoch();
        m_latestAdjustedTimestampMSecsSinceEpoch = latest.getAdjustedMSecsSinceEpoch();
        m_oldestAdjustedTimestampMSecsSinceEpoch = oldest.getAdjustedMSecsSinceEpoch();
        m_constAscending = sorted.isConstAscending(true);
        m_constDescending = sorted.isConstDescending(true);
        m_constOnGround = sorted.isConstOnGround();
        m_constNotOnGround = sorted.isConstNotOnGround();
        m_justTakeoff = sorted.isJustTakingOff(true);
        m_justTouchdown = sorted.isJustTouchingDown(true);
        m_constAccelerating = sorted.isConstAccelerating(true);
        m_constDecelerating = sorted.isConstDecelarating(true);
        m_containsPushBack = !isVtol && sorted.containsPushBack();

        if (sorted.size() >= 3)
        {
            const CAircraftSituationList sortedWithout = sorted.withoutFrontSituation();
            m_wasNotOnGround = sortedWithout.isConstNotOnGround();
            m_wasOnGround = sortedWithout.isConstOnGround();
        }

        if (calcStdDeviations)
        {
            this->calculateStdDeviations(situations, cg);
            m_rotateUp = sorted.front().getPitch() > (m_pitchMean + m_pitchStdDev);
        }
        else { m_rotateUp = sorted.isRotatingUp(true); }
    }

    bool CAircraftSituationChange::guessOnGround(CAircraftSituation &situation,
                                                 const simulation::CAircraftModel &model) const
    {
        if (!situation.shouldGuessOnGround()) { return false; }

        // for debugging purposed
        QString *details = /*CBuildConfig::isLocalDeveloperDebugBuild() ? &m_onGroundGuessingDetails :*/ nullptr;

        // Non VTOL aircraft have to move to be not on ground
        const bool vtol = model.isVtol();
        if (!vtol)
        {
            if (situation.getGroundSpeed().isNegativeWithEpsilonConsidered())
            {
                situation.setOnGroundInfo({ COnGroundInfo::OnGround, COnGroundInfo::OnGroundByGuessing });
                if (details) { *details = QStringLiteral("No VTOL, push back"); }
                return true;
            }

            if (!situation.isMoving())
            {
                situation.setOnGroundInfo({ COnGroundInfo::OnGround, COnGroundInfo::OnGroundByGuessing });
                if (details) { *details = QStringLiteral("No VTOL, not moving => on ground"); }
                return true;
            }
        }

        // not on ground is default
        situation.setOnGroundInfo({ COnGroundInfo::NotOnGround, COnGroundInfo::OnGroundByGuessing });

        CLength cg = situation.hasCG() ? situation.getCG() : model.getCG();
        CSpeed guessedRotateSpeed = CSpeed::null();
        CSpeed sureRotateSpeed = CSpeed(130, CSpeedUnit::kts());
        model.getAircraftIcaoCode().guessModelParameters(cg, guessedRotateSpeed);
        if (!guessedRotateSpeed.isNull())
        {
            // does the value make any sense?
            const bool validGuessedSpeed = (guessedRotateSpeed.value(CSpeedUnit::km_h()) > 5.0);
            SWIFT_VERIFY_X(validGuessedSpeed, Q_FUNC_INFO, "Wrong guessed value for lift off");
            if (!validGuessedSpeed) { guessedRotateSpeed = CSpeed(80, CSpeedUnit::kts()); } // fix
            sureRotateSpeed = guessedRotateSpeed * 1.25;
        }

        // "extreme" values for which we are surely not on ground
        if (qAbs(situation.getPitch().value(CAngleUnit::deg())) > 20)
        {
            if (details) { *details = QStringLiteral("max.pitch"); }
            return true;
        } // some tail wheel aircraft already have 11° pitch on ground
        if (qAbs(situation.getBank().value(CAngleUnit::deg())) > 10)
        {
            if (details) { *details = QStringLiteral("max.bank"); }
            return true;
        }
        if (situation.getGroundSpeed() > sureRotateSpeed)
        {
            if (details) { *details = u"gs. > vr " % sureRotateSpeed.valueRoundedWithUnit(1); }
            return true;
        }

        // use the most accurate or reliable guesses here first
        // ------------------------------------------------------
        // by elevation
        // we can detect "on ground" (underflow, near ground), but not "not on ground" because of overflow

        // we can detect on ground for underflow, but not for overflow (so we can not rely on NotOnGround)
        COnGroundInfo og(cg, situation.getGroundDistance(cg));
        if (og.getOnGround() == COnGroundInfo::OnGround)
        {
            if (details) { *details = QStringLiteral("elevation on ground"); }
            og.setOnGroundDetails(COnGroundInfo::OnGroundByGuessing);
            situation.setOnGroundInfo(og);
            return true;
        }

        if (!isNull())
        {
            if (!vtol && wasConstOnGround())
            {
                if (isRotatingUp())
                {
                    // not OG
                    if (details) { *details = QStringLiteral("rotating up detected"); }
                    return true;
                }

                // here we stick to ground until we detect rotate up
                situation.setOnGroundInfo({ COnGroundInfo::OnGround, COnGroundInfo::OnGroundByGuessing });
                if (details) { *details = QStringLiteral("waiting for rotating up"); }
                return true;
            }

            if (isConstAscending())
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
            situation.setOnGroundInfo({ COnGroundInfo::OnGroundSituationUnknown, COnGroundInfo::NotSetGroundDetails });
            return false;
        }

        // guessed speed null -> vtol
        if (!guessedRotateSpeed.isNull())
        {
            // does the value make any sense?
            if (situation.getGroundSpeed() < guessedRotateSpeed)
            {
                situation.setOnGroundInfo({ COnGroundInfo::OnGround, COnGroundInfo::OnGroundByGuessing });
                if (details)
                {
                    *details = QStringLiteral("Guessing, max.guessed gs.") +
                               guessedRotateSpeed.valueRoundedWithUnit(CSpeedUnit::kts(), 1);
                }
                return true;
            }
        }

        // not sure, but this is a guess
        if (details) { *details = QStringLiteral("Fall through"); }
        return true;
    }

    bool CAircraftSituationChange::hasSceneryDeviation() const { return !m_guessedSceneryDeviation.isNull(); }

    bool CAircraftSituationChange::hasElevationDevWithinAllowedRange() const
    {
        if (m_elvStdDev.isNull()) { return false; }
        return m_elvStdDev < CAircraftSituation::allowedAltitudeDeviation();
    }

    bool CAircraftSituationChange::hasAltitudeDevWithinAllowedRange() const
    {
        if (m_altStdDev.isNull()) { return false; }
        return m_altStdDev < CAircraftSituation::allowedAltitudeDeviation();
    }

    QString CAircraftSituationChange::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        static const QString null("null");
        if (this->isNull()) { return null; }
        return u"CS: '" % this->getCallsign().asString() % u" ' ts: " % this->getTimestampAndOffset(true) %
               u" | situations:" % QString::number(m_situationsCount) % u" | ts adj.: " %
               QString::number(m_oldestAdjustedTimestampMSecsSinceEpoch) % u'-' %
               QString::number(m_latestAdjustedTimestampMSecsSinceEpoch) % u" | just takeoff: " %
               boolToYesNo(this->isJustTakingOff()) % u" just touchdown: " % boolToYesNo(this->isJustTouchingDown()) %
               u" | all gnd: " % boolToYesNo(this->isConstOnGround()) % u'/' % boolToYesNo(this->wasConstOnGround()) %
               u" | all not gnd: " % boolToYesNo(this->isConstNotOnGround()) % u'/' %
               boolToYesNo(this->wasConstNotOnGround()) % u" | ascending: " % boolToYesNo(this->isConstAscending()) %
               u" descending: " % boolToYesNo(this->isConstDescending()) % u" | accelerating.: " %
               boolToYesNo(this->isConstAccelerating()) % u" decelarating: " %
               boolToYesNo(this->isConstDecelarating()) % u" | rotate up: " % boolToYesNo(this->isRotatingUp()) %
               u" | push back: " % boolToYesNo(this->containsPushBack()) % u" | scenery delta: " %
               m_guessedSceneryDeviation.valueRoundedWithUnit(1) % u" [" % this->getSceneryDeviationHintAsString() %
               u"] | AGL delta: " % m_gndDistMean.valueRoundedWithUnit(1) % u'/' %
               m_gndDistStdDev.valueRoundedWithUnit(1) % u" | std.dev/mean: pitch " %
               m_pitchMean.valueRoundedWithUnit(1) % u'/' % m_pitchStdDev.valueRoundedWithUnit(1) % u" gs " %
               m_gsMean.valueRoundedWithUnit(1) % u'/' % m_gsStdDev.valueRoundedWithUnit(1) % u" alt. " %
               m_altMean.valueRoundedWithUnit(1) % u'/' % m_altStdDev.valueRoundedWithUnit(1) % u" elv. " %
               m_elvMean.valueRoundedWithUnit(1) % u'/' % m_elvStdDev.valueRoundedWithUnit(1);
    }

    QVariant CAircraftSituationChange::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            return ITimestampWithOffsetBased::propertyByIndex(index);
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCallsign: return m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
        case IndexSituationsCount: return QVariant::fromValue(m_situationsCount);
        case IndexConstAscending: return QVariant::fromValue(m_constAscending);
        case IndexConstDescending: return QVariant::fromValue(m_constDescending);
        case IndexConstNotOnGround: return QVariant::fromValue(m_constNotOnGround);
        case IndexConstOnGround: return QVariant::fromValue(m_constOnGround);
        case IndexIsNull: return QVariant::fromValue(this->isNull());
        case IndexJustTakingOff: return QVariant::fromValue(m_justTakeoff);
        case IndexJustTouchingDown: return QVariant::fromValue(m_justTouchdown);
        case IndexRotatingUp: return QVariant::fromValue(m_rotateUp);
        case IndexContainsPushBack: return QVariant::fromValue(m_containsPushBack);
        case IndexAltitudeMean: return QVariant::fromValue(m_altMean);
        case IndexAltitudeStdDev: return QVariant::fromValue(m_altStdDev);
        case IndexElevationMean: return QVariant::fromValue(m_elvMean);
        case IndexElevationStdDev: return QVariant::fromValue(m_elvStdDev);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAircraftSituationChange::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAircraftSituationChange>();
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
        case IndexCallsign: m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexSituationsCount: m_situationsCount = variant.toInt(); break;
        case IndexConstAscending: m_constAscending = variant.toBool(); break;
        case IndexConstDescending: m_constDescending = variant.toBool(); break;
        case IndexConstNotOnGround: m_constNotOnGround = variant.toBool(); break;
        case IndexConstOnGround: m_constOnGround = variant.toBool(); break;
        case IndexJustTakingOff: m_justTakeoff = variant.toBool(); break;
        case IndexJustTouchingDown: m_justTouchdown = variant.toBool(); break;
        case IndexRotatingUp: m_rotateUp = variant.toBool(); break;
        case IndexContainsPushBack: m_containsPushBack = variant.toBool(); break;
        case IndexIsNull:
        case IndexAltitudeMean:
        case IndexAltitudeStdDev:
        case IndexElevationMean:
        case IndexElevationStdDev: break; // read only
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CAircraftSituationChange::comparePropertyByIndex(CPropertyIndexRef index,
                                                         const CAircraftSituationChange &compareValue) const
    {
        if (index.isMyself())
        {
            return ITimestampWithOffsetBased::comparePropertyByIndex(CPropertyIndex(), compareValue);
        }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue);
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCallsign:
            return m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
        case IndexSituationsCount:
            return Compare::compare(this->getSituationsCount(), compareValue.getSituationsCount());
        case IndexConstAscending: return Compare::compare(this->isConstAscending(), compareValue.isConstAscending());
        case IndexConstDescending: return Compare::compare(this->isConstDescending(), compareValue.isConstDescending());
        case IndexConstNotOnGround:
            return Compare::compare(this->isConstNotOnGround(), compareValue.isConstNotOnGround());
        case IndexConstOnGround: return Compare::compare(this->isConstOnGround(), compareValue.isConstOnGround());
        case IndexJustTakingOff: return Compare::compare(this->isJustTakingOff(), compareValue.isJustTakingOff());
        case IndexJustTouchingDown:
            return Compare::compare(this->isJustTouchingDown(), compareValue.isJustTouchingDown());
        case IndexRotatingUp: return Compare::compare(this->isRotatingUp(), compareValue.isRotatingUp());
        case IndexContainsPushBack: return Compare::compare(this->containsPushBack(), compareValue.containsPushBack());
        case IndexIsNull: return Compare::compare(this->isNull(), compareValue.isNull());
        case IndexAltitudeMean:
            return m_altMean.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_altMean);
        case IndexAltitudeStdDev:
            return m_altStdDev.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_altStdDev);
        case IndexElevationMean:
            return m_elvMean.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_elvMean);
        case IndexElevationStdDev:
            return m_elvStdDev.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_elvStdDev);
        default: return CValueObject::comparePropertyByIndex(index, *this);
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
        return 0;
    }

    bool CAircraftSituationChange::calculateStdDeviations(const CAircraftSituationList &situations, const CLength &cg)
    {
        if (situations.isEmpty()) { return false; }

        const QPair<CSpeed, CSpeed> gsStdDevMean = situations.groundSpeedStandardDeviationAndMean();
        m_gsStdDev = gsStdDevMean.first;
        m_gsMean = gsStdDevMean.second;

        const QPair<CAngle, CAngle> pitchStdDevMean = situations.pitchStandardDeviationAndMean();
        m_pitchStdDev = pitchStdDevMean.first;
        m_pitchMean = pitchStdDevMean.second;

        const QList<double> altValues = situations.altitudeValues(CAltitude::defaultUnit());
        if (altValues.size() == situations.size())
        {
            const QPair<double, double> altDevMean = CMathUtils::standardDeviationAndMean(altValues);
            m_altStdDev = CAltitude(altDevMean.first, CAltitude::MeanSeaLevel, CAltitude::defaultUnit());
            m_altMean = CAltitude(altDevMean.second, CAltitude::MeanSeaLevel, CAltitude::defaultUnit());
        }

        const QList<double> elvValues = situations.elevationValues(CAltitude::defaultUnit());
        if (elvValues.size() == situations.size())
        {
            const QPair<double, double> elvDevMean = CMathUtils::standardDeviationAndMean(elvValues);
            m_elvStdDev = CAltitude(elvDevMean.first, CAltitude::MeanSeaLevel, CAltitude::defaultUnit());
            m_elvMean = CAltitude(elvDevMean.second, CAltitude::MeanSeaLevel, CAltitude::defaultUnit());

            if (altValues.size() == situations.size())
            {
                QList<double> gndDistance;
                for (int i = 0; i < altValues.size(); i++)
                {
                    const double delta = altValues[i] - elvValues[i];
                    gndDistance.push_back(delta);
                }
                const QPair<double, double> gndDistanceDevMean = CMathUtils::standardDeviationAndMean(gndDistance);
                m_gndDistStdDev = CLength(gndDistanceDevMean.first, CAltitude::defaultUnit());
                m_gndDistMean = CLength(gndDistanceDevMean.second, CAltitude::defaultUnit());

                const auto gndDistMinMax = std::minmax_element(gndDistance.constBegin(), gndDistance.constEnd());
                const double gndDistMin = *gndDistMinMax.first;
                const double gndDistMax = *gndDistMinMax.second;
                m_minGroundDistance = CLength(gndDistMin, CAltitude::defaultUnit());
                m_maxGroundDistance = CLength(gndDistMax, CAltitude::defaultUnit());
                this->guessSceneryDeviation(cg);
            }
        }
        return true;
    }

    const CAircraftSituationChange &CAircraftSituationChange::null()
    {
        static const CAircraftSituationChange null;
        return null;
    }

    const QString &CAircraftSituationChange::guessedSceneryDeviationToString(GuessedSceneryDeviation hint)
    {
        static const QString noInfo("no info");
        static const QString completeOg("complete og");
        static const QString wasOg("was og");
        static const QString smallAGLDev("small AGL dev. near gnd.");

        switch (hint)
        {
        case AllOnGround: return completeOg;
        case WasOnGround: return wasOg;
        case SmallAGLDeviationNearGround: return smallAGLDev;
        case NoDeviationInfo:
        default: break;
        }
        return noInfo;
    }

    void CAircraftSituationChange::setSceneryDeviation(const CLength &deviation, const CLength &cg,
                                                       CAircraftSituationChange::GuessedSceneryDeviation hint)
    {
        m_guessedSceneryDeviation = deviation;
        m_guessedSceneryDeviationCG = cg.isNull() ? CLength::null() : deviation - cg;
        this->setSceneryDeviationHint(hint);
    }

    void CAircraftSituationChange::guessSceneryDeviation(const CLength &cg)
    {
        m_guessedSceneryDeviation = CLength::null();
        this->setSceneryDeviationHint(NoDeviationInfo);
        if (m_gndDistStdDev.isNull()) { return; }
        if (m_gndDistMean.isNull()) { return; }

        // only for a small deviation we can calculate scenery differemce
        static const CLength maxDeviation(2, CLengthUnit::ft());

        // Small deviation means "const" AGL
        if (m_gndDistStdDev <= maxDeviation)
        {
            do {
                if (this->isConstOnGround())
                {
                    this->setSceneryDeviation(m_gndDistMean, cg, AllOnGround);
                    break;
                }
                if (this->wasConstNotOnGround())
                {
                    this->setSceneryDeviation(m_gndDistMean, cg, WasOnGround);
                    break;
                }
                if (!m_altStdDev.isNull() && m_altStdDev <= maxDeviation)
                {
                    // small alt.deviation too!
                    if (!m_maxGroundDistance.isNull() && m_maxGroundDistance < cg)
                    {
                        if (this->isConstOnGround())
                        {
                            this->setSceneryDeviation(m_gndDistMean, cg, SmallAGLDeviationNearGround);
                            break;
                        }
                    }
                }
            }
            while (false);
        }
    }
} // namespace swift::misc::aviation
