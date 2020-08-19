/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/identifierlist.h"
#include "blackmisc/compare.h"

#include <QString>
#include <tuple>

namespace BlackMisc
{
    CIdentifierList::CIdentifierList() { }

    CIdentifierList::CIdentifierList(const CSequence<CIdentifier> &other) :
        CSequence<CIdentifier>(other)
    { }

    bool CIdentifierList::containsAnyNotIn(const CIdentifierList &other) const
    {
        return containsBy([&other](const CIdentifier & id) { return ! other.contains(id); });
    }

    CIdentifierList CIdentifierList::getMachinesUnique() const
    {
        CIdentifierList il;
        for (const CIdentifier &identifier : *this)
        {
            bool contained = il.containsBy([ = ] (const CIdentifier &ident)
            {
                return identifier.hasSameMachineName(ident);
            });
            if (!contained) { il.push_back(identifier); }
        }
        return il;
    }

    QStringList CIdentifierList::getMachineNames(bool unique, bool sort) const
    {
        QStringList codes = this->transform(Predicates::MemberTransform(&CIdentifier::getMachineName));
        if (sort) { codes.sort(); }
        if (unique) { codes.removeDuplicates(); }
        return codes;
    }

    int CIdentifierList::removeDuplicates()
    {
        if (this->size() < 2) { return 0; }
        CIdentifierList il;
        for (const CIdentifier &identifier : *this)
        {
            if (il.contains(identifier)) continue;
            il.push_back(identifier);
        }
        const int delta = this->size() - il.size();
        if (delta == 0) { return 0; }
        *this = il;
        return delta;
    }
} // namespace
