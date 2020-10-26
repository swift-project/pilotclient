/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/namevariantpair.h"
#include "blackmisc/propertyindexvariantmap.h"
#include <tuple>

namespace BlackMisc
{
    CNameVariantPair::CNameVariantPair(const QString &name, const CVariant &variant, const CIcon &icon)
        : m_name(name), m_variant(variant), m_icon(icon)
    {  }

    CIcons::IconIndex CNameVariantPair::toIcon() const { return m_icon.getIndex(); }

    bool CNameVariantPair::hasIcon() const
    {
        return this->m_icon.isSet();
    }

    QString CNameVariantPair::convertToQString(bool i18n) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_variant.toQString(i18n));
        return s;
    }

    CVariant CNameVariantPair::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName:
            return CVariant(this->m_name);
        case IndexVariant:
            return this->m_variant;
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CNameVariantPair::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.to<CNameVariantPair>(); return; }
        ColumnIndex i = index.frontCasted<ColumnIndex>();

        // special case, handle icon and allow to set it
        // doing this in the switch gives gcc warning as IndexIcon is no member of ColumnIndex
        if (static_cast<int>(i) == static_cast<int>(IndexIcon))
        {
            if (static_cast<QMetaType::Type>(variant.type()) == QMetaType::Int)
            {
                CIcons::IconIndex iconIndex = variant.value<CIcons::IconIndex>();
                this->m_icon = CIcon::iconByIndex(iconIndex);
            }
            else
            {
                this->m_icon = variant.value<BlackMisc::CIcon>();
            }
            return;
        }

        // properties
        switch (i)
        {
        case IndexName:
            this->setName(variant.value<QString>());
            break;
        case IndexVariant:
            this->m_variant = variant;
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // namespace
