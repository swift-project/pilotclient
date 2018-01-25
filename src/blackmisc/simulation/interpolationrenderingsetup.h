/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONRENDERINGSETUP_H
#define BLACKMISC_SIMULATION_INTERPOLATIONRENDERINGSETUP_H

#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Value object for interpolator and rendering
         */
        class BLACKMISC_EXPORT CInterpolationAndRenderingSetup :
            public CValueObject<CInterpolationAndRenderingSetup>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexInterpolatorDebugMessages = CPropertyIndex::GlobalIndexCInterpolatioRenderingSetup,
                IndexSimulatorDebugMessages,
                IndexForceFullInterpolation,
                IndexMaxRenderedAircraft,
                IndexMaxRenderedDistance,
                IndexEnabledAircraftParts
            };

            //! Constructor.
            CInterpolationAndRenderingSetup();

            //! Considered as "all aircraft"
            static int InfiniteAircraft();

            //! Debugging messages
            bool showSimulatorDebugMessages() const { return m_simulatorDebugMessages; }

            //! Debugging messages
            void setDriverDebuggingMessages(bool debug) { m_simulatorDebugMessages = debug; }

            //! Full interpolation
            bool isForcingFullInterpolation() const { return m_forceFullInterpolation; }

            //! Force full interpolation
            void setForceFullInterpolation(bool force) { m_forceFullInterpolation = force; }

            //! Max. number of aircraft rendered
            int getMaxRenderedAircraft() const;

            //! Max. number of aircraft rendered
            bool setMaxRenderedAircraft(int maxRenderedAircraft);

            //! Max. distance for rendering
            bool setMaxRenderedDistance(const PhysicalQuantities::CLength &distance);

            //! Set enabled aircraft parts
            bool setEnabledAircraftParts(bool enabled);

            //! Disable
            void clearMaxRenderedDistance();

            //! Rendering enabled (at all)
            bool isRenderingEnabled() const;

            //! Rendering enabled, but restricted
            bool isRenderingRestricted() const;

            //! Aircraft parts enabled
            bool isAircraftPartsEnabled() const;

            //! Max.distance for rendering
            PhysicalQuantities::CLength getMaxRenderedDistance() const { return m_maxRenderedDistance; }

            //! Restricted by distance?
            bool isMaxDistanceRestricted() const;

            //! Restricted by quantity?
            bool isMaxAircraftRestricted() const;

            //! Remove all render restrictions
            void clearAllRenderingRestrictions();

            //! Entirely disable rendering
            void disableRendering();

            //! Text describing the restrictions
            QString getRenderRestrictionText() const;

            //! Add a callsign which will be logged
            void addCallsignToLog(const Aviation::CCallsign &callsign);

            //! Remove a callsign from logging
            void removeCallsignFromLog(const Aviation::CCallsign &callsign);

            //! Clear all interpolator log callsigns
            void clearInterpolatorLogCallsigns();

            //! Callsigns for logging
            BlackMisc::Aviation::CCallsignSet getLogCallsigns() const;

            //! Log the given callsign?
            bool logCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

        private:
            bool m_interpolatorDebugMessage = false; //! Debug messages in interpolator
            bool m_simulatorDebugMessages   = false; //! Debug messages of simulator (aka plugin)
            bool m_forceFullInterpolation   = false; //! always do a full interpolation, even if aircraft is not moving
            bool m_enabledAircraftParts     = true;  //! Update aircraft parts
            int  m_maxRenderedAircraft = InfiniteAircraft(); //!< max.rendered aircraft
            PhysicalQuantities::CLength m_maxRenderedDistance { 0, nullptr }; //!< max.distance for rendering
            Aviation::CCallsignSet m_callsignsToLog;

            BLACK_METACLASS(
                CInterpolationAndRenderingSetup,
                BLACK_METAMEMBER(interpolatorDebugMessage),
                BLACK_METAMEMBER(simulatorDebugMessages),
                BLACK_METAMEMBER(forceFullInterpolation),
                BLACK_METAMEMBER(enabledAircraftParts),
                BLACK_METAMEMBER(maxRenderedAircraft),
                BLACK_METAMEMBER(maxRenderedDistance),
                BLACK_METAMEMBER(callsignsToLog, 0, DisabledForComparison)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationAndRenderingSetup)

#endif // guard
