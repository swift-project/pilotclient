// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PROPERTYINDEXLIST_H
#define SWIFT_MISC_PROPERTYINDEXLIST_H

#include <QMetaType>

#include "misc/collection.h"
#include "misc/propertyindex.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc, CPropertyIndex, CPropertyIndexList)

namespace swift::misc
{
    //! Value object encapsulating a list of property indexes.
    class SWIFT_MISC_EXPORT CPropertyIndexList :
        public CSequence<CPropertyIndex>,
        public swift::misc::mixin::MetaType<CPropertyIndexList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CPropertyIndexList)
        using CSequence::CSequence;

        //! Default constructor.
        CPropertyIndexList() = default;

        //! Construct from a base class object.
        CPropertyIndexList(const CSequence<CPropertyIndex> &other);

        //! List without front element, or empty list if not applicable
        Q_REQUIRED_RESULT CPropertyIndexList copyFrontRemoved() const;
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CPropertyIndexList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CPropertyIndex>)

#endif // SWIFT_MISC_PROPERTYINDEXLIST_H
