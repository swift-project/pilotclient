// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H
#define BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H

#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolant.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/blackmiscexport.h"

#include <QtGlobal>

class QObject;

namespace BlackMisc
{
    namespace Aviation
    {
        class CCallsign;
    }
    namespace Simulation
    {
        //! Linear interpolator, calculation inbetween positions
        class BLACKMISC_EXPORT CInterpolatorLinear : public CInterpolator
        {
            virtual void anchor() override;

        public:
            //! Constructor
            CInterpolatorLinear(const Aviation::CCallsign &callsign,
                                ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteAircraftProvider,
                                CInterpolationLogger *logger = nullptr) : CInterpolator(callsign, simEnvProvider, setupProvider, remoteAircraftProvider, logger) {}

            //! Linear function that performs the actual interpolation
            class BLACKMISC_EXPORT CInterpolant : public IInterpolant
            {
            public:
                //! @{
                //! Constructor
                CInterpolant() = default;
                CInterpolant(const Aviation::CAircraftSituation &startSituation);
                CInterpolant(const Aviation::CAircraftSituation &startSituation, const CInterpolatorLinearPbh &pbh);
                CInterpolant(const Aviation::CAircraftSituation &startSituation, const Aviation::CAircraftSituation &endSituation, double timeFraction, qint64 interpolatedTime);
                //! @}

                //! \copydoc BlackMisc::Simulation::IInterpolant::interpolatePositionAndAltitude
                std::tuple<Geo::CCoordinateGeodetic, Aviation::CAltitude> interpolatePositionAndAltitude() const override;

                //! \copydoc BlackMisc::Simulation::IInterpolant::interpolateGroundFactor
                Aviation::COnGroundInfo interpolateGroundFactor() const override;

                //! Start situation
                const Aviation::CAircraftSituation &getStartSituation() const { return m_startSituation; }

                //! End situation
                const Aviation::CAircraftSituation &getEndSituation() const { return m_endSituation; }

                //! \copydoc BlackMisc::Simulation::IInterpolant::pbh
                const IInterpolatorPbh &pbh() const override { return m_pbh; }

            private:
                Aviation::CAircraftSituation m_startSituation;
                Aviation::CAircraftSituation m_endSituation;
                double m_simulationTimeFraction = 0.0; //!< 0..1
                CInterpolatorLinearPbh m_pbh;
            };

            //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolant
            const IInterpolant &getInterpolant(SituationLog &log) override;

        private:
            CInterpolant m_interpolant; //!< current interpolant
        };
    } // ns
} // ns

#endif // guard
