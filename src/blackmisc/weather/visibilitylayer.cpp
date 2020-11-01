/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/weather/visibilitylayer.h"
#include "blackmisc/propertyindexref.h"
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

        QVariant CVisibilityLayer::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBase:
                return QVariant::fromValue(m_base);
            case IndexTop:
                return QVariant::fromValue(m_top);
            case IndexVisibility:
                return QVariant::fromValue(m_visibility);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CVisibilityLayer::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CVisibilityLayer>(); return; }
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
