/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/ecosystemlist.h"

namespace BlackMisc::Network
{
    CEcosystemList::CEcosystemList() { }

    CEcosystemList::CEcosystemList(const CSequence &other) : CSequence<CEcosystem>(other)
    { }

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
