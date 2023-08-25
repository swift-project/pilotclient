// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/ecosystemlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CEcosystem, CEcosystemList)

namespace BlackMisc::Network
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
