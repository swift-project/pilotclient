/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONHINTS_H
#define BLACKMISC_SIMULATION_INTERPOLATIONHINTS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Hints for interpolator such as ground elevation
        class BLACKMISC_EXPORT CInterpolationHints :
            public BlackMisc::CValueObject<CInterpolationHints>
        {
        public:
            //! Property indexes
            enum ColumnIndex
            {
                IndexElevationPlane = CPropertyIndex::GlobalIndexCInterpolationHints,
                IndexCenterOfGravity,
                IndexIsVtolAircraft
            };

            //! Default constructor.
            CInterpolationHints();

            //! Constructor
            CInterpolationHints(bool isVtolAircraft);

            //! Get elevation
            const BlackMisc::Geo::CElevationPlane &getElevation() const { return m_elevation;}

            //! Set elevation
            void setElevation(const BlackMisc::Geo::CElevationPlane &elevation) { m_elevation = elevation; }

            //! Elevation set to null
            void resetElevation();

            //! Check if elevation is within radius and can be used
            bool isWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate) const;

            //! Get CG above ground
            const BlackMisc::PhysicalQuantities::CLength &getCGAboveGround() const { return m_cgAboveGround;}

            //! Set CG above ground
            void setCGAboveGround(const BlackMisc::PhysicalQuantities::CLength &cgAboveGround) { m_cgAboveGround = cgAboveGround; }

            //! VTOL aircraft?
            bool isVtolAircraft() const { return m_isVtol; }

            //! VTOL aircraft
            void setVtolAircraft(bool vtol) { m_isVtol = vtol; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! For debugging
            QString debugInfo(const BlackMisc::Geo::CElevationPlane &deltaElevation) const;

        private:
            bool m_isVtol = false; //!< VTOL aircraft?
            BlackMisc::Geo::CElevationPlane m_elevation; //!< aircraft's elevation if available
            BlackMisc::PhysicalQuantities::CLength m_cgAboveGround { 0, nullptr }; //!< center of gravity above ground

            BLACK_METACLASS(
                CInterpolationHints,
                BLACK_METAMEMBER(isVtol),
                BLACK_METAMEMBER(elevation),
                BLACK_METAMEMBER(cgAboveGround)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationHints)

#endif // guard
