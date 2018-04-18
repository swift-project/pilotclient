/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftparts.h"
#include "aircraftlights.h"
#include "aircraftsituation.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"

#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;

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
            json.remove("is_full_data");
            json.insert("is_full_data", QJsonValue(false));
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

        CAircraftParts CAircraftParts::guessedParts(const CAircraftSituation &situation, bool vtol, int engineNumber)
        {
            CAircraftParts parts;
            CAircraftEngineList engines;
            parts.setLights(CAircraftLights::guessedLights(situation));

            // set some reasonable defaults
            const bool onGround = situation.isOnGround();
            parts.setGearDown(onGround);
            engines.initEngines(engineNumber, !onGround || situation.isMoving());

            if (situation.hasGroundElevation())
            {
                const double aGroundFt = situation.getHeightAboveGround().value(CLengthUnit::ft());
                if (aGroundFt < 1000)
                {
                    parts.setGearDown(true);
                    parts.setFlapsPercent(25);
                }
                else if (aGroundFt < 2000)
                {
                    parts.setGearDown(true);
                    parts.setFlapsPercent(10);
                }
                else
                {
                    parts.setGearDown(false);
                    parts.setFlapsPercent(0);
                }
            }
            else
            {
                if (!situation.hasInboundGroundDetails())
                {
                    // the ground flag is not reliable and we have no ground elevation
                    if (situation.getOnGroundDetails() == CAircraftSituation::OnGroundByGuessing)
                    {
                        // should be OK
                    }
                    else
                    {
                        if (!vtol)
                        {
                            const bool gearDown = situation.getGroundSpeed().value(CSpeedUnit::kts()) < 60;
                            parts.setGearDown(gearDown);
                        }
                    }
                }
            }

            parts.setEngines(engines);
            parts.setPartsDetails(GuessedParts);
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

        void CAircraftParts::guessParts(const CAircraftSituation &situation, bool vtol, int engineNumber)
        {
            *this = guessedParts(situation, vtol, engineNumber);
        }
    } // namespace
} // namespace
