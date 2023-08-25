// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftpartslist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftParts, CAircraftPartsList)

namespace BlackMisc::Aviation
{
    CAircraftPartsList::CAircraftPartsList() {}

    CAircraftPartsList::CAircraftPartsList(const CSequence<CAircraftParts> &other) : CSequence<CAircraftParts>(other)
    {}

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
