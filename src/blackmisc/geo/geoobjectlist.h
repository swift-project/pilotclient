/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_GEOOBJECTLIST_H
#define BLACKMISC_GEO_GEOOBJECTLIST_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/geo/coordinategeodetic.h"

#include <QList>
#include <tuple>

namespace BlackMisc
{
    namespace Geo
    {
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
            CONTAINER findWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
            {
                return this->container().findBy([&](const OBJ & geoObj)
                {
                    return calculateGreatCircleDistance(geoObj, coordinate) <= range;
                });
            }

            //! Find 0..n objects outside range of given coordinate
            //! \param coordinate other position
            //! \param range      outside range of other position
            CONTAINER findOutsideRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
            {
                return this->container().findBy([&](const OBJ & geoObj)
                {
                    return calculateGreatCircleDistance(geoObj, coordinate) > range;
                });
            }

            //! Find first in range
            OBJ findFirstWithinRangeOrDefault(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
            {
                return this->container().findFirstByOrDefault([&](const OBJ & geoObj)
                {
                    return calculateGreatCircleDistance(geoObj, coordinate) <= range;
                });
            }

            //! Elements with geodetic height (only MSL)
            CONTAINER findWithGeodeticMSLHeight() const
            {
                return this->container().findBy(&OBJ::hasMSLGeodeticHeight, true);
            }

            //! Any object in range?
            bool containsObjectInRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
            {
                return this->container().containsBy([&](const OBJ & geoObj)
                {
                    const PhysicalQuantities::CLength d = coordinate.calculateGreatCircleDistance(geoObj);
                    return d <= range;
                });
            }

            //! Any object in range?
            bool containsObjectOutsideRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
            {
                return this->container().containsBy([&](const OBJ & geoObj)
                {
                    const PhysicalQuantities::CLength d = coordinate.calculateGreatCircleDistance(geoObj);
                    return d > range;
                });
            }

            //! Any NULL position?
            bool containsNullPosition() const
            {
                return this->container().containsBy([&](const ICoordinateGeodetic & geoObj)
                {
                    return geoObj.isNull();
                });
            }

            //! Any NULL position or NULL height
            bool containsNullPositionOrHeight() const
            {
                return this->container().containsBy([&](const ICoordinateGeodetic & geoObj)
                {
                    return geoObj.isNull() || geoObj.isGeodeticHeightNull();
                });
            }

            //! Find min/max/average height
            MinMaxAverageHeight findMinMaxAverageHeight() const
            {
                MinMaxAverageHeight stats{ Aviation::CAltitude::null(), Aviation::CAltitude::null(), Aviation::CAltitude::null(), 0 };
                if (this->container().isEmpty()) { return stats; } // avoid div by zero
                int count = 0;
                double avgFt = 0;
                for (const OBJ &obj : this->container())
                {
                    if (!obj.hasMSLGeodeticHeight()) { continue; }
                    const Aviation::CAltitude alt = obj.geodeticHeight();
                    if (std::get<0>(stats).isNull() || std::get<0>(stats) > alt)
                    {
                        std::get<0>(stats) = alt; // min.
                    }
                    if (std::get<1>(stats).isNull() || std::get<1>(stats) < alt)
                    {
                        std::get<1>(stats) = alt; //max.
                    }
                    avgFt += alt.value(PhysicalQuantities::CLengthUnit::ft()); // add up
                    count++;
                }

                if (count > 0) { std::get<2>(stats) = Aviation::CAltitude(avgFt / count, Aviation::CAltitude::MeanSeaLevel, PhysicalQuantities::CLengthUnit::ft()); }
                std::get<3>(stats) = count;
                return stats;
            }

            //! Find min/max/average height
            Aviation::CAltitude findMaxHeight() const
            {
                if (this->container().isEmpty()) { return Aviation::CAltitude::null(); }
                Aviation::CAltitude max = Aviation::CAltitude::null();
                for (const OBJ &obj : this->container())
                {
                    if (!obj.hasMSLGeodeticHeight()) { continue; }
                    const Aviation::CAltitude alt = obj.geodeticHeight();
                    if (max.isNull() || alt > max) { max = alt; }
                }
                return max;
            }

            //! Remove inside range
            int removeInsideRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range)
            {
                const int size = this->container().size();
                const CONTAINER copy = this->container().findOutsideRange(coordinate, range);
                const int d = size - copy.size();
                if (d > 0) { *this = copy; }
                return d;
            }

            //! Remove outside range
            int removeOutsideRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range)
            {
                const int size = this->container().size();
                const CONTAINER copy = this->container().findWithinRange(coordinate, range);
                const int d = size - copy.size();
                if (d > 0) { *this = copy; }
                return d;
            }

            //! Remove if there is no geodetic height
            int removeWithoutGeodeticHeight()
            {
                const int size = this->container().size();
                const CONTAINER copy = this->findWithGeodeticMSLHeight();
                const int d = size - copy.size();
                if (d > 0) { *this = copy; }
                return d;
            }

            //! Find 0..n objects closest to the given coordinate.
            CONTAINER findClosest(int number, const ICoordinateGeodetic &coordinate) const
            {
                CONTAINER closest = this->container().partiallySorted(number, [&](const OBJ & a, const OBJ & b)
                {
                    return calculateEuclideanDistanceSquared(a, coordinate) < calculateEuclideanDistanceSquared(b, coordinate);
                });
                closest.truncate(number);
                return closest;
            }

            //! Find 0..n objects farthest to the given coordinate.
            CONTAINER findFarthest(int number, const ICoordinateGeodetic &coordinate) const
            {
                CONTAINER farthest = this->container().partiallySorted(number, [&](const OBJ & a, const OBJ & b)
                {
                    return calculateEuclideanDistanceSquared(a, coordinate) > calculateEuclideanDistanceSquared(b, coordinate);
                });
                farthest.truncate(number);
                return farthest;
            }

            //! Find closest within range to the given coordinate
            OBJ findClosestWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
            {
                OBJ closest;
                PhysicalQuantities::CLength distance = PhysicalQuantities::CLength::null();
                for (const OBJ &obj : this->container())
                {
                    const PhysicalQuantities::CLength d = coordinate.calculateGreatCircleDistance(obj);
                    if (d > range) { continue; }
                    if (distance.isNull() || distance > d)
                    {
                        distance = d;
                        closest = obj;
                    }
                }
                return closest;
            }

            //! Sort by distance
            void sortByEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate)
            {
                this->container().sort([&](const OBJ & a, const OBJ & b)
                {
                    return calculateEuclideanDistanceSquared(a, coordinate) < calculateEuclideanDistanceSquared(b, coordinate);
                });
            }

            //! Sorted by distance
            CONTAINER sortedByEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate) const
            {
                CONTAINER copy(this->container());
                copy.sortByEuclideanDistanceSquared(coordinate);
                return copy;
            }

        protected:
            //! Constructor
            IGeoObjectList()
            { }

            //! Container
            const CONTAINER &container() const
            {
                return static_cast<const CONTAINER &>(*this);
            }

            //! Container
            CONTAINER &container()
            {
                return static_cast<CONTAINER &>(*this);
            }
        };

        //! List of objects with geo coordinates.
        template<class OBJ, class CONTAINER>
        class IGeoObjectWithRelativePositionList : public IGeoObjectList<OBJ, CONTAINER>
        {
        public:
            //! Calculate distances, then sort by range
            void sortByRange(const ICoordinateGeodetic &position, bool updateValues)
            {
                if (updateValues)
                {
                    this->calculcateAndUpdateRelativeDistanceAndBearing(position);
                }
                this->container().sort([&](const OBJ & a, const OBJ & b) { return a.getRelativeDistance() < b.getRelativeDistance(); });
            }

            //! If distance is already set, just sort container
            //! \remark requires calculcateAndUpdateRelativeDistanceAndBearing
            void sortByDistanceToReferencePosition()
            {
                this->container().sort([&](const OBJ & a, const OBJ & b) { return a.getRelativeDistance() < b.getRelativeDistance(); });
            }

            //! Sort the first n closest objects
            void partiallySortByDistanceToReferencePosition(int number)
            {
                this->container().partiallySort(number, [&](const OBJ & a, const OBJ & b) { return a.getRelativeDistance() < b.getRelativeDistance(); });
            }

            //! Get n closest objects
            CONTAINER getClosestObjects(int number) const
            {
                if (number < 1) { return CONTAINER(); }
                if (this->container().size() >= number) { return (this->container()); }
                CONTAINER closest(this->container());
                closest.partiallySortByDistanceToReferencePosition(number);
                Q_ASSERT_X(closest.size() <= number, Q_FUNC_INFO, "size exceeded");
                return closest;
            }

            //! Calculate distances, remove if outside range
            void removeIfOutsideRange(const ICoordinateGeodetic &position, const PhysicalQuantities::CLength &maxDistance, bool updateValues)
            {
                this->container().removeIf([&](OBJ & geoObj)
                {
                    return updateValues ?
                        geoObj.calculcateAndUpdateRelativeDistanceAndBearing(position) > maxDistance :
                    geoObj.calculateGreatCircleDistance(position) > maxDistance;
                });
            }

            //! Calculate distances
            void calculcateAndUpdateRelativeDistanceAndBearing(const ICoordinateGeodetic &position)
            {
                for (OBJ &geoObj : this->container())
                {
                    geoObj.calculcateAndUpdateRelativeDistanceAndBearing(position);
                }
            }

        protected:
            //! Constructor
            IGeoObjectWithRelativePositionList()
            { }
        };
    } //namespace
} // namespace

#endif //guard
