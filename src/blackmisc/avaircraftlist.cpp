/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avaircraftlist.h"
#include "nwuser.h"
#include "predicates.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

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

        /*
         * Merge with aircraft
         */
        int CAircraftList::updateFromVatsimDataFileAircraft(CAircraft &aircraftToBeUpdated) const
        {
            if (this->isEmpty()) return 0;
            if (aircraftToBeUpdated.hasValidRealName() && aircraftToBeUpdated.hasValidId()) return 0;

            int c = 0;
            for (auto i = this->begin(); i != this->end(); ++i)
            {
                CAircraft currentDataFileAircraft = *i;
                if (currentDataFileAircraft.getCallsign() != aircraftToBeUpdated.getCallsign()) continue;

                CUser user = aircraftToBeUpdated.getPilot();
                if (!aircraftToBeUpdated.hasValidRealName()) user.setRealName(currentDataFileAircraft.getPilotRealname());
                if (!aircraftToBeUpdated.hasValidId()) user.setId(currentDataFileAircraft.getPilotId());
                aircraftToBeUpdated.setPilot(user);
                c++;
            }

            // normally 1 expected, as I should find
            // only one online station for this booking
            return c;
        }
    } // namespace
} // namespace
