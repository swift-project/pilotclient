/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/geoobjectlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Geo
    {
        template <class OBJ, class CONTAINER>
        IGeoObjectList<OBJ, CONTAINER>::IGeoObjectList()
        { }

        template <class OBJ, class CONTAINER>
        IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::IGeoObjectWithRelativePositionList()
        { }

        template <class OBJ, class CONTAINER>
        CONTAINER IGeoObjectList<OBJ, CONTAINER>::findWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
        {
            return this->container().findBy([&](const OBJ & geoObj)
            {
                return calculateGreatCircleDistance(geoObj, coordinate) <= range;
            });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::calculcateDistanceAndBearingToPlane(const ICoordinateGeodetic &position)
        {
            for (OBJ &geoObj : this->container())
            {
                geoObj.calculcateDistanceAndBearingToOwnAircraft(position);
            }
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::removeIfOutsideRange(const Geo::ICoordinateGeodetic &position, const CLength &maxDistance, bool updateValues)
        {
            this->container().removeIf([ & ](OBJ & geoObj)
            {
                return geoObj.calculcateDistanceAndBearingToOwnAircraft(position, updateValues) > maxDistance;
            });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::sortByRange(const BlackMisc::Geo::ICoordinateGeodetic &position, bool updateValues)
        {
            if (updateValues)
            {
                this->calculcateDistanceAndBearingToPlane(position);
            }
            this->container().sort([ & ](const OBJ & a, const OBJ & b) { return a.getDistanceToOwnAircraft() < b.getDistanceToOwnAircraft(); });
        }


        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class IGeoObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class IGeoObjectList<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
        template class IGeoObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class IGeoObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;

        template class IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
        template class IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class IGeoObjectWithRelativePositionList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;

    } // namespace
} // namespace

