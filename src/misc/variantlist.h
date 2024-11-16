// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_VARIANTLIST_H
#define SWIFT_MISC_VARIANTLIST_H

#include <iterator>

#include <QMetaType>
#include <QVariantList>

#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc, CVariant, CVariantList)

namespace swift::misc
{
    /*!
     * Value object encapsulating a list of variants.
     *
     * A CVariant containing any registered sequential container type can be converted to a CVariantList.
     */
    class SWIFT_MISC_EXPORT CVariantList : public CSequence<CVariant>, public swift::misc::mixin::MetaType<CVariantList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CVariantList)
        using CSequence::CSequence;

        //! Default constructor.
        CVariantList() {}

        //! Construct from a base class object.
        CVariantList(const CSequence &other);

        //! Construct from a QVariantList.
        CVariantList(const QVariantList &other);

        //! Construct from a moved QVariantList.
        CVariantList(QVariantList &&other);

        //! Convert to a sequence type by converting all elements.
        template <typename T>
        T to() const
        {
            return CVariant::from(*this).template to<T>();
        }

        //! Convert from a sequence type by converting all elements.
        template <typename T>
        static CVariantList from(const T &list)
        {
            return CVariant::from(list).template to<CVariantList>();
        }

        //! \copydoc swift::misc::CValueObject::registerMetadata
        static void registerMetadata();

        //! True if any element of the list matches the given event.
        bool matches(const CVariant &event) const;
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CVariantList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CVariant>)

#endif
