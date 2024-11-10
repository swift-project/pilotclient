// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/metarlist.h"

#include <tuple>

BLACK_DEFINE_SEQUENCE_MIXINS(swift::misc::weather, CMetar, CMetarList)

namespace swift::misc::weather
{
    CMetarList::CMetarList(const CSequence<CMetar> &other) : CSequence<CMetar>(other)
    {}

    CMetar CMetarList::getMetarForAirport(const aviation::CAirportIcaoCode &icao) const
    {
        return this->findFirstByOrDefault(&CMetar::getAirportIcaoCode, icao);
    }

} // namespace
