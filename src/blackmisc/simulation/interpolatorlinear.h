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
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraftsituation.h"
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
                    situationsAvailable(1), oldSituation(situation) {}
                Interpolant(const Aviation::CAircraftSituation &situation1, const Aviation::CAircraftSituation &situation2, double time) :
                    situationsAvailable(2), oldSituation(situation1), newSituation(situation2), simulationTimeFraction(time) {}
                //! @}

                //! Perform the interpolation
                //! @{
                Geo::CCoordinateGeodetic interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                Aviation::CAltitude interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const;
                //! @}

                //! Interpolator for pitch, bank, heading, groundspeed
                CInterpolatorPbh pbh() const { return { simulationTimeFraction, oldSituation, newSituation }; }

            private:
                int situationsAvailable = 0;
                Aviation::CAircraftSituation oldSituation;
                Aviation::CAircraftSituation newSituation;
                double simulationTimeFraction = 0.0;
            };

            //! Get the interpolant for the given time point
            Interpolant getInterpolant(qint64 currentTimeMsSinceEpoc, const CInterpolationAndRenderingSetup &setup,
                const CInterpolationHints &hints, CInterpolationStatus &status, InterpolationLog &log) const;

            //! Log category
            static QString getLogCategory() { return "swift.interpolatorlinear"; }
        };
    } // ns
} // ns

#endif // guard
