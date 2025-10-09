// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftpartslist.h"

#include "misc/aviation/aircraftparts.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftParts, CAircraftPartsList)

namespace swift::misc::aviation
{
    CAircraftPartsList::CAircraftPartsList(const CSequence<CAircraftParts> &other) : CSequence<CAircraftParts>(other) {}

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
} // namespace swift::misc::aviation
