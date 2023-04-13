/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/identifierset.h"

BLACK_DEFINE_COLLECTION_MIXINS(BlackMisc, CIdentifier, CIdentifierSet)

namespace BlackMisc
{
    CIdentifierSet::CIdentifierSet() {}

    CIdentifierSet::CIdentifierSet(const CCollection<CIdentifier> &other) : CCollection<CIdentifier>(other)
    {
        // void
    }

    bool CIdentifierSet::containsAnyNotIn(const CIdentifierSet &other) const
    {
        return containsBy([&other](const CIdentifier &id) { return !other.contains(id); });
    }

    CIdentifierSet CIdentifierSet::getMachinesUnique() const
    {
        CIdentifierSet il;
        for (const CIdentifier &identifier : *this)
        {
            const bool contained = il.containsBy([=](const CIdentifier &ident) {
                return identifier.hasSameMachineName(ident);
            });
            if (!contained) { il.push_back(identifier); }
        }
        return il;
    }

    QStringList CIdentifierSet::getMachineNames(bool unique, bool sort) const
    {
        QStringList codes = this->transform(Predicates::MemberTransform(&CIdentifier::getMachineName));
        if (sort) { codes.sort(); }
        if (unique) { codes.removeDuplicates(); }
        return codes;
    }
} // namespace
