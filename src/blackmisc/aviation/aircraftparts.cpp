/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"

#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {
        QString CAircraftParts::convertToQString(bool i18n) const
        {
            return QStringLiteral("ts: ") % this->getFormattedTimestampAndOffset(true) %
                   QStringLiteral(" lights: ") % m_lights.toQString(i18n) %
                   QStringLiteral(" gear down: ") % BlackMisc::boolToYesNo(m_gearDown) %
                   QStringLiteral(" flaps pct: ") % QString::number(m_flapsPercentage) %
                   QStringLiteral(" spoilers out: ") % BlackMisc::boolToYesNo(m_spoilersOut) %
                   QStringLiteral(" engines on: ") % m_engines.toQString(i18n) %
                   QStringLiteral(" on ground: ") % BlackMisc::boolToYesNo(m_isOnGround);
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

        double CAircraftParts::isOnGroundInterpolated() const
        {
            if (m_isOnGroundInterpolated < 0)
            {
                return m_isOnGround ? 1.0 : 0.0;
            }
            return m_isOnGroundInterpolated;
        }
    } // namespace
} // namespace
