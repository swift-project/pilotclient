// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/namevariantpair.h"

#include "misc/propertyindexvariantmap.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CNameVariantPair)

namespace swift::misc
{
    CNameVariantPair::CNameVariantPair(const QString &name, const CVariant &variant, const CIcon &icon)
        : m_name(name), m_variant(variant), m_icon(icon)
    {}

    CIcons::IconIndex CNameVariantPair::toIcon() const { return m_icon.getIndex(); }

    bool CNameVariantPair::hasIcon() const { return this->m_icon.isSet(); }

    QString CNameVariantPair::convertToQString(bool i18n) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_variant.toQString(i18n));
        return s;
    }

    QVariant CNameVariantPair::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: return QVariant(this->m_name);
        case IndexVariant: return this->m_variant;
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CNameVariantPair::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CNameVariantPair>();
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();

        // special case, handle icon and allow to set it
        // doing this in the switch gives gcc warning as IndexIcon is no member of ColumnIndex
        if (static_cast<int>(i) == static_cast<int>(IndexIcon))
        {
            if (static_cast<QMetaType::Type>(variant.typeId()) == QMetaType::Int)
            {
                CIcons::IconIndex iconIndex = variant.value<CIcons::IconIndex>();
                this->m_icon = CIcon::iconByIndex(iconIndex);
            }
            else { this->m_icon = variant.value<swift::misc::CIcon>(); }
            return;
        }

        // properties
        switch (i)
        {
        case IndexName: this->setName(variant.value<QString>()); break;
        case IndexVariant: this->m_variant = variant; break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::misc
