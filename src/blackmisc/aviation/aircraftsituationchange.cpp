/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"
#include "QStringBuilder"
#include <QtGlobal>
#include <QPair>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;
using namespace BlackConfig;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituationChange::CAircraftSituationChange() {}

        CAircraftSituationChange::CAircraftSituationChange(const CAircraftSituation &s1, const CAircraftSituation &s2) :
            CAircraftSituationChange::CAircraftSituationChange(CAircraftSituationList({s1, s2})) {}

        CAircraftSituationChange::CAircraftSituationChange(const CAircraftSituationList &situations, bool alreadySortedLatestFirst, bool calcStdDeviations)
        {
            if (situations.size() < 2) { return; }
            const CAircraftSituationList sorted(alreadySortedLatestFirst ? situations : situations.getSortedAdjustedLatestFirst());

            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                Q_ASSERT_X(sorted.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Wrong sort order or NULL position");
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
            m_constOnGround =  sorted.isConstOnGround();
            m_constNotOnGround = sorted.isConstNotOnGround();
            m_justTakeoff = sorted.isJustTakingOff(true);
            m_justTouchdown = sorted.isJustTouchingDown(true);
            m_constAccelerating = sorted.isConstAccelerating(true);
            m_constDecelerating = sorted.isConstDecelarating(true);
            m_containsPushBack = sorted.containsPushBack();

            if (sorted.size() >= 3)
            {
                const CAircraftSituationList sortedWithout = sorted.withoutFrontSituation();
                m_wasNotOnGround = sortedWithout.isConstNotOnGround();
                m_wasOnGround = sortedWithout.isConstOnGround();
            }

            if (calcStdDeviations)
            {
                this->calculateStdDeviations(situations);
                m_rotateUp = sorted.front().getPitch() > (m_pitchMean + m_pitchStdDev);
            }
            else
            {
                m_rotateUp = sorted.isRotatingUp(true);
            }
        }

        CLength CAircraftSituationChange::getGuessedSceneryDeviation(const CLength &cg) const
        {
            if (cg.isNull()) { return this->guessedSceneryDeviation(); }
            if (this->guessedSceneryDeviation().isNull()) { return CLength::null(); }
            return this->guessedSceneryDeviation() - cg;
        }

        bool CAircraftSituationChange::hasSceneryDeviation() const
        {
            return !m_guessedSceneryDeviation.isNull();
        }

        QString CAircraftSituationChange::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString null("null");
            if (this->isNull()) { return null; }
            return QStringLiteral("CS: '") % this->getCallsign().asString() %
                   QStringLiteral(" ' ts: ") % this->getTimestampAndOffset(true) %
                   QStringLiteral(" | situations:") % QString::number(m_situationsCount) %
                   QStringLiteral(" | ts adj.: ") % QString::number(m_oldestAdjustedTimestampMSecsSinceEpoch) % QStringLiteral("-") % QString::number(m_latestAdjustedTimestampMSecsSinceEpoch) %
                   QStringLiteral(" | just takeoff: ") % boolToYesNo(this->isJustTakingOff()) % QStringLiteral(" just touchdown: ") % boolToYesNo(this->isJustTouchingDown()) %
                   QStringLiteral(" | all gnd: ") % boolToYesNo(this->isConstOnGround()) % QStringLiteral("/") % boolToYesNo(this->wasConstOnGround()) %
                   QStringLiteral(" | all not gnd: ") % boolToYesNo(this->isConstNotOnGround()) % QStringLiteral("/") % boolToYesNo(this->wasConstNotOnGround()) %
                   QStringLiteral(" | ascending: ") % boolToYesNo(this->isConstAscending()) % QStringLiteral(" descending: ") % boolToYesNo(this->isConstDescending()) %
                   QStringLiteral(" | accelerating.: ") % boolToYesNo(this->isConstAccelerating()) % QStringLiteral(" decelarating: ") % boolToYesNo(this->isConstDecelarating()) %
                   QStringLiteral(" | rotate up: ") % boolToYesNo(this->isRotatingUp()) %
                   QStringLiteral(" | push back: ") % boolToYesNo(this->containsPushBack()) %
                   QStringLiteral(" | scenery delta: ") % m_guessedSceneryDeviation.valueRoundedWithUnit(1) % QStringLiteral(" [") % this->getGuessedSceneryDeviationAsString() %
                   QStringLiteral("] | AGL delta: ") % m_altAglMean.valueRoundedWithUnit(1) % QStringLiteral("/") %  m_altAglStdDev.valueRoundedWithUnit(1) %
                   QStringLiteral(" | std.dev/mean: pitch ") %  m_pitchMean.valueRoundedWithUnit(1) % QStringLiteral("/") %  m_pitchStdDev.valueRoundedWithUnit(1) %
                   QStringLiteral(" gs ") % m_gsMean.valueRoundedWithUnit(1) % QStringLiteral("/") % m_gsStdDev.valueRoundedWithUnit(1) %
                   QStringLiteral(" alt. ") % m_altMean.valueRoundedWithUnit(1) % QStringLiteral("/") % m_altStdDev.valueRoundedWithUnit(1) %
                   QStringLiteral(" elv. ") % m_elvMean.valueRoundedWithUnit(1) % QStringLiteral("/") % m_elvStdDev.valueRoundedWithUnit(1);
        }

        CVariant CAircraftSituationChange::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign: return m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexConstAscending: return CVariant::from(m_constAscending);
            case IndexConstDescending: return CVariant::from(m_constDescending);
            case IndexConstNotOnGround: return CVariant::from(m_constNotOnGround);
            case IndexConstOnGround: return CVariant::from(m_constOnGround);
            case IndexIsNull: return CVariant::from(this->isNull());
            case IndexJustTakingOff: return CVariant::from(m_justTakeoff);
            case IndexJustTouchingDown: return CVariant::from(m_justTouchdown);
            case IndexRotatingUp: return CVariant::from(m_rotateUp);
            case IndexContainsPushBack: return CVariant::from(m_containsPushBack);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftSituationChange::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftSituationChange>(); return; }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { ITimestampWithOffsetBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign: m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexConstAscending: m_constAscending = variant.toBool(); break;
            case IndexConstDescending: m_constDescending = variant.toBool(); break;
            case IndexConstNotOnGround: m_constNotOnGround = variant.toBool(); break;
            case IndexConstOnGround: m_constOnGround = variant.toBool(); break;
            case IndexJustTakingOff: m_justTakeoff = variant.toBool(); break;
            case IndexJustTouchingDown: m_justTouchdown = variant.toBool(); break;
            case IndexRotatingUp: m_rotateUp = variant.toBool(); break;
            case IndexContainsPushBack: m_containsPushBack = variant.toBool(); break;
            case IndexIsNull: break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        bool CAircraftSituationChange::calculateStdDeviations(const CAircraftSituationList &situations)
        {
            if (situations.isEmpty()) { return false; }

            const QPair<CSpeed, CSpeed> gsStdDevMean = situations.groundSpeedStandardDeviationAndMean();
            m_gsStdDev = gsStdDevMean.first;
            m_gsMean = gsStdDevMean.second;

            const QPair<CAngle, CAngle> pitchStdDevMean = situations.pitchStandardDeviationAndMean();
            m_pitchStdDev = pitchStdDevMean.first;
            m_pitchMean = pitchStdDevMean.second;

            const QList<double> altValues = situations.altitudeValues(CLengthUnit::ft());
            if (altValues.size() == situations.size())
            {
                const QPair<double, double> altFt = CMathUtils::standardDeviationAndMean(altValues);
                m_altStdDev = CAltitude(altFt.first, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                m_altMean = CAltitude(altFt.second, CAltitude::MeanSeaLevel, CLengthUnit::ft());
            }

            const QList<double> elvValues = situations.elevationValues(CLengthUnit::ft());
            if (elvValues.size() == situations.size())
            {
                const QPair<double, double> elvFt = CMathUtils::standardDeviationAndMean(elvValues);
                m_elvStdDev = CAltitude(elvFt.first, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                m_elvMean = CAltitude(elvFt.second, CAltitude::MeanSeaLevel, CLengthUnit::ft());

                if (altValues.size() == situations.size())
                {
                    QList<double> altElvDeltas;
                    for (int i = 0; i < altValues.size(); i++)
                    {
                        const double delta = altValues[i] - elvValues[i];
                        altElvDeltas.push_back(delta);
                    }
                    const QPair<double, double> deltaFt = CMathUtils::standardDeviationAndMean(altElvDeltas);
                    m_altAglStdDev = CLength(deltaFt.first, CLengthUnit::ft());
                    m_altAglMean = CLength(deltaFt.second, CLengthUnit::ft());

                    this->guessSceneryDeviation(situations);
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
            static const QString someOg("some og");

            switch (hint)
            {
            case AllOnGround: return completeOg;
            case WasOnGround: return wasOg;
            case SomeSituationsOnGround: return someOg;
            case NoDeviationInfo:
            default: break;
            }
            return noInfo;
        }

        void CAircraftSituationChange::guessSceneryDeviation(const CAircraftSituationList &situations)
        {
            m_guessedSceneryDeviation = CLength::null();
            if (m_altAglStdDev.isNull()) { return; }
            if (m_altAglMean.isNull()) { return; }

            // only for a small deviation we can calculate scenery differemce
            static const CLength maxDeviation(2, CLengthUnit::ft());

            // On ground or was on ground
            if (this->wasConstOnGround())
            {
                if (m_altAglStdDev > maxDeviation) { return; }
                m_guessedSceneryDeviation = m_altAglMean;
                this->setSceneryDeviationHint(this->isConstOnGround() ? AllOnGround : WasOnGround);
            }
            else
            {
                const CAircraftSituationList situationsOg = situations.findOnGroundWithElevation(CAircraftSituation::OnGround);
                if (situationsOg.size() >= 2)
                {
                    const QPair<CAltitude, CAltitude> altAgl = situationsOg.altitudeAglStandardDeviationAndMean();
                    if (altAgl.first > maxDeviation) { return; } // deviation
                    m_guessedSceneryDeviation = altAgl.second; // AGL mean;
                    this->setSceneryDeviationHint(SomeSituationsOnGround);
                }
            }
        }
    } // namespace
} // namespace
