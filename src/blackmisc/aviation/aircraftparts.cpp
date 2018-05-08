/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodel.h"
#include "aircraftparts.h"
#include "aircraftlights.h"
#include "aircraftsituation.h"
#include "aircraftsituationchange.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackConfig;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftParts::CAircraftParts(int flapsPercent) : m_flapsPercentage(flapsPercent) {}

        CAircraftParts::CAircraftParts(const CAircraftLights &lights, bool gearDown, int flapsPercent, bool spoilersOut, const CAircraftEngineList &engines, bool onGround)
            : m_lights(lights), m_engines(engines), m_flapsPercentage(flapsPercent), m_gearDown(gearDown),
              m_spoilersOut(spoilersOut), m_isOnGround(onGround)
        {}

        QString CAircraftParts::convertToQString(bool i18n) const
        {
            return QStringLiteral("ts: ") % this->getFormattedTimestampAndOffset(true) %
                   QStringLiteral(" details: ") % this->getPartsDetailsAsString() %
                   (m_guessingDetails.isEmpty() ? QStringLiteral("") : QStringLiteral(" - ") % m_guessingDetails) %
                   QStringLiteral(" | on ground: ") % BlackMisc::boolToYesNo(m_isOnGround) %
                   QStringLiteral(" | lights: ") % m_lights.toQString(i18n) %
                   QStringLiteral(" | gear down: ") % BlackMisc::boolToYesNo(m_gearDown) %
                   QStringLiteral(" | flaps pct: ") % QString::number(m_flapsPercentage) %
                   QStringLiteral(" | spoilers out: ") % BlackMisc::boolToYesNo(m_spoilersOut) %
                   QStringLiteral(" | engines on: ") % m_engines.toQString(i18n);
        }

        QJsonObject CAircraftParts::toIncrementalJson() const
        {
            QJsonObject json = this->toJson();
            json.remove(attributeNameIsFullJson());
            json.insert(attributeNameIsFullJson(), QJsonValue(false));
            return json;
        }

        QJsonObject CAircraftParts::toFullJson() const
        {
            QJsonObject json = this->toJson();
            json.remove(attributeNameIsFullJson());
            json.insert(attributeNameIsFullJson(), QJsonValue(true));
            return json;
        }

        bool CAircraftParts::isNull() const
        {
            return this->getPartsDetails() == NotSet && m_flapsPercentage < 0;
        }

        const CAircraftParts &CAircraftParts::null()
        {
            static const CAircraftParts null(-1);
            return null;
        }

        CAircraftParts CAircraftParts::guessedParts(const CAircraftSituation &situation, const CAircraftSituationChange &change, const CAircraftModel &model)
        {
            CAircraftParts parts;
            parts.setPartsDetails(GuessedParts);
            parts.setLights(CAircraftLights::guessedLights(situation));

            QString *details = CBuildConfig::isLocalDeveloperDebugBuild() ? &parts.m_guessingDetails : nullptr;

            CAircraftEngineList engines;
            const bool vtol = model.isVtol();
            const int engineCount = model.getEngineCount();
            CSpeed guessedVRotate = CSpeed::null();
            CLength guessedCG = model.getCG();
            model.getAircraftIcaoCode().guessModelParameters(guessedCG, guessedVRotate);

            if (situation.getOnGroundDetails() != CAircraftSituation::NotSetGroundDetails)
            {
                // set some reasonable values
                const bool isOnGround = situation.isOnGround();
                engines.initEngines(engineCount, !isOnGround || situation.isMoving());
                parts.setGearDown(isOnGround);
                parts.setSpoilersOut(false);
                parts.setEngines(engines);

                if (!change.isNull())
                {
                    if (change.isConstDecelarating())
                    {
                        parts.setSpoilersOut(true);
                        parts.setFlapsPercent(10);
                        return parts;
                    }
                }

                const CSpeed slowSpeed = guessedVRotate * 0.30;
                if (situation.getGroundSpeed() < slowSpeed)
                {
                    if (details) { *details += QStringLiteral("slow speed <") % slowSpeed.valueRoundedWithUnit(1) % QStringLiteral(" on ground"); }
                    parts.setFlapsPercent(0);
                    return parts;
                }
                else
                {
                    if (details) { *details += QStringLiteral("faster speed >") % slowSpeed.valueRoundedWithUnit(1) % QStringLiteral(" on ground"); }
                    parts.setFlapsPercent(0);
                    return parts;
                }
            }
            else
            {
                if (details) { *details = QStringLiteral("no ground info");  }

                // no idea if on ground or not
                engines.initEngines(engineCount, true);
                parts.setEngines(engines);
                parts.setGearDown(true);
                parts.setSpoilersOut(false);
            }

            const double pitchDeg = situation.getPitch().value(CAngleUnit::deg());
            const bool isLikelyTakeOffOrClimbing = change.isNull() ?  pitchDeg > 20 : (change.isRotatingUp() || change.isConstAscending());
            const bool isLikelyLanding = change.isNull() ? false : change.isConstDescending();

            if (situation.hasGroundElevation())
            {
                const double nearGround1Ft = 300;
                const double nearGround2Ft = isLikelyTakeOffOrClimbing ? 500 : 1000;
                const double aGroundFt = situation.getHeightAboveGround().value(CLengthUnit::ft());
                static const QString detailsInfo("above ground: %1ft near grounds: %2ft %3ft likely takeoff: %4 likely landing: %5");

                if (details) { *details = detailsInfo.arg(aGroundFt).arg(nearGround1Ft).arg(nearGround2Ft).arg(boolToYesNo(isLikelyTakeOffOrClimbing), boolToYesNo(isLikelyLanding));  }
                if (aGroundFt < nearGround1Ft)
                {
                    if (details) { details->prepend(QStringLiteral("near ground: ")); }
                    parts.setGearDown(true);
                    parts.setFlapsPercent(25);
                }
                else if (aGroundFt < nearGround2Ft)
                {
                    if (details) { details->prepend(QStringLiteral("2nd layer: ")); }
                    const bool gearDown = !isLikelyTakeOffOrClimbing && (situation.getGroundSpeed() < guessedVRotate || isLikelyLanding);
                    parts.setGearDown(gearDown);
                    parts.setFlapsPercent(10);
                }
                else
                {
                    if (details) { details->prepend(QStringLiteral("airborne: ")); }
                    parts.setGearDown(false);
                    parts.setFlapsPercent(0);
                }
            }
            else
            {
                if (situation.getOnGroundDetails() != CAircraftSituation::NotSetGroundDetails)
                {
                    // we have no ground elevation but a ground info
                    if (situation.getOnGroundDetails() == CAircraftSituation::OnGroundByGuessing)
                    {
                        // should be OK
                        if (details) { *details = QStringLiteral("on ground, no elv.");  }
                    }
                    else
                    {
                        if (!vtol)
                        {
                            const bool gearDown = situation.getGroundSpeed() < guessedVRotate;
                            parts.setGearDown(gearDown);
                            if (details) { *details = QStringLiteral("not on ground elv., gs < ") + guessedVRotate.valueRoundedWithUnit(1);  }
                        }
                    }
                }
            }

            return parts;
        }

        const QString &CAircraftParts::partsDetailsToString(CAircraftParts::PartsDetails details)
        {
            static const QString guessed("guessed");
            static const QString notset("not set");
            static const QString fsd("FSD parts");

            switch (details)
            {
            case GuessedParts: return guessed;
            case FSDAircraftParts: return fsd;
            case NotSet: break;
            default: break;
            }
            return notset;
        }

        const QString &CAircraftParts::attributeNameIsFullJson()
        {
            static const QString a("is_full_data");
            return a;
        }

        CVariant CAircraftParts::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEngines: return CVariant::fromValue(m_engines);
            case IndexFlapsPercentage: return CVariant::fromValue(m_flapsPercentage);
            case IndexGearDown: return CVariant::fromValue(m_gearDown);
            case IndexLights: return m_lights.propertyByIndex(index.copyFrontRemoved());
            case IndexSpoilersOut: return CVariant::fromValue(m_spoilersOut);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftParts::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftParts>(); return; }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { ITimestampWithOffsetBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEngines: m_engines = variant.to < decltype(m_engines) > (); break;
            case IndexFlapsPercentage: m_flapsPercentage = variant.toInt(); break;
            case IndexGearDown: m_gearDown = variant.toBool(); break;
            case IndexLights: m_lights.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexSpoilersOut: m_spoilersOut = variant.toBool(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftParts::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftParts &compareValue) const
        {
            if (index.isMyself()) { return ITimestampWithOffsetBased::comparePropertyByIndex(CPropertyIndex(), compareValue); }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEngines: return Compare::compare(this->getEnginesCount(), compareValue.getEnginesCount());
            case IndexFlapsPercentage: return Compare::compare(m_flapsPercentage, compareValue.getFlapsPercent());
            case IndexGearDown: return Compare::compare(m_gearDown, compareValue.isGearDown());
            case IndexSpoilersOut: return Compare::compare(m_spoilersOut, compareValue.isSpoilersOut());
            case IndexLights:
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
            return 0;
        }

        CAircraftLights CAircraftParts::getAdjustedLights() const
        {
            CAircraftLights lights = this->getLights();
            const bool anyEngine = this->isAnyEngineOn();
            lights.setRecognitionOn(anyEngine);
            lights.setCabinOn(anyEngine);
            return lights;
        }

        void CAircraftParts::setAllLightsOn()
        {
            m_lights.setAllOn();
        }

        void CAircraftParts::setAllLightsOff()
        {
            m_lights.setAllOff();
        }

        CAircraftEngine CAircraftParts::getEngine(int number) const
        {
            return m_engines.getEngine(number);
        }

        bool CAircraftParts::isEngineOn(int number) const
        {
            return m_engines.isEngineOn(number);
        }

        bool CAircraftParts::isAnyEngineOn() const
        {
            return m_engines.isAnyEngineOn();
        }

        void CAircraftParts::setEngines(const CAircraftEngine &engine, int engineNumber)
        {
            CAircraftEngineList engines;
            engines.setEngines(engine, engineNumber);
            m_engines = engines;
        }

        void CAircraftParts::guessParts(const CAircraftSituation &situation, const CAircraftSituationChange &change, const CAircraftModel &model)
        {
            *this = guessedParts(situation, change, model);
        }
    } // namespace
} // namespace
