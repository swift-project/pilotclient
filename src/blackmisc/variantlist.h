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
#include <QMetaType>

namespace BlackMisc
{
    //! Value object encapsulating a list of variants.
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
    };
}

Q_DECLARE_METATYPE(BlackMisc::CVariantList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CVariant>)

#endif
