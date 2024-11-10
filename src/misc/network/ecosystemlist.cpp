// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/ecosystemlist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CEcosystem, CEcosystemList)

namespace swift::misc::network
{
    CEcosystemList::CEcosystemList() {}

    CEcosystemList::CEcosystemList(const CSequence &other) : CSequence<CEcosystem>(other)
    {}

    QStringList CEcosystemList::allSystemStrings() const
    {
        QStringList l;
        for (const CEcosystem &e : *this)
        {
            l.push_back(e.getSystemString());
        }
        return l;
    }

    const CEcosystemList &CEcosystemList::allKnownSystems()
    {
        static const CEcosystemList s({ CEcosystem::vatsim(), CEcosystem::swift(), CEcosystem::swiftTest(), CEcosystem::privateFsd() });
        return s;
    }
} // namespace
