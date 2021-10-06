/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftpartslist.h"

#include <tuple>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftParts, CAircraftPartsList)

namespace BlackMisc::Aviation
{
    CAircraftPartsList::CAircraftPartsList() { }

    CAircraftPartsList::CAircraftPartsList(const CSequence<CAircraftParts> &other) :
        CSequence<CAircraftParts>(other)
    { }

    int CAircraftPartsList::setOnGround(bool onGround)
    {
        int c = 0;
        for (CAircraftParts &p : *this)
        {
            if (p.isOnGround() == onGround) { continue; }
            p.setOnGround(onGround);
            c++;
        }
        return c;
    }
} // namespace
