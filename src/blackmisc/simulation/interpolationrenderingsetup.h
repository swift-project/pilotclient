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

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/valueobject.h"
#include <QString>

namespace BlackMisc
{
    namespace Network { class CClient; }
    namespace Simulation
    {
        //! Value object for interpolator and rendering base class
        class BLACKMISC_EXPORT CInterpolationAndRenderingSetupBase
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexLogInterpolation = CPropertyIndex::GlobalIndexCInterpolatioRenderingSetup,
                IndexSimulatorDebugMessages,
                IndexForceFullInterpolation,
                IndexSendGndFlagToSimulator,
                IndexEnableGndFlag,
                IndexEnabledAircraftParts,
                IndexInterpolatorMode
            };

            //! Interpolator type
            enum InterpolatorMode
            {
                Spline,
                Linear
            };

            //! Debugging messages for simulation
            bool showSimulatorDebugMessages() const { return m_simulatorDebugMessages; }

            //! Debugging messages for simulation
            void setSimulatorDebuggingMessages(bool debug) { m_simulatorDebugMessages = debug; }

            //! Log.interpolation
            bool logInterpolation() const { return m_logInterpolation; }

            //! Log.interpolation
            void setLogInterpolation(bool log) { m_logInterpolation = log; }

            //! Full interpolation
            bool isForcingFullInterpolation() const { return m_forceFullInterpolation; }

            //! Force full interpolation
            void setForceFullInterpolation(bool force) { m_forceFullInterpolation = force; }

            //! Set enabled aircraft parts
            bool setEnabledAircraftParts(bool enabled);

            //! Mask enabled aircraft parts
            bool maskEnabledAircraftParts(bool mask);

            //! Aircraft parts enabled
            bool isAircraftPartsEnabled() const { return m_enabledAircraftParts; }

            //! Set gnd flag enabled
            bool setEnabledGndFLag(bool enabled);

            //! Aircraft parts enabled
            bool isGndFlagEnabled() const { return m_enabledGndFlag; }

            //! Send GND flag to simulator
            bool sendGndFlagToSimulator() const { return m_sendGndToSim; }

            //! Set sending
            bool setSendGndFlagToSimulator(bool sendFLag);

            //! Consolidate with a network client
            void consolidateWithClient(const Network::CClient &client);

            //! Interpolator mode
            InterpolatorMode getInterpolatorMode() const { return static_cast<InterpolatorMode>(m_interpolatorMode);  }

            //! Interpolator mode
            const QString &getInterpolatorModeAsString() const { return modeToString(this->getInterpolatorMode()); }

            //! Set interpolator mode
            bool setInterpolatorMode(InterpolatorMode mode);

            //! Set interpolator mode
            bool setInterpolatorMode(const QString &mode);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Interpolator mode as string
            static const QString &modeToString(InterpolatorMode mode);

        protected:
            //! Constructor
            CInterpolationAndRenderingSetupBase();

            //! Can handle index?
            static bool canHandleIndex(int index);

            bool m_logInterpolation       = false; //!< Debug messages in interpolator
            bool m_simulatorDebugMessages = false; //!< Debug messages of simulator (aka plugin)
            bool m_forceFullInterpolation = false; //!< always do a full interpolation, even if aircraft is not moving
            bool m_enabledAircraftParts   = true;  //!< Enable aircraft parts
            bool m_enabledGndFlag         = true;  //!< Enable gnd.flag
            bool m_sendGndToSim           = true;  //!< Send the gnd.flag to simulator
            int  m_interpolatorMode       = static_cast<int>(Spline); //!< interpolator mode (spline, ...)
        };

        //! Value object for interpolator and rendering
        class BLACKMISC_EXPORT CInterpolationAndRenderingSetupGlobal :
            public CValueObject<CInterpolationAndRenderingSetupGlobal>,
            public CInterpolationAndRenderingSetupBase
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexMaxRenderedAircraft = CInterpolationAndRenderingSetupBase::IndexEnabledAircraftParts + 1,
                IndexMaxRenderedDistance
            };

            //! Constructor.
            CInterpolationAndRenderingSetupGlobal();

            //! Considered as "all aircraft"
            static int InfiniteAircraft();

            //! Max.number of aircraft rendered
            int getMaxRenderedAircraft() const;

            //! Max.number of aircraft rendered
            bool setMaxRenderedAircraft(int maxRenderedAircraft);

            //! Max.distance for rendering
            bool setMaxRenderedDistance(const PhysicalQuantities::CLength &distance);

            //! Disable
            void clearMaxRenderedDistance();

            //! Rendering enabled (at all)?
            bool isRenderingEnabled() const;

            //! Rendering enabled, but restricted
            bool isRenderingRestricted() const;

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

            //! Set all base values
            void setBaseValues(const CInterpolationAndRenderingSetupBase &baseValues);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

        private:
            int m_maxRenderedAircraft = InfiniteAircraft(); //!< max.rendered aircraft
            PhysicalQuantities::CLength m_maxRenderedDistance { 0, nullptr }; //!< max.distance for rendering

            BLACK_METACLASS(
                CInterpolationAndRenderingSetupGlobal,
                BLACK_METAMEMBER(logInterpolation),
                BLACK_METAMEMBER(simulatorDebugMessages),
                BLACK_METAMEMBER(forceFullInterpolation),
                BLACK_METAMEMBER(sendGndToSim),
                BLACK_METAMEMBER(enabledAircraftParts),
                BLACK_METAMEMBER(enabledGndFlag),
                BLACK_METAMEMBER(interpolatorMode),
                BLACK_METAMEMBER(maxRenderedAircraft),
                BLACK_METAMEMBER(maxRenderedDistance)
            );
        };

        //! Value object for interpolator and rendering per callsign
        class BLACKMISC_EXPORT CInterpolationAndRenderingSetupPerCallsign :
            public CValueObject<CInterpolationAndRenderingSetupPerCallsign>,
            public CInterpolationAndRenderingSetupBase
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = CInterpolationAndRenderingSetupGlobal::IndexMaxRenderedDistance + 1
            };

            //! Constructor
            CInterpolationAndRenderingSetupPerCallsign();

            //! Constructor from global setup
            CInterpolationAndRenderingSetupPerCallsign(const Aviation::CCallsign &callsign, const CInterpolationAndRenderingSetupGlobal &globalSetup);

            //! Properties unequal to global setup
            CPropertyIndexList unequalToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const;

            //! Equal to global setup?
            bool isEqualToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const;

            //! Get callsign
            const Aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Set callsign
            void setCallsign(const Aviation::CCallsign &callsign) { m_callsign = callsign; }

            //! NULL?
            bool isNull() const { return m_callsign.isEmpty(); }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! NULL object
            static const CInterpolationAndRenderingSetupPerCallsign &null();

        private:
            Aviation::CCallsign m_callsign;

            BLACK_METACLASS(
                CInterpolationAndRenderingSetupPerCallsign,
                BLACK_METAMEMBER(logInterpolation),
                BLACK_METAMEMBER(simulatorDebugMessages),
                BLACK_METAMEMBER(forceFullInterpolation),
                BLACK_METAMEMBER(sendGndToSim),
                BLACK_METAMEMBER(enabledAircraftParts),
                BLACK_METAMEMBER(enabledGndFlag),
                BLACK_METAMEMBER(interpolatorMode)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode)

#endif // guard
