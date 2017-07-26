/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_COORDINATEGEODETIC_H
#define BLACKMISC_GEO_COORDINATEGEODETIC_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>

namespace BlackMisc
{
    namespace Geo
    {
        //! Geodetic coordinate, a position in 3D space relative to the reference geoid.
        //!
        //! Composed of a latitude, longitude, and height (aka altitude, aka elevation).
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
            //!
            //! This is approximately MSL (orthometric) height, aka altitude, aka elevation.
            //! The terms "geodetic height", "altitude", and "elevation" are interchangable.
            //! "Geodetic height" is the generic, context-free term for the vertical component of a position.
            //! This is commonly called "altitude" for objects that can move freely in the vertical component.
            //! By a similar convention, "elevation" is commonly used for objects that are fixed to the ground.
            //! \sa see http://www.gmat.unsw.edu.au/snap/gps/clynch_pdfs/coordcvt.pdf page 5
            //! \sa http://www.esri.com/news/arcuser/0703/geoid1of3.html
            virtual const BlackMisc::Aviation::CAltitude &geodeticHeight() const = 0;

            //! Normal vector
            //! \note QVector3D uses floats, so don't use if double precision is needed
            //! \sa https://en.wikipedia.org/wiki/N-vector
            //! \sa http://www.movable-type.co.uk/scripts/latlong-vectors.html
            virtual QVector3D normalVector() const = 0;

            //! Normal vector with double precision
            virtual std::array<double, 3> normalVectorDouble() const = 0;

            //! Latitude as string
            QString latitudeAsString() const { return this->latitude().toQString(true); }

            //! Longitude as string
            QString longitudeAsString() const { return this->longitude().toQString(true); }

            //! Height as string
            QString geodeticHeightAsString() const { return this->geodeticHeight().toQString(true); }

            //! Great circle distance
            BlackMisc::PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &otherCoordinate) const;

            //! Initial bearing
            BlackMisc::PhysicalQuantities::CAngle calculateBearing(const ICoordinateGeodetic &otherCoordinate) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const ICoordinateGeodetic &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

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

        //! Interface (actually more an abstract class) of coordinates and relative position to something (normally own aircraft)
        class BLACKMISC_EXPORT ICoordinateWithRelativePosition : public ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexRelativeDistance = BlackMisc::CPropertyIndex::GlobalIndexICoordinateWithRelativePosition,
                IndexRelativeBearing
            };

            //! Get the distance
            const BlackMisc::PhysicalQuantities::CLength &getRelativeDistance() const { return m_relativeDistance; }

            //! Set relative distance
            void setRelativeDistance(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_relativeDistance = distance; }

            //! Get the relative bearing
            const BlackMisc::PhysicalQuantities::CAngle &getRelativeBearing() const { return m_relativeBearing; }

            //! Set bearing to own plane
            void setRelativeBearing(const BlackMisc::PhysicalQuantities::CAngle &angle) { this->m_relativeBearing = angle; }

            //! Valid distance?
            bool hasValidRelativeDistance() const { return !this->m_relativeDistance.isNull();}

            //! Valid bearing?
            bool hasValidRelativeBearing() const { return !this->m_relativeBearing.isNull();}

            //! Calculcate distance, set it, and return distance
            BlackMisc::PhysicalQuantities::CLength calculcateAndUpdateRelativeDistance(const BlackMisc::Geo::ICoordinateGeodetic &position);

            //! Calculcate distance and bearing to plane, set it, and return distance
            BlackMisc::PhysicalQuantities::CLength calculcateAndUpdateRelativeDistanceAndBearing(const BlackMisc::Geo::ICoordinateGeodetic &position);

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const ICoordinateWithRelativePosition &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        protected:
            //! Constructor
            ICoordinateWithRelativePosition();

            //! Can given index be handled?
            static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

            BlackMisc::PhysicalQuantities::CAngle  m_relativeBearing  { 0, nullptr }; //!< temporary stored value
            BlackMisc::PhysicalQuantities::CLength m_relativeDistance { 0, nullptr }; //!< temporary stored value
        };

        //! Geodetic coordinate
        class BLACKMISC_EXPORT CCoordinateGeodetic : public CValueObject<CCoordinateGeodetic>, public ICoordinateGeodetic
        {
        public:
            //! Default constructor
            CCoordinateGeodetic() : CCoordinateGeodetic(0, 0, 0) {}

            //! Constructor by normal vector
            CCoordinateGeodetic(const QVector3D &normal) : m_x(normal.x()), m_y(normal.y()), m_z(normal.z()) {}

            //! Constructor by values
            CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude, const BlackMisc::Aviation::CAltitude &geodeticHeight);

            //! Constructor by double values, but no geodetic height
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees);

            //! Constructor by values
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightFeet);

            //! Constructor by interface
            CCoordinateGeodetic(const ICoordinateGeodetic &coordinate);

            //! \copydoc ICoordinateGeodetic::latitude
            virtual CLatitude latitude() const override;

            //! \copydoc ICoordinateGeodetic::longitude
            virtual CLongitude longitude() const override;

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            virtual const BlackMisc::Aviation::CAltitude &geodeticHeight() const override { return this->m_geodeticHeight; }

            //! \copydoc ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override;

            //! \copydoc ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Switch unit of height
            CCoordinateGeodetic &switchUnit(const BlackMisc::PhysicalQuantities::CLengthUnit &unit);

            //! Set latitude
            void setLatitude(const CLatitude &latitude);

            //! Set longitude
            void setLongitude(const CLongitude &longitude);

            //! Set latitude and longitude
            void setLatLong(const CLatitude &latitude, const CLongitude &longitude);

            //! Set height (ellipsoidal or geodetic height)
            void setGeodeticHeight(const BlackMisc::Aviation::CAltitude &height) { this->m_geodeticHeight = height; }

            //! Set normal vector
            void setNormalVector(const QVector3D &normal) { this->m_x = normal.x(); this->m_y = normal.y(); this->m_z = normal.z(); }

            //! Set normal vector
            void setNormalVector(double x, double y, double z) { this->m_x = x; this->m_y = y; this->m_z = z; }

            //! Coordinate by WGS84 position data
            static CCoordinateGeodetic fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const BlackMisc::Aviation::CAltitude &geodeticHeight = {});

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            double m_x = 0; //!< normal vector
            double m_y = 0; //!< normal vector
            double m_z = 0; //!< normal vector
            BlackMisc::Aviation::CAltitude m_geodeticHeight { 0, nullptr }; //!< height, ellipsoidal or geodetic height

            BLACK_METACLASS(
                CCoordinateGeodetic,
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
