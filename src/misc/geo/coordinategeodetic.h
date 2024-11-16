// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_COORDINATEGEODETIC_H
#define SWIFT_MISC_GEO_COORDINATEGEODETIC_H

#include <array>

#include <QString>
#include <QVector3D>

#include "misc/aviation/altitude.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/math/mathutils.h"
#include "misc/metaclass.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::geo, CCoordinateGeodetic)

namespace swift::misc
{
    namespace geo
    {
        //! Geodetic coordinate, a position in 3D space relative to the reference geoid.
        //!
        //! Composed of a latitude, longitude, and height (aka altitude, aka elevation).
        //! \sa http://www.esri.com/news/arcuser/0703/geoid1of3.html
        //! \sa http://http://www.gmat.unsw.edu.au/snap/gps/clynch_pdfs/coordcvt.pdf (page 5)
        //! \sa http://en.wikipedia.org/wiki/Geodetic_datum#Vertical_datum
        class SWIFT_MISC_EXPORT ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexLatitude = CPropertyIndexRef::GlobalIndexICoordinateGeodetic,
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
            ICoordinateGeodetic &operator=(const ICoordinateGeodetic &) = default;

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
            virtual const aviation::CAltitude &geodeticHeight() const = 0;

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

            //! Geodetic height not null and aviation::CAltitude::MeanSeaLevel
            bool hasMSLGeodeticHeight() const { return this->geodeticHeight().hasMeanSeaLevelValue(); }

            //! Is null, means vector x, y, z == 0
            //! \remark this is a default implementation, concrete implementations of ICoordinateGeodetic might override
            //! it
            virtual bool isNull() const { return this->normalVector().isNull(); }

            //! Great circle distance
            physical_quantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &otherCoordinate) const;

            //! Object within range?
            bool isWithinRange(const ICoordinateGeodetic &otherCoordinate,
                               const physical_quantities::CLength &range) const;

            //! Initial bearing
            physical_quantities::CAngle calculateBearing(const ICoordinateGeodetic &otherCoordinate) const;

            //! \copydoc mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const ICoordinateGeodetic &compareValue) const;

            //! \copydoc mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! @{
            //! Check values
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
        SWIFT_MISC_EXPORT physical_quantities::CLength
        calculateGreatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Initial bearing
        SWIFT_MISC_EXPORT physical_quantities::CAngle calculateBearing(const ICoordinateGeodetic &coordinate1,
                                                                       const ICoordinateGeodetic &coordinate2);

        //! Euclidean distance between normal vectors
        SWIFT_MISC_EXPORT double calculateEuclideanDistance(const ICoordinateGeodetic &coordinate1,
                                                            const ICoordinateGeodetic &coordinate2);

        //! Euclidean distance squared between normal vectors, use for more efficient sorting by distance
        SWIFT_MISC_EXPORT double calculateEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate1,
                                                                   const ICoordinateGeodetic &coordinate2);

        //! Interface (actually more an abstract class) of coordinates and relative position to something (normally own
        //! aircraft)
        class SWIFT_MISC_EXPORT ICoordinateWithRelativePosition : public ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexRelativeDistance = CPropertyIndexRef::GlobalIndexICoordinateWithRelativePosition,
                IndexRelativeBearing
            };

            //! Get the distance
            const physical_quantities::CLength &getRelativeDistance() const { return m_relativeDistance; }

            //! Set relative distance
            void setRelativeDistance(const physical_quantities::CLength &distance) { m_relativeDistance = distance; }

            //! Get the relative bearing
            const physical_quantities::CAngle &getRelativeBearing() const { return m_relativeBearing; }

            //! Set bearing to own plane
            void setRelativeBearing(const physical_quantities::CAngle &angle) { m_relativeBearing = angle; }

            //! Valid distance?
            bool hasValidRelativeDistance() const { return !m_relativeDistance.isNull(); }

            //! Valid bearing?
            bool hasValidRelativeBearing() const { return !m_relativeBearing.isNull(); }

            //! Calculcate distance, set it, and return distance
            physical_quantities::CLength calculcateAndUpdateRelativeDistance(const geo::ICoordinateGeodetic &position);

            //! Calculcate distance and bearing to plane, set it, and return distance
            physical_quantities::CLength
            calculcateAndUpdateRelativeDistanceAndBearing(const geo::ICoordinateGeodetic &position);

            //! \copydoc mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index,
                                       const ICoordinateWithRelativePosition &compareValue) const;

            //! \copydoc mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        protected:
            //! Constructor
            ICoordinateWithRelativePosition();

            //! Can given index be handled?
            static bool canHandleIndex(CPropertyIndexRef index);

            physical_quantities::CAngle m_relativeBearing { 0, nullptr }; //!< temporary stored value
            physical_quantities::CLength m_relativeDistance { 0, nullptr }; //!< temporary stored value
        };

        //! Geodetic coordinate
        class SWIFT_MISC_EXPORT CCoordinateGeodetic :
            public CValueObject<CCoordinateGeodetic>,
            public ICoordinateGeodetic
        {
        public:
            //! Default constructor (null coordinate)
            CCoordinateGeodetic() {}

            //! Constructor by normal vector
            CCoordinateGeodetic(const QVector3D &normal)
                : m_x(static_cast<double>(normal.x())), m_y(static_cast<double>(normal.y())),
                  m_z(static_cast<double>(normal.z()))
            {}

            //! Constructor by normal vector
            CCoordinateGeodetic(const std::array<double, 3> &normalVector);

            //! Constructor by latitude/longitude, height is null
            CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude);

            //! Constructor by latitude/longitude/height (or altitude)
            CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude,
                                const aviation::CAltitude &geodeticHeight);

            //! Constructor by double values, but no geodetic height
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees);

            //! Constructor by values
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightFeet);

            //! Constructor by interface
            CCoordinateGeodetic(const ICoordinateGeodetic &coordinate);

            //! Calculate a position in distance/bearing
            CCoordinateGeodetic calculatePosition(const physical_quantities::CLength &distance,
                                                  const physical_quantities::CAngle &relBearing) const;

            //! \copydoc ICoordinateGeodetic::latitude
            virtual CLatitude latitude() const override;

            //! \copydoc ICoordinateGeodetic::longitude
            virtual CLongitude longitude() const override;

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            virtual const aviation::CAltitude &geodeticHeight() const override { return m_geodeticHeight; }

            //! \copydoc ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override;

            //! \copydoc ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override;

            //! \copydoc mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc mixin::Index::setPropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CCoordinateGeodetic &compareValue) const;

            //! Switch unit of height
            CCoordinateGeodetic &switchUnit(const physical_quantities::CLengthUnit &unit);

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
            void setGeodeticHeight(const aviation::CAltitude &height) { m_geodeticHeight = height; }

            //! Set height to NULL
            void setGeodeticHeightToNull();

            //! Set normal vector
            void setNormalVector(const QVector3D &normal)
            {
                m_x = static_cast<double>(normal.x());
                m_y = static_cast<double>(normal.y());
                m_z = static_cast<double>(normal.z());
            }

            //! Set normal vector
            void setNormalVector(double x, double y, double z)
            {
                m_x = x;
                m_y = y;
                m_z = z;
            }

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
                return math::CMathUtils::epsilonZeroLimits(m_x) && math::CMathUtils::epsilonZeroLimits(m_y) &&
                       math::CMathUtils::epsilonZeroLimits(m_z);
            }

            //! Coordinate by WGS84 position data
            static CCoordinateGeodetic fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84,
                                                 const aviation::CAltitude &geodeticHeight = {});

            //! null coordinate
            static const CCoordinateGeodetic &null();

            //! \copydoc mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            // Coordinates are stored using `double` internally and use `double` for most calculations.
            // They use `double` when converting to and from lat/lon representation.
            // `QVector3D` is only used for calculating distance and bearing.
            double m_x = 0; //!< normal vector [-1,1]
            double m_y = 0; //!< normal vector [-1,1]
            double m_z = 0; //!< normal vector [-1,1]
            aviation::CAltitude m_geodeticHeight { 0, nullptr }; //!< height, ellipsoidal or geodetic height

            SWIFT_METACLASS(
                CCoordinateGeodetic,
                SWIFT_METAMEMBER(x),
                SWIFT_METAMEMBER(y),
                SWIFT_METAMEMBER(z),
                SWIFT_METAMEMBER(geodeticHeight));
        };
    } // namespace geo

    //! \cond
    template <>
    struct TString<geo::ICoordinateGeodetic>
    {
        static QString toQString(const geo::ICoordinateGeodetic &coord) { return coord.convertToQString(); }
    };
    //! \endcond
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::geo::CCoordinateGeodetic)

#endif // guard
