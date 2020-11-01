/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/propertyindexref.h"
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
        void CAircraftSituationChange::registerMetadata()
        {
            CValueObject<CAircraftSituationChange>::registerMetadata();
            qRegisterMetaType<CAircraftSituationChange::GuessedSceneryDeviation>();
        }

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

        bool CAircraftSituationChange::hasElevationDevWithinAllowedRange() const
        {
            if (m_elvStdDev.isNull()) { return false; }
            return m_elvStdDev < allowedAltitudeDeviation();
        }

        bool CAircraftSituationChange::hasAltitudeDevWithinAllowedRange() const
        {
            if (m_altStdDev.isNull()) { return false; }
            return m_altStdDev < allowedAltitudeDeviation();
        }

        QString CAircraftSituationChange::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n)
            static const QString null("null");
            if (this->isNull()) { return null; }
            return u"CS: '" % this->getCallsign().asString() %
                   u" ' ts: " % this->getTimestampAndOffset(true) %
                   u" | situations:" % QString::number(m_situationsCount) %
                   u" | ts adj.: " % QString::number(m_oldestAdjustedTimestampMSecsSinceEpoch) % u'-' % QString::number(m_latestAdjustedTimestampMSecsSinceEpoch) %
                   u" | just takeoff: " % boolToYesNo(this->isJustTakingOff()) % u" just touchdown: " % boolToYesNo(this->isJustTouchingDown()) %
                   u" | all gnd: " % boolToYesNo(this->isConstOnGround()) % u'/' % boolToYesNo(this->wasConstOnGround()) %
                   u" | all not gnd: " % boolToYesNo(this->isConstNotOnGround()) % u'/' % boolToYesNo(this->wasConstNotOnGround()) %
                   u" | ascending: " % boolToYesNo(this->isConstAscending()) % u" descending: " % boolToYesNo(this->isConstDescending()) %
                   u" | accelerating.: " % boolToYesNo(this->isConstAccelerating()) % u" decelarating: " % boolToYesNo(this->isConstDecelarating()) %
                   u" | rotate up: " % boolToYesNo(this->isRotatingUp()) %
                   u" | push back: " % boolToYesNo(this->containsPushBack()) %
                   u" | scenery delta: " % m_guessedSceneryDeviation.valueRoundedWithUnit(1) % u" [" % this->getSceneryDeviationHintAsString() %
                   u"] | AGL delta: " % m_gndDistMean.valueRoundedWithUnit(1) % u'/' %  m_gndDistStdDev.valueRoundedWithUnit(1) %
                   u" | std.dev/mean: pitch " %  m_pitchMean.valueRoundedWithUnit(1) % u'/' %  m_pitchStdDev.valueRoundedWithUnit(1) %
                   u" gs " % m_gsMean.valueRoundedWithUnit(1) % u'/' % m_gsStdDev.valueRoundedWithUnit(1) %
                   u" alt. " % m_altMean.valueRoundedWithUnit(1) % u'/' % m_altStdDev.valueRoundedWithUnit(1) %
                   u" elv. " % m_elvMean.valueRoundedWithUnit(1) % u'/' % m_elvStdDev.valueRoundedWithUnit(1);
        }

        QVariant CAircraftSituationChange::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::propertyByIndex(index); }

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
            if (index.isMyself()) { (*this) = variant.value<CAircraftSituationChange>(); return; }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { ITimestampWithOffsetBased::setPropertyByIndex(index, variant); return; }

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
            case IndexElevationStdDev:
                break; // read only
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftSituationChange::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftSituationChange &compareValue) const
        {
            if (index.isMyself()) { return ITimestampWithOffsetBased::comparePropertyByIndex(CPropertyIndex(), compareValue); }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign: return m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexSituationsCount: return Compare::compare(this->getSituationsCount(), compareValue.getSituationsCount());
            case IndexConstAscending: return Compare::compare(this->isConstAscending(), compareValue.isConstAscending());
            case IndexConstDescending: return Compare::compare(this->isConstDescending(), compareValue.isConstDescending());
            case IndexConstNotOnGround: return Compare::compare(this->isConstNotOnGround(), compareValue.isConstNotOnGround());
            case IndexConstOnGround: return Compare::compare(this->isConstOnGround(), compareValue.isConstOnGround());
            case IndexJustTakingOff: return Compare::compare(this->isJustTakingOff(), compareValue.isJustTakingOff());
            case IndexJustTouchingDown: return Compare::compare(this->isJustTouchingDown(), compareValue.isJustTouchingDown());
            case IndexRotatingUp: return Compare::compare(this->isRotatingUp(), compareValue.isRotatingUp());
            case IndexContainsPushBack: return Compare::compare(this->containsPushBack(), compareValue.containsPushBack());
            case IndexIsNull: return Compare::compare(this->isNull(), compareValue.isNull());
            case IndexAltitudeMean: return m_altMean.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_altMean);
            case IndexAltitudeStdDev: return m_altStdDev.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_altStdDev);
            case IndexElevationMean: return m_elvMean.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_elvMean);
            case IndexElevationStdDev: return m_elvStdDev.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.m_elvStdDev);
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

        const CLength &CAircraftSituationChange::allowedAltitudeDeviation()
        {
            // approx. 1 meter
            static const CLength allowedStdDev(3, CLengthUnit::ft());
            return allowedStdDev;
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
                    if (this->wasConstNotOnGround()) { this->setSceneryDeviation(m_gndDistMean, cg, WasOnGround); break; }
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
