/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_COORDINATEGEODETIC_H
#define BLACKMISC_GEO_COORDINATEGEODETIC_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/mixin/mixinmetatype.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/blackmiscexport.h"

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
                IndexLatitude = CPropertyIndex::GlobalIndexICoordinateGeodetic,
                IndexLongitude,
                IndexLatitudeAsString,
                IndexLongitudeAsString,
                IndexGeodeticHeight,
                IndexGeodeticHeightAsString,
                IndexNormalVector
            };

            //! Ctor
            ICoordinateGeodetic() = default;

            //! Destructor
            virtual ~ICoordinateGeodetic();

            //! Copy constructor
            ICoordinateGeodetic(const ICoordinateGeodetic &) = default;

            //! Copy assignment operator
            ICoordinateGeodetic &operator =(const ICoordinateGeodetic &) = default;

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
            virtual const Aviation::CAltitude &geodeticHeight() const = 0;

            //! Normal vector
            //! \note QVector3D uses floats, so don't use if double precision is needed
            //! \sa https://en.wikipedia.org/wiki/N-vector
            //! \sa http://www.movable-type.co.uk/scripts/latlong-vectors.html
            virtual QVector3D normalVector() const = 0;

            //! Normal vector with double precision
            virtual std::array<double, 3> normalVectorDouble() const = 0;

            //! Is equal? Epsilon considered.
            bool equalNormalVectorDouble(const std::array<double, 3> &otherVector) const;

            //! Is equal, epsilon considered?
            bool equalNormalVectorDouble(const ICoordinateGeodetic &otherCoordinate) const;

            //! Latitude as string
            QString latitudeAsString() const { return this->latitude().toQString(true); }

            //! Longitude as string
            QString longitudeAsString() const { return this->longitude().toQString(true); }

            //! Height as string
            QString geodeticHeightAsString() const { return this->geodeticHeight().toQString(true); }

            //! Geodetic height null?
            bool isGeodeticHeightNull() const { return this->geodeticHeight().isNull(); }

            //! Geodetic height not null and Aviation::CAltitude::MeanSeaLevel
            bool hasMSLGeodeticHeight() const { return this->geodeticHeight().hasMeanSeaLevelValue(); }

            //! Is null, means vector x, y, z == 0
            //! \remark this is a default implementation, concrete implementations of ICoordinateGeodetic might override it
            virtual bool isNull() const { return this->normalVector().isNull(); }

            //! Great circle distance
            PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &otherCoordinate) const;

            //! Object within range?
            bool isWithinRange(const ICoordinateGeodetic &otherCoordinate, const PhysicalQuantities::CLength &range) const;

            //! Initial bearing
            PhysicalQuantities::CAngle calculateBearing(const ICoordinateGeodetic &otherCoordinate) const;

            //! \copydoc Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const ICoordinateGeodetic &compareValue) const;

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Check values
            //! @{
            bool isNaNVector() const;
            bool isNaNVectorDouble() const;
            bool isInfVector() const;
            bool isInfVectorDouble() const;
            bool isValidVectorRange() const;
            static bool isValidVector(const std::array<double, 3> &v);
            //! @}

        protected:
            //! Can given index be handled?
            static bool canHandleIndex(CPropertyIndexRef index);
        };

        //! Great circle distance between points
        BLACKMISC_EXPORT PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Initial bearing
        BLACKMISC_EXPORT PhysicalQuantities::CAngle calculateBearing(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

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
                IndexRelativeDistance = CPropertyIndex::GlobalIndexICoordinateWithRelativePosition,
                IndexRelativeBearing
            };

            //! Get the distance
            const PhysicalQuantities::CLength &getRelativeDistance() const { return m_relativeDistance; }

            //! Set relative distance
            void setRelativeDistance(const PhysicalQuantities::CLength &distance) { m_relativeDistance = distance; }

            //! Get the relative bearing
            const PhysicalQuantities::CAngle &getRelativeBearing() const { return m_relativeBearing; }

            //! Set bearing to own plane
            void setRelativeBearing(const PhysicalQuantities::CAngle &angle) { m_relativeBearing = angle; }

            //! Valid distance?
            bool hasValidRelativeDistance() const { return !m_relativeDistance.isNull();}

            //! Valid bearing?
            bool hasValidRelativeBearing() const { return !m_relativeBearing.isNull();}

            //! Calculcate distance, set it, and return distance
            PhysicalQuantities::CLength calculcateAndUpdateRelativeDistance(const Geo::ICoordinateGeodetic &position);

            //! Calculcate distance and bearing to plane, set it, and return distance
            PhysicalQuantities::CLength calculcateAndUpdateRelativeDistanceAndBearing(const Geo::ICoordinateGeodetic &position);

            //! \copydoc Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const ICoordinateWithRelativePosition &compareValue) const;

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        protected:
            //! Constructor
            ICoordinateWithRelativePosition();

            //! Can given index be handled?
            static bool canHandleIndex(CPropertyIndexRef index);

            PhysicalQuantities::CAngle  m_relativeBearing  { 0, nullptr }; //!< temporary stored value
            PhysicalQuantities::CLength m_relativeDistance { 0, nullptr }; //!< temporary stored value
        };

        //! Geodetic coordinate
        class BLACKMISC_EXPORT CCoordinateGeodetic : public CValueObject<CCoordinateGeodetic>, public ICoordinateGeodetic
        {
        public:
            //! Default constructor (null coordinate)
            CCoordinateGeodetic() {}

            //! Constructor by normal vector
            CCoordinateGeodetic(const QVector3D &normal) : m_x(static_cast<double>(normal.x())), m_y(static_cast<double>(normal.y())), m_z(static_cast<double>(normal.z())) {}

            //! Constructor by normal vector
            CCoordinateGeodetic(const std::array<double, 3> &normalVector);

            //! Constructor by latitude/longitude, height is null
            CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude);

            //! Constructor by latitude/longitude/height (or altitude)
            CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude, const Aviation::CAltitude &geodeticHeight);

            //! Constructor by double values, but no geodetic height
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees);

            //! Constructor by values
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightFeet);

            //! Constructor by interface
            CCoordinateGeodetic(const ICoordinateGeodetic &coordinate);

            //! Calculate a position in distance/bearing
            CCoordinateGeodetic calculatePosition(const PhysicalQuantities::CLength &distance, const PhysicalQuantities::CAngle &relBearing) const;

            //! \copydoc ICoordinateGeodetic::latitude
            virtual CLatitude latitude() const override;

            //! \copydoc ICoordinateGeodetic::longitude
            virtual CLongitude longitude() const override;

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            virtual const Aviation::CAltitude &geodeticHeight() const override { return m_geodeticHeight; }

            //! \copydoc ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override;

            //! \copydoc ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override;

            //! \copydoc Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc Mixin::Index::setPropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CCoordinateGeodetic &compareValue) const;

            //! Switch unit of height
            CCoordinateGeodetic &switchUnit(const PhysicalQuantities::CLengthUnit &unit);

            //! Set latitude
            void setLatitude(const CLatitude &latitude);

            //! Set latitude
            void setLatitudeFromWgs84(const QString &wgs);

            //! Set longitude
            void setLongitude(const CLongitude &longitude);

            //! Set longitude
            void setLongitudeFromWgs84(const QString &wgs);

            //! Set latitude and longitude
            void setLatLong(const CLatitude &latitude, const CLongitude &longitude);

            //! Set latitude and longitude
            void setLatLongFromWgs84(const QString &latitude, const QString &longitude);

            //! Set height (ellipsoidal or geodetic height)
            void setGeodeticHeight(const Aviation::CAltitude &height) { m_geodeticHeight = height; }

            //! Set height to NULL
            void setGeodeticHeightToNull();

            //! Set normal vector
            void setNormalVector(const QVector3D &normal) { m_x = static_cast<double>(normal.x()); m_y = static_cast<double>(normal.y()); m_z = static_cast<double>(normal.z()); }

            //! Set normal vector
            void setNormalVector(double x, double y, double z) { m_x = x; m_y = y; m_z = z; }

            //! Set normal vector
            void setNormalVector(const std::array<double, 3> &normalVector);

            //! Clamp vector values if out [-1, 1]
            int clampVector();

            //! Set to null
            void setNull()
            {
                this->setNormalVector(0, 0, 0);
                m_geodeticHeight.setNull();
            }

            //! Is null?
            virtual bool isNull() const override
            {
                if (m_geodeticHeight.isNull()) { return true; }
                return Math::CMathUtils::epsilonZeroLimits(m_x) && Math::CMathUtils::epsilonZeroLimits(m_y) && Math::CMathUtils::epsilonZeroLimits(m_z);
            }

            //! Coordinate by WGS84 position data
            static CCoordinateGeodetic fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const Aviation::CAltitude &geodeticHeight = {});

            //! null coordinate
            static const CCoordinateGeodetic &null();

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            // Coordinates are stored using `double` internally and use `double` for most calculations.
            // They use `double` when converting to and from lat/lon representation.
            // `QVector3D` is only used for calculating distance and bearing.
            double m_x = 0; //!< normal vector [-1,1]
            double m_y = 0; //!< normal vector [-1,1]
            double m_z = 0; //!< normal vector [-1,1]
            Aviation::CAltitude m_geodeticHeight { 0, nullptr }; //!< height, ellipsoidal or geodetic height

            BLACK_METACLASS(
                CCoordinateGeodetic,
                BLACK_METAMEMBER(x),
                BLACK_METAMEMBER(y),
                BLACK_METAMEMBER(z),
                BLACK_METAMEMBER(geodeticHeight)
            );
        };
    } // namespace

    //! \cond
    template <>
    struct TString<Geo::ICoordinateGeodetic>
    {
        static QString toQString(const Geo::ICoordinateGeodetic &coord) { return coord.convertToQString(); }
    };
    //! \endcond
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateGeodetic)

#endif // guard
