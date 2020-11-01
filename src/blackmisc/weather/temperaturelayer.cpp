/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Weather
    {

        CTemperatureLayer::CTemperatureLayer(const CAltitude &level,
                                             const CTemperature &value,
                                             const CTemperature &dewPoint,
                                             double relativeHumidity) :
            m_level(level), m_temperature(value), m_dewPoint(dewPoint), m_relativeHumidity(relativeHumidity)
        { }

        QVariant CTemperatureLayer::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLevel:
                return QVariant::fromValue(m_level);
            case IndexTemperature:
                return QVariant::fromValue(m_temperature);
            case IndexDewPoint:
                return QVariant::fromValue(m_dewPoint);
            case IndexRelativeHumidity:
                return QVariant::fromValue(m_relativeHumidity);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CTemperatureLayer::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CTemperatureLayer>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLevel:
                setLevel(variant.value<CAltitude>());
                break;
            case IndexTemperature:
                setTemperature(variant.value<CTemperature>());
                break;
            case IndexDewPoint:
                setDewPoint(variant.value<CTemperature>());
                break;
            case IndexRelativeHumidity:
                setRelativeHumidity(variant.value<double>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CTemperatureLayer::convertToQString(bool /** i18n **/) const
        {
            return QStringLiteral("%1 %2 at %3").arg(m_temperature.toQString(), QString::number(m_relativeHumidity), m_level.toQString());
        }

    } // namespace
} // namespace
