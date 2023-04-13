/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANTLIST_H
#define BLACKMISC_VARIANTLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include <QVariantList>
#include <QMetaType>
#include <iterator>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CVariant, CVariantList)

namespace BlackMisc
{
    /*!
     * Value object encapsulating a list of variants.
     *
     * A CVariant containing any registered sequential container type can be converted to a CVariantList.
     */
    class BLACKMISC_EXPORT CVariantList :
        public CSequence<CVariant>,
        public BlackMisc::Mixin::MetaType<CVariantList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CVariantList)
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

        //! \copydoc BlackMisc::CValueObject::registerMetadata
        static void registerMetadata();

        //! True if any element of the list matches the given event.
        bool matches(const CVariant &event) const;
    };
}

Q_DECLARE_METATYPE(BlackMisc::CVariantList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CVariant>)

#endif
