// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONRENDERINGSETUP_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONRENDERINGSETUP_H

#include <QString>

#include "misc/aviation/callsign.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/propertyindexlist.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CInterpolationAndRenderingSetupGlobal)
SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CInterpolationAndRenderingSetupPerCallsign)

namespace swift::misc
{
    namespace network
    {
        class CClient;
    }
    namespace simulation
    {
        //! Value object for interpolator and rendering base class
        class SWIFT_MISC_EXPORT CInterpolationAndRenderingSetupBase
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexLogInterpolation = CPropertyIndexRef::GlobalIndexCInterpolatioRenderingSetup,
                IndexSimulatorDebugMessages,
                IndexForceFullInterpolation,
                IndexSendGndFlagToSimulator,
                IndexEnabledAircraftParts,
                IndexInterpolatorMode,
                IndexInterpolatorModeAsString,
                IndexFixSceneryOffset,
                IndexPitchOnGround
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
            bool setLogInterpolation(bool log);

            //! Full interpolation (skip optimizations like checking if aircraft moves etc.)
            bool isForcingFullInterpolation() const { return m_forceFullInterpolation; }

            //! Force full interpolation
            void setForceFullInterpolation(bool force) { m_forceFullInterpolation = force; }

            //! Set enabled aircraft parts
            bool setEnabledAircraftParts(bool enabled);

            //! Mask enabled aircraft parts
            bool maskEnabledAircraftParts(bool mask);

            //! Aircraft parts enabled (still requires the other aircraft to send parts)
            bool isAircraftPartsEnabled() const { return m_enabledAircraftParts; }

            //! Send GND flag to simulator
            bool isSendingGndFlagToSimulator() const { return m_sendGndToSim; }

            //! Set sending GND flag to simulator
            bool setSendingGndFlagToSimulator(bool sendFLag);

            //! Fix scenery offset if it has been detected
            bool isFixingSceneryOffset() const { return m_fixSceneryOffset; }

            //! Enable fix scenery offset if it has been detected
            void setFixingSceneryOffset(bool fix) { m_fixSceneryOffset = fix; }

            //! Force a given pitch on ground
            const physical_quantities::CAngle &getPitchOnGround() const { return m_pitchOnGround; }

            //! Force a given pitch on ground
            bool setPitchOnGround(const physical_quantities::CAngle &pitchOnGround);

            //! Consolidate with a network client
            void consolidateWithClient(const network::CClient &client);

            //! Interpolator mode
            InterpolatorMode getInterpolatorMode() const { return static_cast<InterpolatorMode>(m_interpolatorMode); }

            //! Interpolator mode
            const QString &getInterpolatorModeAsString() const { return modeToString(this->getInterpolatorMode()); }

            //! Set interpolator mode
            bool setInterpolatorMode(InterpolatorMode mode);

            //! Set interpolator mode
            bool setInterpolatorMode(const QString &mode);

            //! \copydoc swift::misc::mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! Interpolator mode as string
            static const QString &modeToString(InterpolatorMode mode);

        protected:
            //! Constructor
            CInterpolationAndRenderingSetupBase();

            //! Can handle index?
            static bool canHandleIndex(int index);

            bool m_logInterpolation = false; //!< Debug messages in interpolator
            bool m_simulatorDebugMessages = false; //!< Debug messages of simulator (aka plugin)
            bool m_forceFullInterpolation = false; //!< always do a full interpolation, even if aircraft is not moving
            bool m_enabledAircraftParts = true; //!< Enable aircraft parts
            bool m_sendGndToSim = true; //!< Send the gnd.flag to simulator
            bool m_fixSceneryOffset = false; //!< Fix. scenery offset
            int m_interpolatorMode = static_cast<int>(Spline); //!< interpolator mode (spline, ...)
            physical_quantities::CAngle m_pitchOnGround = physical_quantities::CAngle::null(); //!< pitch angle on ground
        };

        //! Value object for interpolator and rendering
        class SWIFT_MISC_EXPORT CInterpolationAndRenderingSetupGlobal :
            public CValueObject<CInterpolationAndRenderingSetupGlobal>,
            public CInterpolationAndRenderingSetupBase
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexMaxRenderedAircraft = CInterpolationAndRenderingSetupBase::IndexFixSceneryOffset + 1,
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
            bool setMaxRenderedDistance(const physical_quantities::CLength &distance);

            //! Disable
            void clearMaxRenderedDistance();

            //! Rendering enabled (at all)?
            bool isRenderingEnabled() const;

            //! Rendering enabled, but restricted
            bool isRenderingRestricted() const;

            //! Max.distance for rendering
            physical_quantities::CLength getMaxRenderedDistance() const { return m_maxRenderedDistance; }

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

            //! \copydoc swift::misc::mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        private:
            int m_maxRenderedAircraft = InfiniteAircraft(); //!< max.rendered aircraft
            physical_quantities::CLength m_maxRenderedDistance { 0, nullptr }; //!< max.distance for rendering

            SWIFT_METACLASS(
                CInterpolationAndRenderingSetupGlobal,
                SWIFT_METAMEMBER(logInterpolation),
                SWIFT_METAMEMBER(simulatorDebugMessages),
                SWIFT_METAMEMBER(forceFullInterpolation),
                SWIFT_METAMEMBER(sendGndToSim),
                SWIFT_METAMEMBER(enabledAircraftParts),
                SWIFT_METAMEMBER(fixSceneryOffset),
                SWIFT_METAMEMBER(interpolatorMode),
                SWIFT_METAMEMBER(pitchOnGround),
                SWIFT_METAMEMBER(maxRenderedAircraft),
                SWIFT_METAMEMBER(maxRenderedDistance));
        };

        //! Value object for interpolator and rendering per callsign
        class SWIFT_MISC_EXPORT CInterpolationAndRenderingSetupPerCallsign :
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
            CInterpolationAndRenderingSetupPerCallsign(const aviation::CCallsign &callsign, const CInterpolationAndRenderingSetupGlobal &globalSetup);

            //! Properties unequal to global setup
            CPropertyIndexList unequalToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const;

            //! Equal to global setup?
            bool isEqualToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const;

            //! Get callsign
            const aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Set callsign
            void setCallsign(const aviation::CCallsign &callsign) { m_callsign = callsign; }

            //! NULL?
            bool isNull() const { return m_callsign.isEmpty(); }

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! NULL object
            static const CInterpolationAndRenderingSetupPerCallsign &null();

        private:
            aviation::CCallsign m_callsign;

            SWIFT_METACLASS(
                CInterpolationAndRenderingSetupPerCallsign,
                SWIFT_METAMEMBER(logInterpolation),
                SWIFT_METAMEMBER(simulatorDebugMessages),
                SWIFT_METAMEMBER(forceFullInterpolation),
                SWIFT_METAMEMBER(sendGndToSim),
                SWIFT_METAMEMBER(enabledAircraftParts),
                SWIFT_METAMEMBER(fixSceneryOffset),
                SWIFT_METAMEMBER(interpolatorMode),
                SWIFT_METAMEMBER(pitchOnGround));
        };
    } // namespace simulation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign)
Q_DECLARE_METATYPE(swift::misc::simulation::CInterpolationAndRenderingSetupGlobal)
Q_DECLARE_METATYPE(swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode)

#endif // guard
