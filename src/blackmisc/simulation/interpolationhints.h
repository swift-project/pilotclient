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

            //! Constructor
            CInterpolationHints(bool isVtolAircraft, bool hasParts, bool log);

            //! Get elevation
            const BlackMisc::Geo::CElevationPlane &getElevationPlane() const { return m_elevationPlane;}

            //! Set elevation
            void setElevationPlane(const BlackMisc::Geo::CElevationPlane &elevation) { m_elevationPlane = elevation; }

            //! Elevation plane set to null
            void resetElevationPlane();

            //! Get elevation from CInterpolationHints::getElevationProvider or CInterpolationHints::getElevation
            Aviation::CAltitude getGroundElevation(const BlackMisc::Aviation::CAircraftSituation &situation) const;

            //! Check if elevation is within radius and can be used
            bool isWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate) const;

            //! Get CG above ground
            const BlackMisc::PhysicalQuantities::CLength &getCGAboveGround() const { return m_cgAboveGround;}

            //! Has CG above ground
            bool hasCGAboveGround() const { return m_cgAboveGround.isNull(); }

            //! Set CG above ground
            void setCGAboveGround(const BlackMisc::PhysicalQuantities::CLength &cgAboveGround) { m_cgAboveGround = cgAboveGround; }

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
            BlackMisc::Aviation::CAircraftParts getAircraftParts() const { return m_aircraftParts; }

            //! Set aircraft parts required for interpolation if any
            //! \remark when used to reset value (default object) use hasParts false. Needed as there is no null parts object
            void setAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts, bool hasParts = true);

            //! Function object that can obtain ground elevation
            using ElevationProvider = std::function<BlackMisc::Aviation::CAltitude(const BlackMisc::Aviation::CAircraftSituation &)>;

            //! Has elevation provider
            bool hasElevationProvider() const;

            //! Function object that can obtain ground elevation
            //! \remark either a provider or a value set can be used
            const ElevationProvider &getElevationProvider() const { return m_elevationProvider; }

            //! Set function object that can obtain ground elevation
            void setElevationProvider(const ElevationProvider &ep) { m_elevationProvider = ep; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! For debugging
            QString debugInfo(const BlackMisc::Geo::CElevationPlane &deltaElevation) const;

        private:
            bool m_isVtol = false;           //!< VTOL aircraft?
            bool m_hasParts = false;         //!< Has valid aircraft parts?
            bool m_logInterpolation = false; //!< log interpolation
            BlackMisc::Aviation::CAircraftParts    m_aircraftParts;     //!< Aircraft parts
            BlackMisc::Geo::CElevationPlane        m_elevationPlane;    //!< aircraft's elevation if available
            ElevationProvider                      m_elevationProvider; //!< Provider of ground elevation (lazy computation)
            BlackMisc::PhysicalQuantities::CLength m_cgAboveGround { 0, nullptr }; //!< center of gravity above ground

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
