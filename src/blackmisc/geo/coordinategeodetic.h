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
                IndexGeodeticHeightAsString
            };

            //! Destructor
            virtual ~ICoordinateGeodetic() {}

            //! Latitude
            virtual const CLatitude &latitude() const = 0;

            //! Longitude
            virtual const CLongitude &longitude() const = 0;

            //! Height, ellipsoidal or geodetic height (used in GPS)
            //! This is approximately MSL (orthometric) height, aka elevation.
            //! \sa see http://www.gmat.unsw.edu.au/snap/gps/clynch_pdfs/coordcvt.pdf page 5
            //! \sa http://www.esri.com/news/arcuser/0703/geoid1of3.html
            virtual const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const = 0;

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

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

            //! Can given index be handled
            static bool canHandleIndex(const BlackMisc::CPropertyIndex &index)
            {
                int i = index.frontCasted<int>();
                return (i >= static_cast<int>(IndexLatitude)) && (i <= static_cast<int>(IndexGeodeticHeightAsString));
            }
        };

        //! Great circle distance between points
        BLACKMISC_EXPORT BlackMisc::PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        //! Initial bearing
        BLACKMISC_EXPORT BlackMisc::PhysicalQuantities::CAngle calculateBearing(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

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

            BlackMisc::PhysicalQuantities::CAngle  m_bearingToOwnAircraft;  //!< temporary stored value
            BlackMisc::PhysicalQuantities::CLength m_distanceToOwnAircraft; //!< temporary stored value
        };


        //! Geodetic coordinate
        class BLACKMISC_EXPORT CCoordinateGeodetic : public CValueObject<CCoordinateGeodetic>, public ICoordinateGeodetic
        {

        public:
            //! Default constructor
            CCoordinateGeodetic() = default;

            //! Constructor by values
            CCoordinateGeodetic(CLatitude latitude, CLongitude longitude, BlackMisc::PhysicalQuantities::CLength height) :
                m_latitude(latitude), m_longitude(longitude), m_geodeticHeight(height) {}

            //! Constructor by values
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightMeters) :
                m_latitude(latitudeDegrees, BlackMisc::PhysicalQuantities::CAngleUnit::deg()), m_longitude(longitudeDegrees, BlackMisc::PhysicalQuantities::CAngleUnit::deg()), m_geodeticHeight(heightMeters, BlackMisc::PhysicalQuantities::CLengthUnit::m()) {}

            //! \copydoc ICoordinateGeodetic::latitude
            virtual const CLatitude &latitude() const override { return this->m_latitude; }

            //! \copydoc ICoordinateGeodetic::longitude
            virtual const CLongitude &longitude() const override { return this->m_longitude; }

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            virtual const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_geodeticHeight; }

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Switch unit of latitude / longitude
            CCoordinateGeodetic &switchUnit(const BlackMisc::PhysicalQuantities::CAngleUnit &unit);

            //! Switch unit of height
            CCoordinateGeodetic &switchUnit(const BlackMisc::PhysicalQuantities::CLengthUnit &unit);

            //! Set latitude
            void setLatitude(const CLatitude &latitude) { this->m_latitude = latitude; }

            //! Set longitude
            void setLongitude(const CLongitude &longitude) { this->m_longitude = longitude; }

            //! Set height (ellipsoidal or geodetic height)
            void setGeodeticHeight(const BlackMisc::PhysicalQuantities::CLength &height) { this->m_geodeticHeight = height; }

            //! Coordinate by WGS84 position data
            static CCoordinateGeodetic fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const BlackMisc::PhysicalQuantities::CLength &geodeticHeight = {});

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCoordinateGeodetic)
            BlackMisc::Geo::CLatitude  m_latitude;  //!< Latitude
            BlackMisc::Geo::CLongitude m_longitude; //!< Longitude
            BlackMisc::PhysicalQuantities::CLength m_geodeticHeight { 0, BlackMisc::PhysicalQuantities::CLengthUnit::nullUnit() }; //!< height, ellipsoidal or geodetic height
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Geo::CCoordinateGeodetic, (o.m_latitude, o.m_longitude, o.m_geodeticHeight))
Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateGeodetic)

#endif // guard
