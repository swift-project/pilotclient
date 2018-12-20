/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/weather/windlayer.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {

        CWindLayer::CWindLayer(const CAltitude &level, const CAngle &direction, const CSpeed &speed, const CSpeed &gustSpeed) :
            m_level(level), m_directionMain(direction), m_speed(speed), m_gustSpeed(gustSpeed)
        { }

        CVariant CWindLayer::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLevel:
                return CVariant::fromValue(m_level);
            case IndexDirection:
                return CVariant::fromValue(m_directionMain);
            case IndexDirectionVariable:
                return CVariant::fromValue(m_directionVariable);
            case IndexSpeed:
                return CVariant::fromValue(m_speed);
            case IndexGustSpeed:
                return CVariant::fromValue(m_gustSpeed);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CWindLayer::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CWindLayer>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLevel:
                setLevel(variant.value<CAltitude>());
                break;
            case IndexDirection:
                setDirection(variant.value<CAngle>());
                break;
            case IndexDirectionVariable:
                setDirectionVariable(variant.toBool());
                break;
            case IndexSpeed:
                setSpeed(variant.value<CSpeed>());
                break;
            case IndexGustSpeed:
                setGustSpeed(variant.value<CSpeed>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CWindLayer::convertToQString(bool /** i18n **/) const
        {
            QString windAsString = QString("Wind: ");
            if (m_directionVariable) windAsString += "variable ";
            else windAsString += QStringLiteral("%1 ").arg(m_directionMain.toQString());

            if (m_directionFrom != CAngle() && m_directionTo != CAngle())
            {
                windAsString += QStringLiteral("variable between %1 and %2 ").arg(m_directionFrom.toQString(), m_directionTo.toQString());
            }

            windAsString += QStringLiteral("at %2").arg(m_speed.toQString());
            if (m_gustSpeed.value() > 0.5) windAsString += QStringLiteral(" and gusts at %1").arg(m_gustSpeed.toQString());
            return windAsString;
        }

    } // namespace
} // namespace
