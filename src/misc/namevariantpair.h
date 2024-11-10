// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NAMEVARIANTPAIR_H
#define SWIFT_MISC_NAMEVARIANTPAIR_H

#include "misc/swiftmiscexport.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

#include <QMetaType>
#include <QString>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CNameVariantPair)

namespace swift::misc
{
    //! Value / variant pair
    class SWIFT_MISC_EXPORT CNameVariantPair : public CValueObject<CNameVariantPair>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = swift::misc::CPropertyIndexRef::GlobalIndexCNameVariantPair,
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

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        swift::misc::CIcons::IconIndex toIcon() const;

        //! Has icon
        bool hasIcon() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_name;
        CVariant m_variant;
        CIcon m_icon; //!< optional icon

        SWIFT_METACLASS(
            CNameVariantPair,
            SWIFT_METAMEMBER(name),
            SWIFT_METAMEMBER(variant),
            SWIFT_METAMEMBER(icon));
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::CNameVariantPair)

#endif // guard
