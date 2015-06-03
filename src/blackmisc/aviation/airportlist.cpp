/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/predicates.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {

        CAirportList::CAirportList() { }

        CAirportList::CAirportList(const CSequence<CAirport> &other) :
            CSequence<CAirport>(other)
        { }

        CAirportList CAirportList::findByIcao(const CAirportIcaoCode &icao) const
        {
            return this->findBy(&CAirport::getIcao, icao);
        }

        void CAirportList::replaceOrAddByIcao(const CAirport &addedOrReplacedAirport)
        {
            if (!addedOrReplacedAirport.hasValidIcaoCode()) return; // ignore invalid airport
            this->replaceOrAdd(&CAirport::getIcao, addedOrReplacedAirport.getIcao(), addedOrReplacedAirport);
        }

        CAirport CAirportList::findFirstByIcao(const CAirportIcaoCode &icao, const CAirport &ifNotFound) const
        {
            return this->findFirstByOrDefault(&CAirport::getIcao, icao, ifNotFound);
        }

    } // namespace
} // namespace
