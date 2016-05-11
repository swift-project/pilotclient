/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/xplane/navdatareference.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Geo
    {
        template <class OBJ, class CONTAINER>
        IGeoObjectList<OBJ, CONTAINER>::IGeoObjectList()
        { }

        template <class OBJ, class CONTAINER>
        const CONTAINER &IGeoObjectList<OBJ, CONTAINER>::container() const
        {
            return static_cast<const CONTAINER &>(*this);
        }

        template <class OBJ, class CONTAINER>
        CONTAINER &IGeoObjectList<OBJ, CONTAINER>::container()
        {
            return static_cast<CONTAINER &>(*this);
        }

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
        CONTAINER IGeoObjectList<OBJ, CONTAINER>::findClosest(int number, const ICoordinateGeodetic &coordinate) const
        {
            CONTAINER closest = this->container().partiallySorted(number, [ & ](const OBJ & a, const OBJ & b)
            {
                return calculateEuclideanDistanceSquared(a, coordinate) < calculateEuclideanDistanceSquared(b, coordinate);
            });
            closest.truncate(number);
            return closest;
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::calculcateDistanceAndBearingToPosition(const ICoordinateGeodetic &position)
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
                this->calculcateDistanceAndBearingToPosition(position);
            }
            this->container().sort([ & ](const OBJ & a, const OBJ & b) { return a.getDistanceToOwnAircraft() < b.getDistanceToOwnAircraft(); });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::sortByDistanceToOwnAircraft()
        {
            this->container().sort([ & ](const OBJ & a, const OBJ & b) { return a.getDistanceToOwnAircraft() < b.getDistanceToOwnAircraft(); });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::partiallySortByDistanceToOwnAircraft(int number)
        {
            this->container().partiallySort(number, [ & ](const OBJ & a, const OBJ & b) { return a.getDistanceToOwnAircraft() < b.getDistanceToOwnAircraft(); });
        }

        template <class OBJ, class CONTAINER>
        CONTAINER IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::getClosestObjects(int number) const
        {
            if (number < 1) { return CONTAINER(); }
            if (this->container().size() >= number) { return (this->container()); }
            CONTAINER closest(this->container());
            closest.partiallySortByDistanceToOwnAircraft(number);
            closest.truncate(number);
            return closest;
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class IGeoObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class IGeoObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class IGeoObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        template class IGeoObjectList<BlackMisc::Simulation::XPlane::CNavDataReference, BlackMisc::Simulation::XPlane::CNavDataReferenceList>;

        template class IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class IGeoObjectWithRelativePositionList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;

    } // namespace
} // namespace

