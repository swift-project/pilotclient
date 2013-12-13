/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avaircraftlist.h"
#include "predicates.h"
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Empty constructor
         */
        CAircraftList::CAircraftList() { }

        /*
         * Construct from base class object
         */
        CAircraftList::CAircraftList(const CSequence<CAircraft> &other) :
            CSequence<CAircraft>(other)
        { }

        /*
         * Register metadata
         */
        void CAircraftList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraft>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraft>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraft>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraft>>();
            qRegisterMetaType<CAircraftList>();
            qDBusRegisterMetaType<CAircraftList>();
        }

        /*
         * Find by callsign
         */
        CAircraftList CAircraftList::findByCallsign(const CCallsign &callsign) const
        {
            return this->findBy(&CAircraft::getCallsign, callsign);
        }

        /*
         * Aircrafts within range
         */
        CAircraftList CAircraftList::findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
        {
            return this->findBy([&](const CAircraft & aircraft)
            {
                return BlackMisc::Geo::greatCircleDistance(aircraft, coordinate) <= range;
            });
        }

    } // namespace
} // namespace
