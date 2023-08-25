// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NAMEVARIANTPAIR_H
#define BLACKMISC_NAMEVARIANTPAIR_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc, CNameVariantPair)

namespace BlackMisc
{
    //! Value / variant pair
    class BLACKMISC_EXPORT CNameVariantPair : public CValueObject<CNameVariantPair>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = BlackMisc::CPropertyIndexRef::GlobalIndexCNameVariantPair,
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
        BlackMisc::CIcons::IconIndex toIcon() const;

        //! Has icon
        bool hasIcon() const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_name;
        CVariant m_variant;
        CIcon m_icon; //!< optional icon

        BLACK_METACLASS(
            CNameVariantPair,
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(variant),
            BLACK_METAMEMBER(icon)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CNameVariantPair)

#endif // guard
