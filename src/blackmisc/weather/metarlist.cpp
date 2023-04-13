/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
