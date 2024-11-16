// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_ELEVATIONPLANE_H
#define SWIFT_MISC_GEO_ELEVATIONPLANE_H

#include "misc/geo/coordinategeodetic.h"
#include "misc/mixin/mixinstring.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::geo
{
    //! Plane of same elevation, can be a single point or larger area (e.g. airport)
    //! \remark 100km/h 1sec => 28m
    class SWIFT_MISC_EXPORT CElevationPlane :
        public CCoordinateGeodetic,
        public mixin::MetaType<CElevationPlane>,
        public mixin::String<CElevationPlane>
    {
    public:
        //! Base type
        using base_type = CCoordinateGeodetic;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CElevationPlane)
        SWIFT_MISC_DECLARE_USING_MIXIN_STRING(CElevationPlane)

        //! Properties by index
        enum ColumnIndex
        {
            IndexRadius = CPropertyIndexRef::GlobalIndexCElevationPlane
        };

        //! Default constructor
        CElevationPlane() {}

        //! Plane at given coordinates with range to 2nd coordinate
        CElevationPlane(const ICoordinateGeodetic &coordinate, const ICoordinateGeodetic &rangeCoordinate);

        //! Plane at given coordinates with radius
        CElevationPlane(const ICoordinateGeodetic &coordinate, const physical_quantities::CLength &radius);

        //! Plane at given coordinates with radius and altitude
        CElevationPlane(const ICoordinateGeodetic &coordinate, const aviation::CAltitude &altitude,
                        const physical_quantities::CLength &radius);

        //! Plane at given coordinates with radius and altitude
        CElevationPlane(const ICoordinateGeodetic &coordinate, double altitudeMSLft,
                        const physical_quantities::CLength &radius);

        //! Plane at given coordinates with radius and altitude
        CElevationPlane(double latDeg, double lngDeg, double altitudeMSLft, const physical_quantities::CLength &radius);

        //! Plane at given coordinates with radius and altitude
        CElevationPlane(const CLatitude &lat, const CLongitude &lng, const aviation::CAltitude &altitude,
                        const physical_quantities::CLength &radius);

        //! Constructors from CCoordinateGeodetic
        using CCoordinateGeodetic::CCoordinateGeodetic;

        //! Radius
        const physical_quantities::CLength &getRadius() const { return m_radius; }

        //! Radius or minimum radius
        const physical_quantities::CLength &getRadiusOrMinimumRadius() const;

        //! Radius
        void setRadius(const physical_quantities::CLength &radius) { m_radius = radius; }

        //! Radius or minimum CElevationPlane::singlePointRadius
        void setRadiusOrMinimumRadius(const physical_quantities::CLength &radius);

        //! Set minimum radius if not having radius
        void fixRadius();

        //! Add offset to altitude
        void addAltitudeOffset(const physical_quantities::CLength &offset);

        //! Switch altitude unit
        void switchAltitudeUnit(const physical_quantities::CLengthUnit &unit);

        //! Altitude when within radius, else null
        const aviation::CAltitude &getAltitudeIfWithinRadius(const ICoordinateGeodetic &coordinate) const;

        //! Altitude (synonym for geodetic height)
        const aviation::CAltitude &getAltitude() const { return this->geodeticHeight(); }

        //! Altitude (synonym for geodetic height) unit
        const physical_quantities::CLengthUnit &getAltitudeUnit() const { return this->geodeticHeight().getUnit(); }

        //! Altitude (synonym for geodetic height)
        aviation::CAltitude getAltitudeInUnit(const physical_quantities::CLengthUnit &unit) const;

        //! Altitude (synonym for geodetic height)
        double getAltitudeValue(const physical_quantities::CLengthUnit &unit) const;

        //! Existing value?
        virtual bool isNull() const override;

        //! Check if elevation is within radius and can be used
        //! \remark checks against the set radius
        bool isWithinRange(const ICoordinateGeodetic &coordinate) const;

        //! Check if elevation is within radius and can be used
        bool isWithinRange(const ICoordinateGeodetic &coordinate, const physical_quantities::CLength &radius) const;

        //! Treat as single point as obtained from simulator
        void setSinglePointRadius();

        //! Treat as elevation of a small airport
        void setMinorAirportRadius();

        //! Treat as elevation of a small airport
        void setMajorAirportRadius();

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CElevationPlane &elevationPlane) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Radius for single point
        static const physical_quantities::CLength &singlePointRadius();

        //! Radius for minor airport
        static const physical_quantities::CLength &minorAirportRadius();

        //! Radius for major airport
        static const physical_quantities::CLength &majorAirportRadius();

        //! NULL plane
        static const CElevationPlane &null();

    private:
        physical_quantities::CLength m_radius { 0, nullptr }; //!< elevation is valid in radius

        SWIFT_METACLASS(
            CElevationPlane,
            SWIFT_METAMEMBER(radius));
    };
} // namespace swift::misc::geo

Q_DECLARE_METATYPE(swift::misc::geo::CElevationPlane)

#endif // guard
