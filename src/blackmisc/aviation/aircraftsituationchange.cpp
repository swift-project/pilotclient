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

        CAircraftSituationChange::CAircraftSituationChange(const CAircraftSituationList &situations, const PhysicalQuantities::CLength &cg, bool isVtol, bool alreadySortedLatestFirst, bool calcStdDeviations)
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
            else
            {
                m_rotateUp = sorted.isRotatingUp(true);
            }
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
                   QStringLiteral(" | scenery delta: ") % m_guessedSceneryDeviation.valueRoundedWithUnit(1) % QStringLiteral(" [") % this->getSceneryDeviationHintAsString() %
                   QStringLiteral("] | AGL delta: ") % m_gndDistMean.valueRoundedWithUnit(1) % QStringLiteral("/") %  m_gndDistStdDev.valueRoundedWithUnit(1) %
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

        void CAircraftSituationChange::setSceneryDeviation(const CLength &deviation, const CLength &cg, CAircraftSituationChange::GuessedSceneryDeviation hint)
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
                do
                {
                    if (this->isConstOnGround()) { this->setSceneryDeviation(m_gndDistMean, cg, AllOnGround); break; }
                    if (this->isConstOnGround()) { this->setSceneryDeviation(m_gndDistMean, cg, WasOnGround); break; }
                    if (!m_altStdDev.isNull() && m_altStdDev <= maxDeviation)
                    {
                        // small alt.deviation too!
                        if (!m_maxGroundDistance.isNull() && m_maxGroundDistance < cg)
                        {
                            if (this->isConstOnGround()) { this->setSceneryDeviation(m_gndDistMean, cg, SmallAGLDeviationNearGround); break; }
                        }
                    }
                }
                while (false);
            }
        }
    } // namespace
} // namespace
