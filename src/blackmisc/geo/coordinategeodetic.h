/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COORDINATEGEODETIC_H
#define BLACKMISC_COORDINATEGEODETIC_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/propertyindex.h"
#include <QVector3D>
#include <array>

namespace BlackMisc
{
    namespace Geo
    {

        //! Geodetic coordinate
        //! \sa http://www.esri.com/news/arcuser/0703/geoid1of3.html
        //! \sa http://http://www.gmat.unsw.edu.au/snap/gps/clynch_pdfs/coordcvt.pdf (page 5)
        //! \sa http://en.wikipedia.org/wiki/Geodetic_datum#Vertical_datum
        class BLACKMISC_EXPORT ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexLatitude = BlackMisc::CPropertyIndex::GlobalIndexICoordinateGeodetic,
                IndexLongitude,
                IndexLatitudeAsString,
                IndexLongitudeAsString,
                IndexGeodeticHeight,
                IndexGeodeticHeightAsString,
                IndexNormalVector
            };

            //! Destructor
            virtual ~ICoordinateGeodetic() {}

            //! Latitude
            virtual CLatitude latitude() const = 0;

            //! Longitude
            virtual CLongitude longitude() const = 0;

            //! Height, ellipsoidal or geodetic height (used in GPS)
            //! This is approximately MSL (orthometric) height, aka elevation.
            //! \sa see http://www.gmat.unsw.edu.au/snap/gps/clynch_pdfs/coordcvt.pdf page 5
            //! \sa http://www.esri.com/news/arcuser/0703/geoid1of3.html
            virtual const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const = 0;

            //! Normal vector
            //! \note QVector3D uses floats, so don't use if double precision is needed
            //! \sa https://en.wikipedia.org/wiki/N-vector
            //! \sa http://www.movable-type.co.uk/scripts/latlong-vectors.html
            virtual QVector3D normalVector() const = 0;

            //! Normal vector with double precision
            virtual std::array<double, 3> normalVectorDouble() const = 0;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! Latitude as string
            QString latitudeAsString() const { return this->latitude().toQString(true); }

            //! Longitude as string
            QString longitudeAsString() const { return this->longitude().toQString(true); }

            //! Height as string
            QString geodeticHeightAsString() const { return this->geodeticHeight().toQString(true); }

            //! Great circle distance
            BlackMisc::PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &otherCoordinate) const;

            //! Initial bearing
            BlackMisc::PhysicalQuantities::CAngle bearing(const ICoordinateGeodetic &otherCoordinate) const;

        protected:
            //! Can given index be handled?
            static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);
        };

        //! Great circle distance between points
        BLACKMISC_EXPORT BlackMisc::PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Initial bearing
        BLACKMISC_EXPORT BlackMisc::PhysicalQuantities::CAngle calculateBearing(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Euclidean distance between normal vectors
        BLACKMISC_EXPORT double calculateEuclideanDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Euclidean distance squared between normal vectors, use for more efficient sorting by distance
        BLACKMISC_EXPORT double calculateEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Interface (actually more an abstract class) of coordinate and
        //! relative position to own aircraft
        class BLACKMISC_EXPORT ICoordinateWithRelativePosition : public ICoordinateGeodetic
        {
        public:
            //! Get the distance to own plane
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToOwnAircraft() const { return m_distanceToOwnAircraft; }

            //! Set distance to own plane
            void setDistanceToOwnAircraft(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToOwnAircraft = distance; }

            //! Get the bearing to own plane
            const BlackMisc::PhysicalQuantities::CAngle &getBearingToOwnAircraft() const { return m_bearingToOwnAircraft; }

            //! Set bearing to own plane
            void setBearingToOwnAircraft(const BlackMisc::PhysicalQuantities::CAngle &angle) { this->m_bearingToOwnAircraft = angle; }

            //! Valid distance?
            bool hasValidDistance() const { return !this->m_distanceToOwnAircraft.isNull();}

            //! Valid bearing?
            bool hasValidBearing() const { return !this->m_bearingToOwnAircraft.isNull();}

            //! Calculcate distance, set it, and return distance
            BlackMisc::PhysicalQuantities::CLength calculcateDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position, bool updateValues = true);

            //! Calculcate distance and bearing to plane, set it, and return distance
            BlackMisc::PhysicalQuantities::CLength calculcateDistanceAndBearingToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position, bool updateValues = true);

        protected:
            //! Constructor
            ICoordinateWithRelativePosition();

            BlackMisc::PhysicalQuantities::CAngle  m_bearingToOwnAircraft  {0.0, BlackMisc::PhysicalQuantities::CAngleUnit::nullUnit()};  //!< temporary stored value
            BlackMisc::PhysicalQuantities::CLength m_distanceToOwnAircraft {0.0, BlackMisc::PhysicalQuantities::CLengthUnit::nullUnit()}; //!< temporary stored value
        };


        //! Geodetic coordinate
        class BLACKMISC_EXPORT CCoordinateGeodetic : public CValueObject<CCoordinateGeodetic>, public ICoordinateGeodetic
        {

        public:
            //! Default constructor
            CCoordinateGeodetic() = default;

            //! Constructor by normal vector
            CCoordinateGeodetic(const QVector3D &normal) : m_x(normal.x()), m_y(normal.y()), m_z(normal.z()) {}

            //! Constructor by values
            CCoordinateGeodetic(CLatitude latitude, CLongitude longitude, BlackMisc::PhysicalQuantities::CLength height);

            //! Constructor by values
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightMeters) :
                CCoordinateGeodetic({ latitudeDegrees, BlackMisc::PhysicalQuantities::CAngleUnit::deg() }, { longitudeDegrees, BlackMisc::PhysicalQuantities::CAngleUnit::deg() }, { heightMeters, BlackMisc::PhysicalQuantities::CLengthUnit::m() }) {}

            //! \copydoc ICoordinateGeodetic::latitude
            virtual CLatitude latitude() const override;

            //! \copydoc ICoordinateGeodetic::longitude
            virtual CLongitude longitude() const override;

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            virtual const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_geodeticHeight; }

            //! \copydoc ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override;

            //! \copydoc ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Switch unit of height
            CCoordinateGeodetic &switchUnit(const BlackMisc::PhysicalQuantities::CLengthUnit &unit);

            //! Set latitude
            void setLatitude(const CLatitude &latitude);

            //! Set longitude
            void setLongitude(const CLongitude &longitude);

            //! Set latitude and longitude
            void setLatLong(const CLatitude &latitude, const CLongitude &longitude);

            //! Set height (ellipsoidal or geodetic height)
            void setGeodeticHeight(const BlackMisc::PhysicalQuantities::CLength &height) { this->m_geodeticHeight = height; }

            //! Set normal vector
            void setNormalVector(const QVector3D &normal) { this->m_x = normal.x(); this->m_y = normal.y(); this->m_z = normal.z(); }

            //! Set normal vector
            void setNormalVector(double x, double y, double z) { this->m_x = x; this->m_y = y; this->m_z = z; }

            //! Coordinate by WGS84 position data
            static CCoordinateGeodetic fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const BlackMisc::PhysicalQuantities::CLength &geodeticHeight = {});

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            double m_x = 0; //!< normal vector
            double m_y = 0; //!< normal vector
            double m_z = 0; //!< normal vector
            BlackMisc::PhysicalQuantities::CLength m_geodeticHeight { 0, BlackMisc::PhysicalQuantities::CLengthUnit::nullUnit() }; //!< height, ellipsoidal or geodetic height

            BLACK_METACLASS(CCoordinateGeodetic,
                BLACK_METAMEMBER(x),
                BLACK_METAMEMBER(y),
                BLACK_METAMEMBER(z),
                BLACK_METAMEMBER(geodeticHeight)
            );
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateGeodetic)

#endif // guard
