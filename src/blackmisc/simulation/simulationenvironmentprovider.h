/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATIONENVIRONMENTPROVIDER_H
#define BLACKMISC_SIMULATION_SIMULATIONENVIRONMENTPROVIDER_H

#include "blackmisc/geo/coordinategeodeticlist.h"
#include "blackmisc/geo/elevationplane.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to elevation data
        //! \remark we are interested in elevations of airports
        class BLACKMISC_EXPORT ISimulationEnvironmentProvider
        {
        public:
            //! All remembered coordiantes
            //! \remark might be a relatively large list
            //! \threadsafe
            Geo::CCoordinateGeodeticList getElevationCoordinates() const;

            //! Only keep closest ones
            //! \threadsafe
            int cleanUpElevations(const Geo::ICoordinateGeodetic &referenceCoordinate, int maxNumber = MaxElevations);

            //! Find closest elevation
            //! \threadsafe
            Geo::CElevationPlane findClosestElevationWithinRange(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range);

            //! Clear data
            //! \threadsafe
            void clearSimulationEnvironmentData();

        protected:
            static constexpr int MaxElevations = 1000;  //!< How many elevations we keep

            //! Remember a given elevation
            //! \threadsafe
            bool rememberGroundElevation(const Geo::ICoordinateGeodetic &elevationCoordinate, const PhysicalQuantities::CLength &epsilon = Geo::CElevationPlane::singlePointRadius()) ;

            //! Remember a given elevation
            //! \threadsafe
            bool rememberGroundElevation(const Geo::CElevationPlane &elevationPlane) ;

        private:
            Geo::CCoordinateGeodeticList m_elvCoordinates;
            mutable QReadWriteLock m_lockElvCoordinates; //!< lock m_coordinates
        };

        //! Class which can be directly used to access an \sa ISimulationEnvironmentProvider object
        class BLACKMISC_EXPORT CSimulationEnvironmentAware
        {
        public:
            //! Elevations, can be changed over time as it comes from simulator
            void setSimulationEnvironmentProvider(ISimulationEnvironmentProvider *provider) { m_simEnvironmentProvider = provider; }

            //! Find closest elevation
            Geo::CElevationPlane findClosestElevationWithinRange(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range);

        protected:
            //! Default constructor
            CSimulationEnvironmentAware() {}

            //! Constructor
            CSimulationEnvironmentAware(ISimulationEnvironmentProvider *simEnvProvider) : m_simEnvironmentProvider(simEnvProvider) { Q_ASSERT(simEnvProvider); }
            ISimulationEnvironmentProvider *m_simEnvironmentProvider = nullptr; //!< access to object
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::ISimulationEnvironmentProvider, "BlackMisc::Simulation::ISimulationEnvironmentProvider")

#endif // guard
