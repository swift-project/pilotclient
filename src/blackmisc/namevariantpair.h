/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NAMEVARIANTPAIR_H
#define BLACKMISC_NAMEVARIANTPAIR_H

#include "blackmiscexport.h"
#include "variant.h"
#include "valueobject.h"
#include "icon.h"

namespace BlackMisc
{
    //! Value / variant pair
    class BLACKMISC_EXPORT CNameVariantPair : public CValueObject<CNameVariantPair>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = BlackMisc::CPropertyIndex::GlobalIndexCNameVariantPair,
            IndexVariant
        };

        //! Default constructor.
        CNameVariantPair() = default;

        //! Constructor.
        CNameVariantPair(const QString &name, const CVariant &variant, const CIcon &icon = CIcon());

        //! Get name.
        const QString &getName() const { return m_name; }

        //! Get variant.
        CVariant getVariant() const { return m_variant; }

        //! Set name.
        void setName(const QString &name) { this->m_name = name; }

        //! Name available?
        bool hasName() const { return !this->m_name.isEmpty(); }

        //! Set variant.
        void setVariant(const CVariant &variant) { m_variant = variant; }

        //! \copydoc BlackMisc::Mixin::Icon::toIcon()
        BlackMisc::CIcon toIcon() const;

        //! Has icon
        bool hasIcon() const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString  m_name;
        CVariant m_variant;
        CIcon    m_icon; //!< optional icon

        BLACK_METACLASS(CNameVariantPair,
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(variant),
            BLACK_METAMEMBER(icon)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CNameVariantPair)

#endif // guard
