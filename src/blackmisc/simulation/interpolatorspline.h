// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORSPLINE_H
#define BLACKMISC_SIMULATION_INTERPOLATORSPLINE_H

#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolant.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Simulation
{
    //! Cubic spline interpolator
    class BLACKMISC_EXPORT CInterpolatorSpline : public CInterpolator<CInterpolatorSpline>
    {
        virtual void anchor() override;

    public:
        //! Constructor
        CInterpolatorSpline(const Aviation::CCallsign &callsign,
                            ISimulationEnvironmentProvider *envProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *aircraftProvider,
                            CInterpolationLogger *logger = nullptr) : CInterpolator(callsign, envProvider, setupProvider, aircraftProvider, logger) {}

        //! Position arrays for interpolation
        struct BLACKMISC_EXPORT PosArray
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
        class BLACKMISC_EXPORT CInterpolant : public IInterpolant
        {
        public:
            //! Default
            CInterpolant() : m_pa(PosArray::zeroPosArray()) {}

            //! Constructor
            CInterpolant(const PosArray &pa, const PhysicalQuantities::CLengthUnit &altitudeUnit, const CInterpolatorLinearPbh &pbh);

            //! Perform the interpolation
            //! \param situation situation used as a base for interpolation. Contains for example the already interpolated PBH.
            //! \return \p situation with interpolated position and altitude and updated timestamp
            std::tuple<Geo::CCoordinateGeodetic, Aviation::CAltitude> interpolatePositionAndAltitude() const;

            //! Interpolate the ground information/factor
            Aviation::COnGroundInfo interpolateGroundFactor() const;

            //! Set the time values
            void setTimes(qint64 currentTimeMs, double timeFraction, qint64 interpolatedTimeMs);

            //! \private UNIT tests/ASSERT only
            const PosArray &getPa() const { return m_pa; }

            const IInterpolatorPbh &pbh() const { return m_pbh; }

        private:
            PosArray m_pa; //!< current positions array, latest values last
            PhysicalQuantities::CLengthUnit m_altitudeUnit;
            qint64 m_currentTimeMsSinceEpoc { -1 };
            CInterpolatorLinearPbh m_pbh; //!< the used PBH interpolator
        };

        //! Strategy used by CInterpolator::getInterpolatedSituation
        CInterpolant getInterpolant(SituationLog &log);

    private:
        //! Update the elevations used in CInterpolatorSpline::m_s
        bool updateElevations(bool canSkip);

        //! Are any elevations missing in CInterpolatorSpline::m_s
        bool areAnyElevationsMissing() const;

        //! Ground relevant
        bool isAnySituationNearGroundRelevant() const;

        //! Fill the situations array
        bool fillSituationsArray();

        qint64 m_prevSampleAdjustedTime = 0; //!< previous sample time + offset
        qint64 m_nextSampleAdjustedTime = 0; //!< previous sample time + offset
        qint64 m_prevSampleTime = 0; //!< previous sample "real time"
        qint64 m_nextSampleTime = 0; //!< next sample "real time"
        std::array<Aviation::CAircraftSituation, 3> m_s; //!< used situations
        CInterpolant m_interpolant;
    };
} // ns

#endif
