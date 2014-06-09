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
         * Find by callsign
         */
        CAircraft CAircraftList::findFirstByCallsign(const CCallsign &callsign, const CAircraft &ifNotFound) const
        {
            CAircraftList aircrafts = this->findByCallsign(callsign);
            if (aircrafts.isEmpty()) return ifNotFound;
            return aircrafts.front();
        }

        /*
         * All pilots
         */
        CUserList CAircraftList::getPilots() const
        {
            CUserList users;
            for (auto i = this->begin(); i != this->end(); ++i)
            {
                CAircraft aircraft = *i;
                if (aircraft.getPilot().isValid()) users.push_back(aircraft.getPilot());
            }
            return users;
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
        bool CAircraftList::updateWithVatsimDataFileData(CAircraft &aircraftToBeUpdated) const
        {
            if (this->isEmpty()) return false;
            if (aircraftToBeUpdated.hasValidRealName() && aircraftToBeUpdated.hasValidId() && aircraftToBeUpdated.hasValidAircraftAndAirlineDesignator()) return false;

            CAircraft currentDataFileAircraft = this->findFirstByCallsign(aircraftToBeUpdated.getCallsign());
            if (currentDataFileAircraft.getCallsign().isEmpty()) return false;

            CUser user = aircraftToBeUpdated.getPilot();
            if (!aircraftToBeUpdated.hasValidRealName()) user.setRealName(currentDataFileAircraft.getPilotRealname());
            if (!aircraftToBeUpdated.hasValidId()) user.setId(currentDataFileAircraft.getPilotId());
            aircraftToBeUpdated.setPilot(user);

            CAircraftIcao icao = aircraftToBeUpdated.getIcaoInfo();
            CAircraftIcao dataFileIcao = currentDataFileAircraft.getIcaoInfo();
            if (!icao.hasAircraftDesignator()) icao.setAircraftDesignator(dataFileIcao.getAircraftDesignator());
            if (!icao.hasAirlineDesignator()) icao.setAirlineDesignator(dataFileIcao.getAirlineDesignator());
            if (!icao.hasAircraftCombinedType()) icao.setAircraftCombinedType(dataFileIcao.getAircraftCombinedType());
            aircraftToBeUpdated.setIcaoInfo(icao);
            return true;
        }
    } // namespace
} // namespace
