/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_ELEVATIONPLANE_H
#define BLACKMISC_GEO_ELEVATIONPLANE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/coordinategeodetic.h"

namespace BlackMisc
{
    namespace Geo
    {
        //! Plane of same elevation, can be a single point or larger area (e.g. airport)
        class BLACKMISC_EXPORT CElevationPlane :
            public CValueObject<CElevationPlane, CCoordinateGeodetic>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexRadius = BlackMisc::CPropertyIndex::GlobalIndexCElevationPlane
            };

            //! Default constructor
            CElevationPlane() {}

            //! Constructor
            using CValueObject<CElevationPlane, CCoordinateGeodetic>::CValueObject;

            //! Radius
            const BlackMisc::PhysicalQuantities::CLength &getRadius() const { return m_radius; }

            //! Radius
            void setRadius(const BlackMisc::PhysicalQuantities::CLength &radius) { m_radius = radius; }

            //! Existing value
            bool isNull() const;

            //! Check if elevation is within radius and can be used
            bool isWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate) const;

            //! Treat as single point as obtained from simulator
            void setSinglePointRadius();

            //! Treat as elevation of a small airport
            void setMinorAirportRadius();

            //! Treat as elevation of a small airport
            void setMajorAirportRadius();

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Radius for single point
            static const BlackMisc::PhysicalQuantities::CLength &singlePointRadius();

            //! Radius for minor airport
            static const BlackMisc::PhysicalQuantities::CLength &minorAirportRadius();

            //! Radius for major airport
            static const BlackMisc::PhysicalQuantities::CLength &majorAirportRadius();

        private:
            BlackMisc::PhysicalQuantities::CLength m_radius { 0, nullptr }; //!< elevation is valid in radius

            BLACK_METACLASS(
                CElevationPlane,
                BLACK_METAMEMBER(radius)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CElevationPlane)

#endif // guard
