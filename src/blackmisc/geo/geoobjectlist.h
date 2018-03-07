/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_GEOOBJECTLIST_H
#define BLACKMISC_GEO_GEOOBJECTLIST_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"

#include <QList>
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAtcStation;
        class CAtcStationList;
        class CAirport;
        class CAirportList;
        class CAircraftSituation;
        class CAircraftSituationList;
    }

    namespace Simulation
    {
        class CSimulatedAircraft;
        class CSimulatedAircraftList;

        namespace XPlane
        {
            class CNavDataReference;
            class CNavDataReferenceList;
        }
    }

    namespace Geo
    {
        class ICoordinateGeodetic;
        class CCoordinateGeodetic;
        class CCoordinateGeodeticList;

        //! List of objects with geo coordinates.
        template<class OBJ, class CONTAINER>
        class IGeoObjectList
        {
        public:
            //! For statistics
            using MinMaxAverageHeight = std::tuple<Aviation::CAltitude, Aviation::CAltitude, Aviation::CAltitude, int>;

            //! Find 0..n objects within range of given coordinate
            //! \param coordinate other position
            //! \param range      within range of other position
            CONTAINER findWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const;

            //! Elements with geodetic height (only MSL)
            CONTAINER findWithGeodeticMSLHeight() const;

            //! Any object in range
            bool containsObjectInRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const;

            //! Find min/max/average height
            MinMaxAverageHeight findMinMaxAverageHeight() const;

            //! Remove outside range
            int removeOutsideRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range);

            //! Remove if there is no geodetic height
            int removeWithoutGeodeticHeight();

            //! Find 0..n objects closest to the given coordinate.
            CONTAINER findClosest(int number, const ICoordinateGeodetic &coordinate) const;

            //! Find closest within range to the given coordinate
            OBJ findClosestWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const;

            //! Sort by distance
            void sortByEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate);

            //! Sorted by distance
            CONTAINER sortedByEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate);

        protected:
            //! Constructor
            IGeoObjectList();

            //! Container
            const CONTAINER &container() const;

            //! Container
            CONTAINER &container();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectList<BlackMisc::Geo::CCoordinateGeodetic, BlackMisc::Geo::CCoordinateGeodeticList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectList<BlackMisc::Simulation::XPlane::CNavDataReference, BlackMisc::Simulation::XPlane::CNavDataReferenceList>;
        //! \endcond

        //! List of objects with geo coordinates.
        template<class OBJ, class CONTAINER>
        class IGeoObjectWithRelativePositionList : public IGeoObjectList<OBJ, CONTAINER>
        {
        public:
            //! Calculate distances, then sort by range
            void sortByRange(const ICoordinateGeodetic &position, bool updateValues);

            //! If distance is already set, just sort
            void sortByDistanceToOwnAircraft();

            //! Sort the first n closest objects
            void partiallySortByDistanceToOwnAircraft(int number);

            //! Get n closest objects
            CONTAINER getClosestObjects(int number) const;

            //! Calculate distances, remove if outside range
            void removeIfOutsideRange(const ICoordinateGeodetic &position, const PhysicalQuantities::CLength &maxDistance, bool updateValues);

            //! Calculate distances
            void calculcateAndUpdateRelativeDistanceAndBearing(const ICoordinateGeodetic &position);

        protected:
            //! Constructor
            IGeoObjectWithRelativePositionList();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        //! \endcond

    } //namespace
} // namespace

#endif //guard
