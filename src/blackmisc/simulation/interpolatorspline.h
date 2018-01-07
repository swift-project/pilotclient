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
            CInterpolatorSpline(const BlackMisc::Aviation::CCallsign &callsign, QObject *parent = nullptr) :
                CInterpolator("CInterpolatorSpline", callsign, parent)
            {}

            //! Cubic function that performs the actual interpolation
            class Interpolant
            {
            public:
                //! Constructor
                Interpolant(const CInterpolatorSpline &interpolator, qint64 time) : i(interpolator), currentTimeMsSinceEpoc(time) {}

                //! Perform the interpolation
                //! @{
                Geo::CCoordinateGeodetic interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                Aviation::CAltitude interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                //! @}

                //! Interpolator for pitch, bank, heading, groundspeed
                CInterpolatorPbh pbh() const { return i.m_pbh; }

            private:
                const CInterpolatorSpline &i;
                qint64 currentTimeMsSinceEpoc = 0;
            };

            //! Strategy used by CInterpolator::getInterpolatedSituation
            Interpolant getInterpolant(
                qint64 currentTimeMsSinceEpoc, const CInterpolationAndRenderingSetup &setup,
                const CInterpolationHints &hints, CInterpolationStatus &status, CInterpolationLogger::SituationLog &log);

        private:
            qint64 m_prevSampleTime = 0;
            qint64 m_nextSampleTime = 0;
            PhysicalQuantities::CLengthUnit m_altitudeUnit;
            std::array<double, 3> x, y, z, a, t, dx, dy, dz, da;
            CInterpolatorPbh m_pbh;
        };
    }
}

#endif
