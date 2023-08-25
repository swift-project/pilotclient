// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/propertyindexlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CPropertyIndex, CPropertyIndexList)

namespace BlackMisc
{
    CPropertyIndexList::CPropertyIndexList() {}

    CPropertyIndexList::CPropertyIndexList(const CSequence<CPropertyIndex> &other) : CSequence<CPropertyIndex>(other)
    {}

    CPropertyIndexList CPropertyIndexList::copyFrontRemoved() const
    {
        if (this->size() < 2) { return CPropertyIndexList(); }
        CPropertyIndexList copy(*this);
        copy.pop_front();
        return copy;
    }

} // namespace
