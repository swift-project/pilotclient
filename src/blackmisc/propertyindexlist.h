/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEXLIST_H
#define BLACKMISC_PROPERTYINDEXLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/sequence.h"

#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CPropertyIndex, CPropertyIndexList)

namespace BlackMisc
{
    //! Value object encapsulating a list of property indexes.
    class BLACKMISC_EXPORT CPropertyIndexList :
        public CSequence<CPropertyIndex>,
        public BlackMisc::Mixin::MetaType<CPropertyIndexList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CPropertyIndexList)
        using CSequence::CSequence;

        //! Default constructor.
        CPropertyIndexList();

        //! Construct from a base class object.
        CPropertyIndexList(const CSequence<CPropertyIndex> &other);

        //! List without front element, or empty list if not applicable
        Q_REQUIRED_RESULT CPropertyIndexList copyFrontRemoved() const;
    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndexList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CPropertyIndex>)

#endif //guard
