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

            //! Merge with vPilot data if possible
            static bool mergeWithVPilotData(BlackMisc::Simulation::CAircraftModelList &modelToBeModified, const BlackMisc::Simulation::CAircraftModelList &vPilotModels, bool force = false);

            //! Matrix airlines/aircraft ICAOs
            static QString createIcaoAirlineAircraftHtmlMatrix(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Matrix airlines/aircraft ICAOs
            static QString createIcaoAirlineAircraftHtmlMatrixFile(const BlackMisc::Simulation::CAircraftModelList &models, const QString &tempDir);

            //! Validate aircraft.cfg entries
            static CStatusMessageList validateModelFiles(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmpty, int stopAtFailedFiles, bool &stopped);
        };
    } //namespace
} // namespace

#endif //guard
