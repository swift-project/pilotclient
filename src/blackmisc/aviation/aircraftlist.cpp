/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindexallclasses.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Aviation
    {

        CAircraftList::CAircraftList() { }

        CAircraftList::CAircraftList(const CSequence<CAircraft> &other) :
            CSequence<CAircraft>(other)
        { }

        void CAircraftList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraft>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraft>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraft>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraft>>();
            qRegisterMetaType<CAircraftList>();
            qDBusRegisterMetaType<CAircraftList>();
            registerMetaValueType<CAircraftList>();
        }

        CUserList CAircraftList::getPilots() const
        {
            return this->findBy(Predicates::MemberValid(&CAircraft::getPilot)).transform(Predicates::MemberTransform(&CAircraft::getPilot));
        }

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
