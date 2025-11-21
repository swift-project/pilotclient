// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORLINEAR_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORLINEAR_H

#include <QtGlobal>

#include "misc/aviation/aircraftsituation.h"
#include "misc/simulation/interpolation/interpolant.h"
#include "misc/simulation/interpolation/interpolationlogger.h"
#include "misc/simulation/interpolation/interpolator.h"
#include "misc/swiftmiscexport.h"

class QObject;

namespace swift::misc
{
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        //! Linear interpolator, calculation inbetween positions
        class SWIFT_MISC_EXPORT CInterpolatorLinear : public CInterpolator
        {
            void anchor() override;

        public:
            //! Constructor
            CInterpolatorLinear(const aviation::CCallsign &callsign, ISimulationEnvironmentProvider *simEnvProvider,
                                IInterpolationSetupProvider *setupProvider,
                                IRemoteAircraftProvider *remoteAircraftProvider, CInterpolationLogger *logger = nullptr)
                : CInterpolator(callsign, simEnvProvider, setupProvider, remoteAircraftProvider, logger)
            {}

            //! Linear function that performs the actual interpolation
            class SWIFT_MISC_EXPORT CInterpolant : public IInterpolant
            {
            public:
                //! @{
                //! Constructor
                CInterpolant() = default;
                CInterpolant(const aviation::CAircraftSituation &startSituation);
                CInterpolant(const aviation::CAircraftSituation &startSituation, const CInterpolatorLinearPbh &pbh);
                CInterpolant(const aviation::CAircraftSituation &startSituation,
                             const aviation::CAircraftSituation &endSituation, double timeFraction,
                             qint64 interpolatedTime);
                //! @}

                //! \copydoc swift::misc::simulation::IInterpolant::interpolatePositionAndAltitude
                std::tuple<geo::CCoordinateGeodetic, aviation::CAltitude>
                interpolatePositionAndAltitude() const override;

                //! \copydoc swift::misc::simulation::IInterpolant::interpolateGroundFactor
                aviation::COnGroundInfo interpolateGroundFactor() const override;

                //! Start situation
                const aviation::CAircraftSituation &getStartSituation() const { return m_startSituation; }

                //! End situation
                const aviation::CAircraftSituation &getEndSituation() const { return m_endSituation; }

                //! \copydoc swift::misc::simulation::IInterpolant::pbh
                const IInterpolatorPbh &pbh() const override { return m_pbh; }

            private:
                aviation::CAircraftSituation m_startSituation;
                aviation::CAircraftSituation m_endSituation;
                double m_simulationTimeFraction = 0.0; //!< 0..1
                CInterpolatorLinearPbh m_pbh;
            };

            //! \copydoc swift::misc::simulation::CInterpolator::getInterpolant
            const IInterpolant &getInterpolant(SituationLog &log) override;

        private:
            CInterpolant m_interpolant; //!< current interpolant
        };
    } // namespace simulation
} // namespace swift::misc

#endif // SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORLINEAR_H
