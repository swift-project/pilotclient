/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H
#define BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H

#include "interpolator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QtGlobal>

class QObject;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation
    {
        //! Linear interpolator, calculation inbetween positions
        class BLACKMISC_EXPORT CInterpolatorLinear : public CInterpolator<CInterpolatorLinear>
        {
            Q_OBJECT

        public:
            //! Constructor
            CInterpolatorLinear(const BlackMisc::Aviation::CCallsign &callsign, QObject *parent = nullptr) :
                CInterpolator("CInterpolatorLinear", callsign, parent)
            {}

            //! Linear function that performs the actual interpolation
            class Interpolant
            {
            public:
                //! Constructor
                //! @{
                Interpolant(const Aviation::CAircraftSituation &situation) :
                    m_situationsAvailable(1), m_oldSituation(situation) {}
                Interpolant(const Aviation::CAircraftSituation &situation1, const Aviation::CAircraftSituation &situation2, double time) :
                    m_situationsAvailable(2), m_oldSituation(situation1), m_newSituation(situation2), m_simulationTimeFraction(time) {}
                //! @}

                //! Perform the interpolation
                //! @{
                Geo::CCoordinateGeodetic interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                Aviation::CAltitude interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                //! @}

                //! Interpolator for pitch, bank, heading, groundspeed
                CInterpolatorPbh pbh() const { return { m_simulationTimeFraction, m_oldSituation, m_newSituation }; }

            private:
                int m_situationsAvailable = 0;
                Aviation::CAircraftSituation m_oldSituation;
                Aviation::CAircraftSituation m_newSituation;
                double m_simulationTimeFraction = 0.0;
            };

            //! Get the interpolant for the given time point
            Interpolant getInterpolant(qint64 currentTimeMsSinceEpoc, const CInterpolationAndRenderingSetup &setup,
                                       const CInterpolationHints &hints, CInterpolationStatus &status, CInterpolationLogger::SituationLog &log) const;
        };
    } // ns
} // ns

#endif // guard
