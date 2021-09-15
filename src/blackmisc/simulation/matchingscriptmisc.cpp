/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/matchingscriptmisc.h"

namespace BlackMisc::Simulation
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
