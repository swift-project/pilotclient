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

#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/geo/coordinategeodetic.h"

namespace BlackMisc
{
    namespace Aviation
    {
        class CAtcStation;
        class CAtcStationList;
        class CAirport;
        class CAirportList;
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
        //! List of objects with geo coordinates.
        template<class OBJ, class CONTAINER>
        class IGeoObjectList
        {
        public:
            //! Find 0..n objects within range of given coordinate
            //! \param coordinate    other position
            //! \param range         within range of other position
            //! \return
            CONTAINER findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            //! Find 0..n objects closest to the given coordinate.
            CONTAINER findClosest(int number, const BlackMisc::Geo::ICoordinateGeodetic &coordinate) const;

        protected:
            //! Constructor
            IGeoObjectList();

            //! Container
            const CONTAINER &container() const;

            //! Container
            CONTAINER &container();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectList<BlackMisc::Simulation::XPlane::CNavDataReference, BlackMisc::Simulation::XPlane::CNavDataReferenceList>;
        //! \endcond

        //! List of objects with geo coordinates.
        template<class OBJ, class CONTAINER>
        class IGeoObjectWithRelativePositionList : public IGeoObjectList<OBJ, CONTAINER>
        {
        public:
            //! Calculate distances, then sort by range
            void sortByRange(const BlackMisc::Geo::ICoordinateGeodetic &position, bool updateValues);

            //! If distance is already set, just sort
            void sortByDistanceToOwnAircraft();

            //! Sort the first n closest objects
            void partiallySortByDistanceToOwnAircraft(int number);

            //! Get n closest objects
            CONTAINER getClosestObjects(int number) const;

            //! Calculate distances, remove if outside range
            void removeIfOutsideRange(const BlackMisc::Geo::ICoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &maxDistance, bool updateValues);

            //! Calculate distances
            void calculcateDistanceAndBearingToPosition(const BlackMisc::Geo::ICoordinateGeodetic &position);

        protected:
            //! Constructor
            IGeoObjectWithRelativePositionList();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE IGeoObjectWithRelativePositionList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        //! \endcond

    } //namespace
} // namespace

#endif //guard
