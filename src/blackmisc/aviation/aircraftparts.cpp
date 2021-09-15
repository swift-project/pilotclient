/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackConfig;

namespace BlackMisc::Aviation
{
    CAircraftParts::CAircraftParts(int flapsPercent) : m_flapsPercentage(flapsPercent) {}

    CAircraftParts::CAircraftParts(const CAircraftLights &lights, bool gearDown, int flapsPercent, bool spoilersOut, const CAircraftEngineList &engines, bool onGround)
        : m_lights(lights), m_engines(engines), m_flapsPercentage(flapsPercent), m_gearDown(gearDown),
            m_spoilersOut(spoilersOut), m_isOnGround(onGround)
    {}

    CAircraftParts::CAircraftParts(const CAircraftLights &lights, bool gearDown, int flapsPercent, bool spoilersOut, const CAircraftEngineList &engines, bool onGround, qint64 timestamp)
        : m_lights(lights), m_engines(engines), m_flapsPercentage(flapsPercent), m_gearDown(gearDown),
            m_spoilersOut(spoilersOut), m_isOnGround(onGround)
    {
        this->setMSecsSinceEpoch(timestamp);
    }

    QString CAircraftParts::convertToQString(bool i18n) const
    {
        return u"ts: " % this->getFormattedTimestampAndOffset(true) %
                u" details: " % this->getPartsDetailsAsString() %
                (m_guessingDetails.isEmpty() ? QString() : u" - " % m_guessingDetails) %
                u" | on ground: " % BlackMisc::boolToYesNo(m_isOnGround) %
                u" | lights: " % m_lights.toQString(i18n) %
                u" | gear down: " % BlackMisc::boolToYesNo(m_gearDown) %
                u" | flaps pct: " % QString::number(m_flapsPercentage) %
                u" | spoilers out: " % BlackMisc::boolToYesNo(m_spoilersOut) %
                u" | engines on: " % m_engines.toQString(i18n);
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

    bool CAircraftParts::equalValues(const CAircraftParts &other) const
    {
        // currently same as some values are diabled for comparison
        // but that could change in future
        return other == *this;
    }

    const CAircraftParts &CAircraftParts::null()
    {
        static const CAircraftParts null(-1);
        return null;
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

    QVariant CAircraftParts::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEngines: return QVariant::fromValue(m_engines);
        case IndexEnginesAsString: return QVariant::fromValue(m_engines.toQString(true));
        case IndexFlapsPercentage: return QVariant::fromValue(m_flapsPercentage);
        case IndexGearDown: return QVariant::fromValue(m_gearDown);
        case IndexLights: return m_lights.propertyByIndex(index.copyFrontRemoved());
        case IndexSpoilersOut: return QVariant::fromValue(m_spoilersOut);
        case IndexIsOnGround: return QVariant::fromValue(m_isOnGround);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAircraftParts::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CAircraftParts>(); return; }
        if (ITimestampWithOffsetBased::canHandleIndex(index)) { ITimestampWithOffsetBased::setPropertyByIndex(index, variant); return; }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEngines: m_engines = variant.value<decltype(m_engines)> (); break;
        case IndexFlapsPercentage: m_flapsPercentage = variant.toInt(); break;
        case IndexGearDown: m_gearDown = variant.toBool(); break;
        case IndexLights: m_lights.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexSpoilersOut: m_spoilersOut = variant.toBool(); break;
        case IndexIsOnGround: m_isOnGround = variant.toBool(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CAircraftParts::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftParts &compareValue) const
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
        case IndexIsOnGround: return Compare::compare(m_isOnGround, compareValue.isOnGround());
        case IndexLights: return m_lights.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getLights());
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

    bool CAircraftParts::isFixedGearDown() const
    {
        return this->isGearDown() || this->isOnGround();
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
} // namespace
