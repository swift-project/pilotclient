/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/propertyindexlist.h"

#include <QString>
#include <tuple>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CPropertyIndex, CPropertyIndexList)

namespace BlackMisc
{
    CPropertyIndexList::CPropertyIndexList() { }

    CPropertyIndexList::CPropertyIndexList(const CSequence<CPropertyIndex> &other) :
        CSequence<CPropertyIndex>(other)
    { }

    CPropertyIndexList CPropertyIndexList::copyFrontRemoved() const
    {
        if (this->size() < 2) { return CPropertyIndexList(); }
        CPropertyIndexList copy(*this);
        copy.pop_front();
        return copy;
    }

} // namespace
