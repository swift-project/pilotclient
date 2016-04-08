/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELUTILS_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELUTILS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Utilities for aircraft models
        class BLACKMISC_EXPORT CAircraftModelUtilities
        {
        public:
            //! No constructor
            CAircraftModelUtilities() = delete;

            //! Merge with DB data if possible
            static bool mergeWithDbData(BlackMisc::Simulation::CAircraftModelList &modelToBeModified, const BlackMisc::Simulation::CAircraftModelList &dbModels, bool force = false);

            //! Merge with vPilot data if possible
            static bool mergeWithVPilotData(BlackMisc::Simulation::CAircraftModelList &modelToBeModified, const BlackMisc::Simulation::CAircraftModelList &vPilotModels, bool force = false);
        };
    } //namespace
} // namespace

#endif //guard
