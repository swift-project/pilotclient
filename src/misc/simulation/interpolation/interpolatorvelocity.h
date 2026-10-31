// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORVELOCITY_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORVELOCITY_H

#include <QtGlobal>

#include "misc/aviation/aircraftsituation.h"
#include "misc/simulation/interpolation/interpolant.h"
#include "misc/simulation/interpolation/interpolationlogger.h"
#include "misc/simulation/interpolation/interpolator.h"
#include "misc/simulation/interpolation/interpolatorlinear.h"
#include "misc/swiftmiscexport.h"

class QObject;

namespace swift::misc
{
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        //! Velocity based extrapolating interpolator, dead reckoning from the 6DOF velocity of the latest fix.
        //! \remark implements swift issue #241: position and rotation are extrapolated from the velocity of the
        //! latest visual position update. The extrapolation error is smoothly converged with each new fix
        //! (error vector added to the velocity for some time), so the rendered position is continuous
        //! (no warping / "zoomies"). Without velocity data the interpolator falls back to linear interpolation.
        class SWIFT_MISC_EXPORT CInterpolatorVelocity : public CInterpolator
        {
            void anchor() override;

        public:
            //! Constructor
            CInterpolatorVelocity(const aviation::CCallsign &callsign, ISimulationEnvironmentProvider *simEnvProvider,
                                  IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteProvider,
                                  CInterpolationLogger *logger = nullptr)
                : CInterpolator(callsign, simEnvProvider, setupProvider, remoteProvider, logger)
            {}

            //! Interpolant returning an already extrapolated situation
            class SWIFT_MISC_EXPORT CInterpolant : public IInterpolant
            {
            public:
                //! @{
                //! Constructor
                CInterpolant() = default;
                CInterpolant(const aviation::CAircraftSituation &situation, const aviation::CHeading &heading,
                             const physical_quantities::CAngle &pitch, const physical_quantities::CAngle &bank,
                             const physical_quantities::CSpeed &groundspeed, qint64 interpolatedTime);
                //! @}

                //! \copydoc swift::misc::simulation::IInterpolant::interpolatePositionAndAltitude
                std::tuple<geo::CCoordinateGeodetic, aviation::CAltitude>
                interpolatePositionAndAltitude() const override;

                //! \copydoc swift::misc::simulation::IInterpolant::interpolateGroundFactor
                aviation::COnGroundInfo interpolateGroundFactor() const override;

                //! \copydoc swift::misc::simulation::IInterpolant::pbh
                const IInterpolatorPbh &pbh() const override { return m_pbh; }

            private:
                //! PBH holder returning pre-extrapolated fixed values
                class CPbhFixed : public IInterpolatorPbh
                {
                public:
                    CPbhFixed() = default;
                    CPbhFixed(const aviation::CAircraftSituation &situation, const aviation::CHeading &heading,
                              const physical_quantities::CAngle &pitch, const physical_quantities::CAngle &bank,
                              const physical_quantities::CSpeed &groundspeed)
                        : m_situation(situation), m_heading(heading), m_pitch(pitch), m_bank(bank),
                          m_groundspeed(groundspeed)
                    {}

                    aviation::CHeading getHeading() const override { return m_heading; }
                    physical_quantities::CAngle getPitch() const override { return m_pitch; }
                    physical_quantities::CAngle getBank() const override { return m_bank; }
                    physical_quantities::CSpeed getGroundSpeed() const override { return m_groundspeed; }
                    const aviation::CAircraftSituation &getStartSituation() const override { return m_situation; }
                    const aviation::CAircraftSituation &getEndSituation() const override { return m_situation; }

                private:
                    aviation::CAircraftSituation m_situation;
                    aviation::CHeading m_heading { 0, nullptr };
                    physical_quantities::CAngle m_pitch { 0, nullptr };
                    physical_quantities::CAngle m_bank { 0, nullptr };
                    physical_quantities::CSpeed m_groundspeed { 0, nullptr };
                };

                aviation::CAircraftSituation m_situation;
                CPbhFixed m_pbh;
            };

            //! \copydoc swift::misc::simulation::CInterpolator::getInterpolant
            const IInterpolant &getInterpolant(SituationLog &log) override;

            //! Velocity data available for this aircraft (latest situations carry 6DOF velocity)?
            //! \remark this is the proof that the network delivers visual positions for this aircraft
            bool hasVelocityData() const;

        private:
            //! Extrapolated values for one render time
            struct SExtrapolated
            {
                double latitudeDeg = 0.0;
                double longitudeDeg = 0.0;
                double altitudeM = 0.0;
                double pitchDeg = 0.0;
                double bankDeg = 0.0;
                double headingDeg = 0.0;
                qint64 realTimeMs = -1; //!< "real" (offset free) time of the extrapolated situation
            };

            //! Correction segment, valid from its time on, decaying exponentially
            struct SCorrectionSegment
            {
                qint64 timeMs = -1; //!< adjusted time the segment was established for
                double eastM = 0.0; //!< meters east
                double northM = 0.0; //!< meters north
                double upM = 0.0; //!< meters up
                double pitchDeg = 0.0;
                double bankDeg = 0.0;
                double headingDeg = 0.0;
                bool isValid() const { return timeMs >= 0; }
            };

            //! Consume new situations: update the anchor and establish a new correction segment
            void updateAnchorAndCorrection();

            //! Extrapolate the given anchor situation to the given adjusted time, correction included
            SExtrapolated extrapolateFrom(const aviation::CAircraftSituation &anchor, qint64 adjustedTimeMs) const;

            //! Correction valid at the given adjusted time (segment selection), in meters and degrees
            void correctionAt(qint64 adjustedTimeMs, const SCorrectionSegment **segment, double *factor) const;

            //! No usable velocity data: linear interpolation as fallback
            const IInterpolant &getFallbackInterpolant(SituationLog &log);

            CInterpolant m_interpolant; //!< current extrapolated interpolant
            CInterpolatorLinear::CInterpolant m_fallbackInterpolant; //!< fallback linear interpolant

            aviation::CAircraftSituation m_anchor; //!< latest situation with usable velocity, empty if none
            SCorrectionSegment m_corrPrev; //!< correction segment before the current one
            SCorrectionSegment m_corrCurr; //!< correction segment established with the latest fix
        };
    } // namespace simulation
} // namespace swift::misc

#endif // SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORVELOCITY_H
