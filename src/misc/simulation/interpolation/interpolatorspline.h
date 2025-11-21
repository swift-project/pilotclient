// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORSPLINE_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORSPLINE_H

#include "misc/aviation/aircraftsituation.h"
#include "misc/simulation/interpolation/interpolant.h"
#include "misc/simulation/interpolation/interpolationlogger.h"
#include "misc/simulation/interpolation/interpolator.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Cubic spline interpolator
    class SWIFT_MISC_EXPORT CInterpolatorSpline : public CInterpolator
    {
        void anchor() override;

    public:
        //! Constructor
        CInterpolatorSpline(const aviation::CCallsign &callsign, ISimulationEnvironmentProvider *envProvider,
                            IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *aircraftProvider,
                            CInterpolationLogger *logger = nullptr)
            : CInterpolator(callsign, envProvider, setupProvider, aircraftProvider, logger)
        {}

        //! Position arrays for interpolation
        struct SWIFT_MISC_EXPORT PosArray
        {
            //! Init all values to zero
            void initToZero();

            //! Zero initialized position array
            static const PosArray &zeroPosArray();

            //! @{
            //! 3 coordinates for spline interpolation
            std::array<double, 3> x;
            std::array<double, 3> y;
            std::array<double, 3> z;
            std::array<double, 3> a;
            std::array<double, 3> gnd;
            std::array<double, 3> t;
            std::array<double, 3> dx;
            std::array<double, 3> dy;
            std::array<double, 3> dz;
            std::array<double, 3> da;
            std::array<double, 3> dgnd;
            //! @}

            //! Array size
            int size() const { return static_cast<int>(x.size()); }
        };

        //! Cubic function that performs the actual interpolation
        class SWIFT_MISC_EXPORT CInterpolant : public IInterpolant
        {
        public:
            //! Default
            CInterpolant() : m_pa(PosArray::zeroPosArray()) {}

            //! Constructor
            CInterpolant(const PosArray &pa, const physical_quantities::CLengthUnit &altitudeUnit,
                         const CInterpolatorLinearPbh &pbh);

            //! \copydoc swift::misc::simulation::IInterpolant::interpolatePositionAndAltitude
            std::tuple<geo::CCoordinateGeodetic, aviation::CAltitude> interpolatePositionAndAltitude() const override;

            //! \copydoc swift::misc::simulation::IInterpolant::interpolateGroundFactor
            aviation::COnGroundInfo interpolateGroundFactor() const override;

            //! Set the time values
            void setTimes(qint64 currentTimeMs, double timeFraction, qint64 interpolatedTimeMs);

            //! \private UNIT tests/ASSERT only
            const PosArray &getPa() const { return m_pa; }

            //! \copydoc swift::misc::simulation::IInterpolant::pbh
            const IInterpolatorPbh &pbh() const override { return m_pbh; }

        private:
            PosArray m_pa; //!< current positions array, latest values last
            physical_quantities::CLengthUnit m_altitudeUnit;
            qint64 m_currentTimeMsSinceEpoc { -1 };
            CInterpolatorLinearPbh m_pbh; //!< the used PBH interpolator
        };

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolant
        const IInterpolant &getInterpolant(SituationLog &log) override;

    private:
        //! Update the elevations used in CInterpolatorSpline::m_s
        bool updateElevations(bool canSkip);

        //! Are any elevations missing in CInterpolatorSpline::m_s
        bool areAnyElevationsMissing() const;

        //! Ground relevant
        bool isAnySituationNearGroundRelevant() const;

        //! Fill the situations array
        bool fillSituationsArray();

        //! Verify gnd flag, times, ... true means "OK"
        static bool verifyInterpolationSituations(const aviation::CAircraftSituation &oldest,
                                                  const aviation::CAircraftSituation &newer,
                                                  const aviation::CAircraftSituation &latest,
                                                  const CInterpolationAndRenderingSetupPerCallsign &setup =
                                                      CInterpolationAndRenderingSetupPerCallsign::null());

        qint64 m_prevSampleAdjustedTime = 0; //!< previous sample time + offset
        qint64 m_nextSampleAdjustedTime = 0; //!< previous sample time + offset
        qint64 m_prevSampleTime = 0; //!< previous sample "real time"
        qint64 m_nextSampleTime = 0; //!< next sample "real time"
        std::array<aviation::CAircraftSituation, 3> m_s; //!< used situations
        CInterpolant m_interpolant;
    };
} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORSPLINE_H
