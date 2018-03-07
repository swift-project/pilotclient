/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/geo/geo.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/xplane/navdatareference.h"

using namespace BlackMisc::Aviation;
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

        template<class OBJ, class CONTAINER>
        CONTAINER IGeoObjectList<OBJ, CONTAINER>::findWithGeodeticMSLHeight() const
        {
            return this->container().findBy(&OBJ::hasMSLGeodeticHeight, true);
        }

        template<class OBJ, class CONTAINER>
        bool IGeoObjectList<OBJ, CONTAINER>::containsObjectInRange(const ICoordinateGeodetic &coordinate, const CLength &range) const
        {
            return this->container().containsBy([&](const OBJ & geoObj)
            {
                const CLength d = coordinate.calculateGreatCircleDistance(geoObj);
                return d <= range;
            });
        }

        template<class OBJ, class CONTAINER>
        typename IGeoObjectList<OBJ, CONTAINER>::MinMaxAverageHeight IGeoObjectList<OBJ, CONTAINER>::findMinMaxAverageHeight() const
        {
            MinMaxAverageHeight stats{ CAltitude::null(), CAltitude::null(), CAltitude::null(), 0 };
            int c = 0;
            double avgFt = 0;
            for (const OBJ &obj : this->container())
            {
                if (!obj.hasMSLGeodeticHeight()) { continue; }
                const CAltitude alt = obj.geodeticHeight();
                if (std::get<0>(stats).isNull() || std::get<0>(stats) > alt)
                {
                    std::get<0>(stats) = alt;
                }
                if (std::get<1>(stats).isNull() || std::get<1>(stats) < alt)
                {
                    std::get<1>(stats) = alt;
                }
                avgFt += alt.value(CLengthUnit::ft());
            }

            std::get<2>(stats) = CAltitude(avgFt / c, CAltitude::MeanSeaLevel, CLengthUnit::ft());
            std::get<3>(stats) = c;
            return stats;
        }

        template <class OBJ, class CONTAINER>
        int IGeoObjectList<OBJ, CONTAINER>::removeOutsideRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range)
        {
            const int size = this->container().size();
            const CONTAINER copy = this->container().findWithinRange(coordinate, range);
            const int d = size - copy.size();
            if (d > 0) { *this = copy; }
            return d;
        }

        template<class OBJ, class CONTAINER>
        int IGeoObjectList<OBJ, CONTAINER>::removeWithoutGeodeticHeight()
        {
            const int size = this->container().size();
            const CONTAINER copy = this->findWithGeodeticMSLHeight();
            const int d = size - copy.size();
            if (d > 0) { *this = copy; }
            return d;
        }

        template <class OBJ, class CONTAINER>
        CONTAINER IGeoObjectList<OBJ, CONTAINER>::findClosest(int number, const ICoordinateGeodetic &coordinate) const
        {
            CONTAINER closest = this->container().partiallySorted(number, [ & ](const OBJ & a, const OBJ & b)
            {
                return calculateEuclideanDistanceSquared(a, coordinate) < calculateEuclideanDistanceSquared(b, coordinate);
            });
            Q_ASSERT_X(closest.size() <= number, Q_FUNC_INFO, "size exceeded");
            return closest;
        }

        template<class OBJ, class CONTAINER>
        OBJ IGeoObjectList<OBJ, CONTAINER>::findClosestWithinRange(const ICoordinateGeodetic &coordinate, const CLength &range) const
        {
            OBJ closest;
            CLength distance = CLength::null();
            for (const OBJ &obj : this->container())
            {
                const CLength d = coordinate.calculateGreatCircleDistance(obj);
                if (d > range) { continue; }
                if (distance.isNull() || distance > d)
                {
                    distance = d;
                    closest = obj;
                }
            }
            return closest;
        }

        template<class OBJ, class CONTAINER>
        void IGeoObjectList<OBJ, CONTAINER>::sortByEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate)
        {
            this->container().sort([ & ](const OBJ & a, const OBJ & b)
            {
                return calculateEuclideanDistanceSquared(a, coordinate) < calculateEuclideanDistanceSquared(b, coordinate);
            });
        }

        template<class OBJ, class CONTAINER>
        CONTAINER IGeoObjectList<OBJ, CONTAINER>::sortedByEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate)
        {
            CONTAINER copy(this->container());
            copy.sortByEuclideanDistanceSquared(coordinate);
            return copy;
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::calculcateAndUpdateRelativeDistanceAndBearing(const ICoordinateGeodetic &position)
        {
            for (OBJ &geoObj : this->container())
            {
                geoObj.calculcateAndUpdateRelativeDistanceAndBearing(position);
            }
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::removeIfOutsideRange(const Geo::ICoordinateGeodetic &position, const CLength &maxDistance, bool updateValues)
        {
            this->container().removeIf([ & ](OBJ & geoObj)
            {
                return updateValues ?
                       geoObj.calculcateAndUpdateRelativeDistanceAndBearing(position) > maxDistance :
                       geoObj.calculateGreatCircleDistance(position) > maxDistance;
            });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::sortByRange(const BlackMisc::Geo::ICoordinateGeodetic &position, bool updateValues)
        {
            if (updateValues)
            {
                this->calculcateAndUpdateRelativeDistanceAndBearing(position);
            }
            this->container().sort([ & ](const OBJ & a, const OBJ & b) { return a.getRelativeDistance() < b.getRelativeDistance(); });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::sortByDistanceToOwnAircraft()
        {
            this->container().sort([ & ](const OBJ & a, const OBJ & b) { return a.getRelativeDistance() < b.getRelativeDistance(); });
        }

        template <class OBJ, class CONTAINER>
        void IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::partiallySortByDistanceToOwnAircraft(int number)
        {
            this->container().partiallySort(number, [ & ](const OBJ & a, const OBJ & b) { return a.getRelativeDistance() < b.getRelativeDistance(); });
        }

        template <class OBJ, class CONTAINER>
        CONTAINER IGeoObjectWithRelativePositionList<OBJ, CONTAINER>::getClosestObjects(int number) const
        {
            if (number < 1) { return CONTAINER(); }
            if (this->container().size() >= number) { return (this->container()); }
            CONTAINER closest(this->container());
            closest.partiallySortByDistanceToOwnAircraft(number);
            Q_ASSERT_X(closest.size() <= number, Q_FUNC_INFO, "size exceeded");
            return closest;
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectList<BlackMisc::Geo::CCoordinateGeodetic, BlackMisc::Geo::CCoordinateGeodeticList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectList<BlackMisc::Simulation::XPlane::CNavDataReference, BlackMisc::Simulation::XPlane::CNavDataReferenceList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        //! \endcond

    } // namespace
} // namespace
