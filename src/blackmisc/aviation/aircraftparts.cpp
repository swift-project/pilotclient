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
            const QString s =
                m_lights.toQString(i18n) %
                " gear down: " %
                BlackMisc::boolToYesNo(m_gearDown) %
                " flaps pct: " %
                QString::number(m_flapsPercentage) %
                " spoilers out: " %
                BlackMisc::boolToYesNo(m_spoilersOut) %
                " engines on: " %
                m_engines.toQString(i18n) %
                " on ground: " %
                BlackMisc::boolToYesNo(m_isOnGround);
            return s;
        }

        CVariant CAircraftParts::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEngines:
                return CVariant::fromValue(this->m_engines);
            case IndexFlapsPercentage:
                return CVariant::fromValue(this->m_flapsPercentage);
            case IndexGearDown:
                return CVariant::fromValue(this->m_gearDown);
            case IndexLights:
                return this->m_lights.propertyByIndex(index.copyFrontRemoved());
            case IndexSpoilersOut:
                return CVariant::fromValue(this->m_spoilersOut);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftParts::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftParts>(); return; }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEngines:
                this->m_engines = variant.to < decltype(this->m_engines) > ();
                break;
            case IndexFlapsPercentage:
                this->m_flapsPercentage = variant.toInt();
                break;
            case IndexGearDown:
                this->m_gearDown = variant.toBool();
                break;
            case IndexLights:
                this->m_lights.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexSpoilersOut:
                this->m_spoilersOut = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CAircraftParts::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftParts &compareValue) const
        {
            if (index.isMyself()) { return ITimestampBased::comparePropertyByIndex(CPropertyIndex(), compareValue); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEngines:
                return Compare::compare(this->getEnginesCount(), compareValue.getEnginesCount());
            case IndexFlapsPercentage:
                return Compare::compare(this->m_flapsPercentage, compareValue.getFlapsPercent());
            case IndexGearDown:
                return Compare::compare(this->m_gearDown, compareValue.isGearDown());
            case IndexLights:
                break;
            case IndexSpoilersOut:
                return Compare::compare(this->m_spoilersOut, compareValue.isSpoilersOut());
            default:
                break;
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
            return this->m_engines.getEngine(number);
        }

        bool CAircraftParts::isEngineOn(int number) const
        {
            return this->m_engines.isEngineOn(number);
        }

        double CAircraftParts::isOnGroundInterpolated() const
        {
            if (this->m_isOnGroundInterpolated < 0)
            {
                return this->m_isOnGround ? 1.0 : 0.0;
            }
            return this->m_isOnGroundInterpolated;
        }

    } // namespace
} // namespace
