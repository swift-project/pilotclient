// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/identifierlist.h"

#include <QStringList>

#include "misc/mixin/mixincompare.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc, CIdentifier, CIdentifierList)

namespace swift::misc
{
    CIdentifierList::CIdentifierList() {}

    CIdentifierList::CIdentifierList(const CSequence<CIdentifier> &other) : CSequence<CIdentifier>(other) {}

    bool CIdentifierList::containsAnyNotIn(const CIdentifierList &other) const
    {
        return containsBy([&other](const CIdentifier &id) { return !other.contains(id); });
    }

    CIdentifierList CIdentifierList::getMachinesUnique() const
    {
        CIdentifierList il;
        for (const CIdentifier &identifier : *this)
        {
            bool contained =
                il.containsBy([=](const CIdentifier &ident) { return identifier.hasSameMachineName(ident); });
            if (!contained) { il.push_back(identifier); }
        }
        return il;
    }

    QStringList CIdentifierList::getMachineNames(bool unique, bool sort) const
    {
        const auto view =
            *this | std::views::transform([](const CIdentifier &identifier) { return identifier.getMachineName(); });
        QStringList codes(view.begin(), view.end());
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
} // namespace swift::misc
