/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_ELEVATIONPLANE_H
#define BLACKMISC_GEO_ELEVATIONPLANE_H

#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    namespace Geo
    {
        //! Plane of same elevation, can be a single point or larger area (e.g. airport)
        //! \remark 100km/h 1sec => 28m
        class BLACKMISC_EXPORT CElevationPlane :
            public CCoordinateGeodetic,
            public Mixin::MetaType<CElevationPlane>,
            public Mixin::String<CElevationPlane>
        {
        public:
            //! Base type
            using base_type = CCoordinateGeodetic;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CElevationPlane)
            BLACKMISC_DECLARE_USING_MIXIN_STRING(CElevationPlane)

            //! Properties by index
            enum ColumnIndex
            {
                IndexRadius = CPropertyIndex::GlobalIndexCElevationPlane
            };

            //! Default constructor
            CElevationPlane() {}

            //! Plane at given coordinates with range to 2nd coordinate
            CElevationPlane(const ICoordinateGeodetic &coordinate, const ICoordinateGeodetic &rangeCoordinate);

            //! Plane at given coordinates with radius
            CElevationPlane(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &radius);

            //! Plane at given coordinates with radius and altitude
            CElevationPlane(const ICoordinateGeodetic &coordinate, const Aviation::CAltitude &altitude, const PhysicalQuantities::CLength &radius);

            //! Plane at given coordinates with radius and altitude
            CElevationPlane(const ICoordinateGeodetic &coordinate, double altitudeMSLft, const PhysicalQuantities::CLength &radius);

            //! Plane at given coordinates with radius and altitude
            CElevationPlane(double latDeg, double lngDeg, double altitudeMSLft, const PhysicalQuantities::CLength &radius);

            //! Plane at given coordinates with radius and altitude
            CElevationPlane(const CLatitude &lat, const CLongitude &lng, const Aviation::CAltitude &altitude, const PhysicalQuantities::CLength &radius);

            //! Constructors from CCoordinateGeodetic
            using CCoordinateGeodetic::CCoordinateGeodetic;

            //! Radius
            const PhysicalQuantities::CLength &getRadius() const { return m_radius; }

            //! Radius or minimum radius
            const PhysicalQuantities::CLength &getRadiusOrMinimumRadius() const;

            //! Radius
            void setRadius(const PhysicalQuantities::CLength &radius) { m_radius = radius; }

            //! Radius or minimum CElevationPlane::singlePointRadius
            void setRadiusOrMinimumRadius(const PhysicalQuantities::CLength &radius);

            //! Set minimum radius if not having radius
            void fixRadius();

            //! Add offset to altitude
            void addAltitudeOffset(const PhysicalQuantities::CLength &offset);

            //! Switch altitude unit
            void switchAltitudeUnit(const PhysicalQuantities::CLengthUnit &unit);

            //! Altitude when within radius, else null
            const Aviation::CAltitude &getAltitudeIfWithinRadius(const ICoordinateGeodetic &coordinate) const;

            //! Altitude (synonym for geodetic height)
            const Aviation::CAltitude &getAltitude() const { return this->geodeticHeight(); }

            //! Altitude (synonym for geodetic height) unit
            const PhysicalQuantities::CLengthUnit &getAltitudeUnit() const { return this->geodeticHeight().getUnit(); }

            //! Altitude (synonym for geodetic height)
            Aviation::CAltitude getAltitudeInUnit(const PhysicalQuantities::CLengthUnit &unit) const;

            //! Altitude (synonym for geodetic height)
            double getAltitudeValue(const PhysicalQuantities::CLengthUnit &unit) const;

            //! Existing value?
            virtual bool isNull() const override;

            //! Check if elevation is within radius and can be used
            //! \remark checks against the set radius
            bool isWithinRange(const ICoordinateGeodetic &coordinate) const;

            //! Check if elevation is within radius and can be used
            bool isWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &radius) const;

            //! Treat as single point as obtained from simulator
            void setSinglePointRadius();

            //! Treat as elevation of a small airport
            void setMinorAirportRadius();

            //! Treat as elevation of a small airport
            void setMajorAirportRadius();

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(const CPropertyIndex &index, const CElevationPlane &elevationPlane) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Radius for single point
            static const PhysicalQuantities::CLength &singlePointRadius();

            //! Radius for minor airport
            static const PhysicalQuantities::CLength &minorAirportRadius();

            //! Radius for major airport
            static const PhysicalQuantities::CLength &majorAirportRadius();

            //! NULL plane
            static const CElevationPlane &null();

        private:
            PhysicalQuantities::CLength m_radius { 0, nullptr }; //!< elevation is valid in radius

            BLACK_METACLASS(
                CElevationPlane,
                BLACK_METAMEMBER(radius)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CElevationPlane)

#endif // guard
