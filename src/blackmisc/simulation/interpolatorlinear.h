/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H
#define BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H

#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolant.h"
#include "blackmisc/simulation/interpolantvelocity.h"
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
            virtual void anchor() override;

        public:
            //! Constructor
            CInterpolatorLinear(const Aviation::CCallsign &callsign,
                                ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteAircraftProvider,
                                CInterpolationLogger *logger = nullptr) :
                CInterpolator(callsign, simEnvProvider, setupProvider, remoteAircraftProvider, logger) {}

            //! Linear function that performs the actual interpolation
            class BLACKMISC_EXPORT CInterpolant : public IInterpolant
            {
            public:
                //! Constructor
                //! @{
                CInterpolant() {}
                CInterpolant(const Aviation::CAircraftSituation &oldSituation);
                CInterpolant(const Aviation::CAircraftSituation &oldSituation, const CInterpolatorPbh &pbh);
                CInterpolant(const Aviation::CAircraftSituation &oldSituation, const Aviation::CAircraftSituation &newSituation, double timeFraction, qint64 interpolatedTime, const CInterpolantVelocity &velocity);
                //! @}

                //! Perform the interpolation
                Aviation::CAircraftSituation interpolatePositionAndAltitude(const Aviation::CAircraftSituation &situation, bool interpolateGndFactor);

                //! Old situation
                const Aviation::CAircraftSituation &getOldSituation() const { return m_oldSituation; }

                //! New situation
                const Aviation::CAircraftSituation &getNewSituation() const { return m_newSituation; }

            private:
                Aviation::CAircraftSituation m_oldSituation;
                Aviation::CAircraftSituation m_newSituation;
                double m_simulationTimeFraction = 0.0; //!< 0..1
            };

            //! Get the interpolant for the given time point
            CInterpolant getInterpolant(SituationLog &log);

            //! To be used by the velocity interpolant
            //! @{
            void setLatestSituation(const Aviation::CAircraftSituation &situation) { m_interpolant.setLatestSituation(situation); }
            void updateInterpolantTime() { m_interpolant.setCurrentTime(m_currentTimeMsSinceEpoch); }
            //! @}

        private:
            CInterpolant m_interpolant; //!< current interpolant
        };
    } // ns
} // ns

#endif // guard
