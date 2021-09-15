/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORSPLINE_H
#define BLACKMISC_SIMULATION_INTERPOLATORSPLINE_H

#include "interpolator.h"
#include "interpolationlogger.h"
#include "interpolant.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QtGlobal>

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
                            CInterpolationLogger *logger = nullptr) :
            CInterpolator(callsign, envProvider, setupProvider, aircraftProvider, logger) {}

        //! Position arrays for interpolation
        struct BLACKMISC_EXPORT PosArray
        {
            //! Init all values to zero
            void initToZero();

            //! Zero initialized position array
            static const PosArray &zeroPosArray();

            //! 3 coordinates for spline interpolation
            //! @{
            std::array<double, 3> x, y, z, a, gnd, t, dx, dy, dz, da, dgnd;

            //! Array size
            int size() const { return static_cast<int>(x.size()); }
            //! @}
        };

        //! Cubic function that performs the actual interpolation
        class BLACKMISC_EXPORT CInterpolant : public IInterpolant
        {
        public:
            //! Default
            CInterpolant() : m_pa(PosArray::zeroPosArray()) {}

            //! Constructor
            CInterpolant(const PosArray &pa, const PhysicalQuantities::CLengthUnit &altitudeUnit, const CInterpolatorPbh &pbh);

            //! Perform the interpolation
            Aviation::CAircraftSituation interpolatePositionAndAltitude(const Aviation::CAircraftSituation &currentSituation, bool interpolateGndFactor) const;

            //! Old situation
            const Aviation::CAircraftSituation &getOldSituation() const { return pbh().getOldSituation(); }

            //! New situation
            const Aviation::CAircraftSituation &getNewSituation() const { return pbh().getNewSituation(); }

            //! Set the time values
            void setTimes(qint64 currentTimeMs, double timeFraction, qint64 interpolatedTimeMs);

            //! \private UNIT tests/ASSERT only
            const PosArray &getPa() const { return m_pa; }

        private:
            PosArray m_pa; //!< current positions array, latest values last
            PhysicalQuantities::CLengthUnit m_altitudeUnit;
            qint64 m_currentTimeMsSinceEpoc { -1 };
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
