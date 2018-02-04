/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORSPLINE_H
#define BLACKMISC_SIMULATION_INTERPOLATORSPLINE_H

#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Cubic spline interpolator
        class BLACKMISC_EXPORT CInterpolatorSpline : public CInterpolator<CInterpolatorSpline>
        {
            Q_OBJECT

        public:
            //! Constructor
            CInterpolatorSpline(const Aviation::CCallsign &callsign, QObject *parent = nullptr) :
                CInterpolator("CInterpolatorSpline", callsign, parent)
            {}

            //! Position arrays for interpolation
            struct PosArray
            {
                //! Init all values to zero
                void initToZero();

                //! Zero initialized position array
                static const PosArray &zeroPosArray();

                //! 3 coordinates for spline interpolation
                //! @{
                std::array<double, 3> x, y, z, a, t, dx, dy, dz, da;
                //! @}
            };

            //! Cubic function that performs the actual interpolation
            class Interpolant
            {
            public:
                //! Default
                Interpolant() : m_pa(PosArray::zeroPosArray()) {}

                //! Constructor
                Interpolant(
                    const PosArray &pa, const PhysicalQuantities::CLengthUnit &altitudeUnit, const CInterpolatorPbh &pbh) :
                    m_pa(pa), m_altitudeUnit(altitudeUnit), m_pbh(pbh) {}

                //! Perform the interpolation
                //! @{
                Geo::CCoordinateGeodetic interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                Aviation::CAltitude interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                //! @}

                //! Interpolator for pitch, bank, heading, groundspeed
                const CInterpolatorPbh &pbh() const { return m_pbh; }

                //! Old situation
                const Aviation::CAircraftSituation &getOldSituation() const { return pbh().getOldSituation(); }

                //! New situation
                const Aviation::CAircraftSituation &getNewSituation() const { return pbh().getNewSituation(); }

                //! "Real time" representing the interpolated situation
                qint64 getInterpolatedTime() const { return m_interpolatedTime; }

                //! Set the time values
                void setTimes(qint64 currentTimeMs, double timeFraction, qint64 interpolatedTimeMs);

            private:
                PosArray m_pa;
                PhysicalQuantities::CLengthUnit m_altitudeUnit;
                CInterpolatorPbh m_pbh;
                qint64 m_currentTimeMsSinceEpoc { 0 };
                qint64 m_interpolatedTime { 0 }; //!< represented "real time" at interpolated situation
            };

            //! Strategy used by CInterpolator::getInterpolatedSituation
            Interpolant getInterpolant(
                qint64 currentTimeMsSinceEpoc,
                const CInterpolationAndRenderingSetup &setup,
                const CInterpolationHints &hints, CInterpolationStatus &status, SituationLog &log);

        private:
            qint64 m_prevSampleAdjustedTime = 0; //!< previous sample time + offset
            qint64 m_nextSampleAdjustedTime = 0; //!< previous sample time + offset
            qint64 m_prevSampleTime = 0; //!< previous sample "real time"
            qint64 m_nextSampleTime = 0; //!< next sample "real time"
            std::array<Aviation::CAircraftSituation, 3> m_s;
            Interpolant m_interpolant;
        };
    } // ns
} // ns

#endif
