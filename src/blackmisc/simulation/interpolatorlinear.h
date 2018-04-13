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
        public:
            //! Constructor
            CInterpolatorLinear(const Aviation::CCallsign &callsign,
                                ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteAircraftProvider,
                                CInterpolationLogger *logger = nullptr) :
                CInterpolator(callsign, simEnvProvider, setupProvider, remoteAircraftProvider, logger) {}

            //! Linear function that performs the actual interpolation
            class Interpolant
            {
            public:
                //! Constructor
                //! @{
                Interpolant(const Aviation::CAircraftSituation &situation);
                Interpolant(const Aviation::CAircraftSituation &situation1, const Aviation::CAircraftSituation &situation2, double timeFraction, qint64 interpolatedTime);
                //! @}

                //! Perform the interpolation
                Aviation::CAircraftSituation interpolatePositionAndAltitude(const Aviation::CAircraftSituation &situation) const;

                //! Interpolator for pitch, bank, heading, groundspeed
                const CInterpolatorPbh &pbh() const { return m_pbh; }

                //! Old situation
                const Aviation::CAircraftSituation &getOldSituation() const { return m_oldSituation; }

                //! New situation
                const Aviation::CAircraftSituation &getNewSituation() const { return m_newSituation; }

                //! "Real time" representing the interpolated situation
                qint64 getInterpolatedTime() const { return m_interpolatedTime; }

            private:
                int m_situationsAvailable = 0;
                Aviation::CAircraftSituation m_oldSituation;
                Aviation::CAircraftSituation m_newSituation;
                double m_simulationTimeFraction = 0.0; //!< 0..1
                qint64 m_interpolatedTime = 0; //!< "Real time "of interpolated situation
                const CInterpolatorPbh m_pbh;  //!< pitch, bank, ground speed and heading
            };

            //! Get the interpolant for the given time point
            Interpolant getInterpolant(qint64 currentTimeMsSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, CInterpolationStatus &status, SituationLog &log);
        };
    } // ns
} // ns

#endif // guard
