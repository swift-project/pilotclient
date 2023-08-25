// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/weather/metarlist.h"

#include <tuple>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CMetar, CMetarList)

namespace BlackMisc::Weather
{
    CMetarList::CMetarList(const CSequence<CMetar> &other) : CSequence<CMetar>(other)
    {}

    CMetar CMetarList::getMetarForAirport(const Aviation::CAirportIcaoCode &icao) const
    {
        return this->findFirstByOrDefault(&CMetar::getAirportIcaoCode, icao);
    }

} // namespace
