/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "identifierlist.h"
#include "predicates.h"

namespace BlackMisc
{
    CIdentifierList::CIdentifierList() { }

    CIdentifierList::CIdentifierList(const CSequence<BlackMisc::CIdentifier> &other) :
        CSequence<BlackMisc::CIdentifier>(other)
    { }

    bool CIdentifierList::containsAnyNotIn(const CIdentifierList &other) const
    {
        return containsBy([&other](const CIdentifier &id) { return ! other.contains(id); });
    }
} // namespace
