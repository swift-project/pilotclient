/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_GEODESICGRID_H
#define BLACKMISC_GEODESICGRID_H

//! \file

#include "mathematics.h"
#include "coordinategeodetic.h"
#include "range.h"
#include "iterator.h"
#include <QMultiMap>
#include <type_traits>

namespace BlackMisc
{
    namespace Geo
    {

        // Compute the integer log2(X) of an integer X at compile time
        //! \private
        template <typename T, T X, int Count = 0>
        struct LogBase2 : std::conditional<bool (X >> 1),
                                           LogBase2<T, (X >> 1), Count + 1>,
                                           std::integral_constant<int, Count>>::type
        {};

        /*!
         * Associative container for efficiently storing and retreiving elements at points on the Earth's surface.
         * Works by dividing the surface of the Earth into 2N^2 triangular tiles of 21600/N nautical miles each.
         * Each element is inserted according to which tile it falls within, and elements can later be retrieved by
         * pointing to specific tiles.
         */
        template <int Slices, class T, class Key = qint32>
        class CGeodesicGrid
        {
        public:
            //! Iterator
            typedef typename QMultiMap<Key, T>::const_iterator const_iterator;

            //! Begin and end iterators of the underlying storage.
            //! @{
            const_iterator begin() const { return m_map.begin(); }
            const_iterator cbegin() const { return m_map.cbegin(); }
            const_iterator end() const { return m_map.end(); }
            const_iterator cend() const { return m_map.cend(); }
            //! @}

            //! Removes all elements from all tiles.
            void clear() { m_map.clear(); }

            //! Returns true if there are no elements in any tiles.
            bool isEmpty() const { return m_map.isEmpty(); }

            //! Inserts an element in the tile at the given point.
            //! \warning Angles are in radians.
            void insert(double lat, double lon, const T &value) { m_map.insert(coordinateToKey(lat, lon), value); }

            //! If T has latitude() and longitude() methods then this convenience insert() method can be used.
            void insert(const T &value) { m_map.insert(value.latitude(), value.longitude(), value); }

            //! Returns a range containing the elements in the tile at the given point.
            //! \warning Angles are in radians.
            CRange<const_iterator> inTileAt(double lat, double lon) const
            {
                Key k = coordinateToKey(lat, lon);
                return makeRange(m_map.lowerBound(k), m_map.upperBound(k));
            }

            //! Returns a range containing the elements in every tile adjacent to the one at the given point, including that one.
            //! \warning Angles are in radians.
            CRange<Iterators::ConcatIterator<const_iterator>> inAdjacentTiles(double lat, double lon, int degree = 1) const
            {
                QVector<const_iterator> its;
                for (auto k : adjacentKeys(coordinateToKey(lat, lon), degree))
                {
                    its.push_back(m_map.lowerBound(k));
                    its.push_back(m_map.upperBound(k));
                }
                Q_ASSERT(!its.isEmpty());
                return makeRange(Iterators::makeConcatIterator(its), its.back());
            }

            //! Overloaded method taking the coordinates in a different form.
            //! @{
            void insert(const CLatitude &lat, const CLongitude &lon, const T &value) { insert(lat.value(PhysicalQuantities::CAngleUnit::rad()), lon.value(PhysicalQuantities::CAngleUnit::rad()), value); }
            void insert(const ICoordinateGeodetic &coord, const T &value) { insert(coord.latitude(), coord.longitude()); }
            CRange<const_iterator> inTileAt(const CLatitude &lat, const CLongitude &lon) const { return inTileAt(lat.value(PhysicalQuantities::CAngleUnit::rad()), lon.value(PhysicalQuantities::CAngleUnit::rad())); }
            CRange<const_iterator> inTileAt(const ICoordinateGeodetic &coord) const { return inTileAt(coord.latitude(), coord.longitude()); }
            CRange<Iterators::ConcatIterator<const_iterator>> inAdjacentTiles(const CLatitude &lat, const CLongitude &lon, int degree = 1) const { return inAdjacentTiles(lat.value(PhysicalQuantities::CAngleUnit::rad()), lon.value(PhysicalQuantities::CAngleUnit::rad()), degree); }
            CRange<Iterators::ConcatIterator<const_iterator>> inAdjacentTiles(const ICoordinateGeodetic &coord, int degree = 1) const { return inAdjacentTiles(coord.latitude(), coord.longitude(), degree); }
            //! @}

            //! Returns the internal keys corresponding to all the tiles.
            QList<Key> keys() const { return m_map.uniqueKeys(); }

            //! Returns the number of elements in the tile corresponding to this internal key.
            int count(Key k) const { return m_map.count(k); }

        private:
            QMultiMap<Key, T> m_map;

            static_assert(std::is_signed<Key>::value && std::is_integral<Key>::value, "Key must be a signed integer");
            static_assert(Slices > 1 && !(Slices & (Slices - 1)), "Slices must be a power of two");
            static_assert(LogBase2<Key, Slices>::value * 3 < sizeof(Key) * 8, "Key is too small to hold all Slices");

            static const Key Zshift = 0;
            static const Key Zmask = Slices - 1;
            static const Key Zone = 1;
            static const Key Yshift = LogBase2<Key, Slices>::value;
            static const Key Ymask = Zmask << Yshift;
            static const Key Yone = Zone << Yshift;
            static const Key Xshift = Yshift * 2;
            static const Key Xmask = Zmask << Xshift;
            static const Key Xone = Zone << Xshift;

            static Key coordinateToKey(double lat, double lon)
            {
                using namespace std;
                using namespace BlackMisc::Math;
                Q_ASSERT(lat >= -CMath::PIHALF() && lat <= CMath::PIHALF());
                Q_ASSERT(lon >= -CMath::PI() && lon <= CMath::PI());
                static const double ratio = Slices / CMath::PI();
                Key x = qFloor(acos(cos(lat) * cos(lon)) * ratio);
                Key y = qFloor(acos(cos(lat) * sin(lon)) * ratio);
                Key z = qFloor(  (lat + CMath::PIHALF()) * ratio);
                return (x << Xshift) | (y << Yshift) | (z << Zshift);
            }

            static QVector<Key> adjacentKeys(Key k, int d)
            {
                QVector<Key> adj;
                for (int dx = -d; dx <= d; ++dx)
                {
                    for (int dy = -d; dy <= d; ++dy)
                    {
                        for (int dz = -d; dz <= d; ++dz)
                        {
                            adj.push_back(plus(k, dx, dy, dz));
                        }
                    }
                }
                return adj;
            }

            static Key plus(Key k, Key dx, Key dy, Key dz)
            {
                Key x = k & Xmask;
                Key y = k & Ymask;
                Key z = k & Zmask;
                dx *= Xone;
                dy *= Yone;
                dz *= Zone;
                if ((dx < 0 ? (-dx > x) : (dx > Xmask - x))
                 || (dy < 0 ? (-dy > y) : (dy > Ymask - y))
                 || (dz < 0 ? (-dz > z) : (dz > Zmask - z)))
                {
                    return -1;
                }
                else
                {
                    return (x + dx) | (y + dy) | (z + dz);
                }
            }
        };

    } // namespace
} // namespace

#endif // guard

