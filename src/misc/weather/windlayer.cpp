// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/windlayer.h"
#include "misc/pq/physicalquantity.h"
#include "misc/propertyindexref.h"

using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::weather, CWindLayer)

namespace swift::misc::weather
{

    CWindLayer::CWindLayer(const CAltitude &level, const CAngle &direction, const CSpeed &speed, const CSpeed &gustSpeed) : m_level(level), m_directionMain(direction), m_speed(speed), m_gustSpeed(gustSpeed)
    {}

    QVariant CWindLayer::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexLevel:
            return QVariant::fromValue(m_level);
        case IndexDirection:
            return QVariant::fromValue(m_directionMain);
        case IndexDirectionVariable:
            return QVariant::fromValue(m_directionVariable);
        case IndexSpeed:
            return QVariant::fromValue(m_speed);
        case IndexGustSpeed:
            return QVariant::fromValue(m_gustSpeed);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CWindLayer::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CWindLayer>();
            return;
        }
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
        if (m_directionVariable)
            windAsString += "variable ";
        else
            windAsString += QStringLiteral("%1 ").arg(m_directionMain.toQString());

        if (m_directionFrom != CAngle() && m_directionTo != CAngle())
        {
            windAsString += QStringLiteral("variable between %1 and %2 ").arg(m_directionFrom.toQString(), m_directionTo.toQString());
        }

        windAsString += QStringLiteral("at %2").arg(m_speed.toQString());
        if (m_gustSpeed.value() > 0.5) windAsString += QStringLiteral(" and gusts at %1").arg(m_gustSpeed.toQString());
        return windAsString;
    }

} // namespace
