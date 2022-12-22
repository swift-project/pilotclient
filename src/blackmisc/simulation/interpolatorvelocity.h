/* Copyright (C) 2022
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORVELOCITY_H
#define BLACKMISC_SIMULATION_INTERPOLATORVELOCITY_H

#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolant.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QtGlobal>

class QObject;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation
    {
        //! Extrapolation with VATSIM velocity packets
        class BLACKMISC_EXPORT CInterpolatorVelocity : public CInterpolator<CInterpolatorVelocity>
        {
            virtual void anchor() override;

        public:
            //! Constructor
            CInterpolatorVelocity(const Aviation::CCallsign &callsign,
                                ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteAircraftProvider,
                                CInterpolationLogger *logger = nullptr) :
                CInterpolator(callsign, simEnvProvider, setupProvider, remoteAircraftProvider, logger) {}

            //! Function that performs the extrapolation
            class BLACKMISC_EXPORT CInterpolant : public IInterpolant
            {
            public:
                //! Constructor
                //! @{
                CInterpolant() { setValid(false); };
                CInterpolant(const Aviation::CAircraftSituation &currentSituation); // First situtation
                CInterpolant(const CInterpolant &old, const Aviation::CAircraftSituation &currentSituation); // everything thereafter
                //! @}

                //! Perform the interpolation
                Aviation::CAircraftSituation interpolatePositionAndAltitude(const Aviation::CAircraftSituation &situation, bool interpolateGndFactor) const;

                //! Current situation
                const Aviation::CAircraftSituation &getCurrentSitutation() const { return m_currentSitutation; }

                //! Update interpolant to given timestamp. Every following call to
                //! interpolatePositionAndAltitude(..) will return the situation according to this timestamp.
                void update(qint64 currentMsSinceEpoch);

            private:
                Aviation::CAircraftSituation m_currentSitutation;
                qint64 m_creationTimeMsSinceEpoch;
                qint64 m_currentTimeMsSinceEpoch;
                double m_altitudeDelta = 0;
                double m_latDeltaRad = 0;
                double m_lonDeltaRad = 0;
                double m_bankDelta = 0;
                double m_headingDelta = 0;
                double m_pitchDelta = 0;
            };

            //! Get the interpolant for current timestamp
            CInterpolant getInterpolant(SituationLog &log);

        private:
            CInterpolant m_interpolant; //!< current interpolant
            Aviation::CAircraftSituation m_lastSituation; //!< Last situation used to create interpolant
        };
    } // ns
} // ns

#endif // guard
