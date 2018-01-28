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
#include "blackmisc/aviation/aircraftparts.h"

namespace BlackMisc
{
    namespace Aviation { class CAircraftSituation; }
    namespace Simulation
    {
        //! Hints for interpolator such as ground elevation
        class BLACKMISC_EXPORT CInterpolationHints :
            public CValueObject<CInterpolationHints>
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

            //! Constructor
            CInterpolationHints(bool isVtolAircraft, bool hasParts, bool log);

            //! Get elevation plane
            const Geo::CElevationPlane &getElevationPlane() const { return m_elevationPlane;}

            //! Set elevation
            //! \remark used to store a ground elevation and use it as well for nearby situatons
            void setElevationPlane(const Geo::CElevationPlane &elevation) { m_elevationPlane = elevation; }

            //! Elevation plane set to null
            void resetElevationPlane();

            //! Get elevation from CInterpolationHints::getElevationProvider or CInterpolationHints::getElevation
            //! \remark avoid unnecessary calls on XPlane (calling elevation provider)
            //! @param situation
            //! @param useProvider using the provider if available
            //! @param forceProvider use the provider and ignore any plane
            //! \see setElevationProvider
            //! \see setElevationPlane
            Aviation::CAltitude getGroundElevation(const Aviation::CAircraftSituation &situation, bool useProvider, bool forceProvider = false) const;

            //! Get elevation from CInterpolationHints::getElevationProvider or CInterpolationHints::getElevation
            //! \remark if validRadius is >= Geo::CElevationPlane::radius use validRadius
            Aviation::CAltitude getGroundElevation(const Aviation::CAircraftSituation &situation, const PhysicalQuantities::CLength &validRadius, bool useProvider, bool forceProvider = false) const;

            //! Check if elevation is within radius and can be used
            bool isWithinRange(const Geo::ICoordinateGeodetic &coordinate) const;

            //! Get CG above ground
            const PhysicalQuantities::CLength &getCGAboveGround() const { return m_cgAboveGround;}

            //! Get CG above ground or 0 ("Zero")
            const PhysicalQuantities::CLength &getCGAboveGroundOrZero() const;

            //! Has CG above ground
            bool hasCGAboveGround() const { return m_cgAboveGround.isNull(); }

            //! Set CG above ground
            void setCGAboveGround(const PhysicalQuantities::CLength &cgAboveGround) { m_cgAboveGround = cgAboveGround; }

            //! VTOL aircraft?
            bool isVtolAircraft() const { return m_isVtol; }

            //! VTOL aircraft
            void setVtolAircraft(bool vtol) { m_isVtol = vtol; }

            //! Log interpolation?
            bool isLoggingInterpolation() const { return m_logInterpolation; }

            //! Log interpolation?
            void setLoggingInterpolation(bool log) { m_logInterpolation = log; }

            //! Has valid aircraft parts?
            bool hasAircraftParts() const { return m_hasParts; }

            //! Aircraft parts required for interpolation
            Aviation::CAircraftParts getAircraftParts() const { return m_aircraftParts; }

            //! Set aircraft parts required for interpolation if any
            //! \remark when used to reset value (default object) use hasParts false. Needed as there is no null parts object
            void setAircraftParts(const Aviation::CAircraftParts &parts, bool hasParts = true);

            //! Function object that can obtain ground elevation
            using ElevationProvider = std::function<Aviation::CAltitude(const Aviation::CAircraftSituation &)>;

            //! Has elevation provider
            bool hasElevationProvider() const;

            //! Set function object that can obtain ground elevation
            //! \remark either a provider can be used or an elevation plan can be set
            //! \see setElevationPlane
            //! \see getGroundElevation
            void setElevationProvider(const ElevationProvider &ep) { m_elevationProvider = ep; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! For debugging
            QString debugInfo(const Geo::CElevationPlane &deltaElevation) const;

        private:
            bool m_isVtol = false;           //!< VTOL aircraft?
            bool m_hasParts = false;         //!< Has valid aircraft parts?
            bool m_logInterpolation = false; //!< log interpolation
            Aviation::CAircraftParts    m_aircraftParts;     //!< Aircraft parts
            Geo::CElevationPlane        m_elevationPlane;    //!< aircraft's elevation if available
            ElevationProvider           m_elevationProvider; //!< Provider of ground elevation (lazy computation)
            PhysicalQuantities::CLength m_cgAboveGround { 0, nullptr }; //!< center of gravity above ground

            BLACK_METACLASS(
                CInterpolationHints,
                BLACK_METAMEMBER(isVtol),
                BLACK_METAMEMBER(elevationPlane),
                BLACK_METAMEMBER(cgAboveGround),
                BLACK_METAMEMBER(hasParts),
                BLACK_METAMEMBER(aircraftParts),
                BLACK_METAMEMBER(logInterpolation)
                // elevationProvider not included
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationHints)

#endif // guard
