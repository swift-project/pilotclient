/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/weather/visibilitylayer.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {

        CVisibilityLayer::CVisibilityLayer(const BlackMisc::Aviation::CAltitude &base,
                                           const BlackMisc::Aviation::CAltitude &top,
                                           const PhysicalQuantities::CLength &visibility) :
            m_base(base), m_top(top), m_visibility(visibility)
        { }

        CVariant CVisibilityLayer::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBase:
                return CVariant::fromValue(m_base);
            case IndexTop:
                return CVariant::fromValue(m_top);
            case IndexVisibility:
                return CVariant::fromValue(m_visibility);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CVisibilityLayer::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CVisibilityLayer>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBase:
                setBase(variant.value<CAltitude>());
                break;
            case IndexTop:
                setTop(variant.value<CAltitude>());
                break;
            case IndexVisibility:
                setVisibility(variant.value<CLength>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CVisibilityLayer::convertToQString(bool /** i18n **/) const
        {
            QString visibilityAsString = QStringLiteral("Visibility: %1 from %2 to %3");
            visibilityAsString = visibilityAsString.arg(m_visibility.toQString(), m_base.toQString(), m_top.toQString());
            return visibilityAsString;
        }

    } // namespace
} // namespace
