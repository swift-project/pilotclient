/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "namevariantpair.h"
#include "iconlist.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{

    /*
     * Constructor
     */
    CNameVariantPair::CNameVariantPair(const QString &name, const CVariant &variant, const CIcon &icon)
        : m_name(name), m_variant(variant), m_icon(icon)
    {  }

    /*
     * Icon
     */
    const CIcon &CNameVariantPair::getIcon() const
    {
        return this->m_icon;
    }

    /*
     * Icon?
     */
    bool CNameVariantPair::hasIcon() const
    {
        return this->getIcon().isSet();
    }

    /*
     * Convert to string
     */
    QString CNameVariantPair::convertToQString(bool i18n) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_variant.toQString(i18n));
        return s;
    }

    /*
     * Property by index
     */
    CVariant CNameVariantPair::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return this->toCVariant(); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName:
            return CVariant(this->m_name);
        case IndexVariant:
            return this->m_variant;
        case IndexIcon:
            return this->m_icon.toCVariant();
        case IndexPixmap:
            return CVariant(this->m_icon.toPixmap());
        default:
            return CValueObjectStdTuple::propertyByIndex(index);
        }
    }

    /*
     * Property by index (setter)
     */
    void CNameVariantPair::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself())
        {
            this->convertFromCVariant(variant);
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName:
            this->setName(variant.value<QString>());
            break;
        case IndexVariant:
            this->m_variant = variant;
            break;
        case IndexIcon:
            if (static_cast<QMetaType::Type>(variant.type()) == QMetaType::Int)
            {
                CIcons::IconIndex index = static_cast<CIcons::IconIndex>(variant.toInt());
                this->m_icon = CIconList::iconByIndex(index);
            }
            else
            {
                this->m_icon = variant.value<BlackMisc::CIcon>();
            }
            break;
        default:
            CValueObjectStdTuple::setPropertyByIndex(variant, index);
            break;
        }
    }

} // namespace
