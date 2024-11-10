// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/matchingscriptmisc.h"

namespace swift::misc::simulation
{
    //! Enum as string
    const QString &msToString(MatchingScript ms)
    {
        static const QString r("Reverse lookup");
        static const QString m("Matching stage");
        static const QString d("Unknown");
        switch (ms)
        {
        case ReverseLookup: return r;
        case MatchingStage: return m;
        default: break;
        }
        return d;
    }
} // namespace
