/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftparts.h"

namespace BlackMisc
{
    namespace Aviation
    {

        QString CAircraftParts::convertToQString(bool i18n) const
        {
            QString s;
            s += m_lights.toQString(i18n);
            s += " gear down: ";
            s += BlackMisc::boolToYesNo(m_gearDown);
            s += " flaps pct: ";
            s += QString::number(m_flapsPercentage);
            s += " spoilers out: ";
            s += BlackMisc::boolToYesNo(m_spoilersOut);
            s += " engines on: ";
            s += m_engines.toQString(i18n);
            s += " on ground: ";
            s += BlackMisc::boolToYesNo(m_isOnGround);
            return s;
        }

        CVariant CAircraftParts::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index))
            {
                return ITimestampBased::propertyByIndex(index);
            }

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

        void CAircraftParts::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftParts>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(variant, index);
                return;
            }

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
                this->m_lights.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexSpoilersOut:
                this->m_spoilersOut = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
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

    } // namespace
} // namespace
