/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/propertyindexlist.h"

#include <QString>
#include <tuple>

namespace BlackMisc
{
    CPropertyIndexList::CPropertyIndexList() { }

    CPropertyIndexList::CPropertyIndexList(const CSequence<CPropertyIndex> &other) :
        CSequence<CPropertyIndex>(other)
    { }

} // namespace
