// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
