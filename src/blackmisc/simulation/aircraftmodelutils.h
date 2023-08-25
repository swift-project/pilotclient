// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELUTILS_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELUTILS_H

#include <atomic>
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"

namespace BlackMisc::Simulation
{
    //! Utilities for aircraft models
    class BLACKMISC_EXPORT CAircraftModelUtilities
    {
    public:
        //! No constructor
        CAircraftModelUtilities() = delete;

        //! Merge with vPilot data if possible
        //! \deprecated vPilot parts might be removed
        static bool mergeWithVPilotData(BlackMisc::Simulation::CAircraftModelList &modelToBeModified, const BlackMisc::Simulation::CAircraftModelList &vPilotModels, bool force = false);

        //! Matrix airlines/aircraft ICAOs
        static QString createIcaoAirlineAircraftHtmlMatrix(const BlackMisc::Simulation::CAircraftModelList &models);

        //! Matrix airlines/aircraft ICAOs
        static QString createIcaoAirlineAircraftHtmlMatrixFile(const BlackMisc::Simulation::CAircraftModelList &models, const QString &tempDir);

        //! Validate aircraft.cfg entries
        static CStatusMessageList validateModelFiles(const CSimulatorInfo &simulator, const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmpty, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir);
    };
} // namespace

#endif // guard
