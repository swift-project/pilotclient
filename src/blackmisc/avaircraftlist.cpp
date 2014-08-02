/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
         * Default constructor
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
         * Find by callsigns
         */
        CAircraftList CAircraftList::findByCallsigns(const CCallsignList &callsigns) const
        {
            return this->findBy(Predicates::MemberIsAnyOf(&CAircraft::getCallsign, callsigns));
        }

        /*
         * Find by callsign
         */
        CAircraft CAircraftList::findFirstByCallsign(const CCallsign &callsign, const CAircraft &ifNotFound) const
        {
            return this->findByCallsign(callsign).frontOrDefault(ifNotFound);
        }

        /*
         * All pilots
         */
        CUserList CAircraftList::getPilots() const
        {
            return this->findBy(Predicates::MemberValid(&CAircraft::getPilot)).transform(Predicates::MemberTransform(&CAircraft::getPilot));
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
