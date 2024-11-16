// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_AIRCRAFTMODELUTILS_H
#define SWIFT_MISC_SIMULATION_AIRCRAFTMODELUTILS_H

#include <atomic>

#include "misc/simulation/aircraftmodellist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Utilities for aircraft models
    class SWIFT_MISC_EXPORT CAircraftModelUtilities
    {
    public:
        //! No constructor
        CAircraftModelUtilities() = delete;

        //! Merge with vPilot data if possible
        //! \deprecated vPilot parts might be removed
        static bool mergeWithVPilotData(swift::misc::simulation::CAircraftModelList &modelToBeModified,
                                        const swift::misc::simulation::CAircraftModelList &vPilotModels,
                                        bool force = false);

        //! Matrix airlines/aircraft ICAOs
        static QString createIcaoAirlineAircraftHtmlMatrix(const swift::misc::simulation::CAircraftModelList &models);

        //! Matrix airlines/aircraft ICAOs
        static QString
        createIcaoAirlineAircraftHtmlMatrixFile(const swift::misc::simulation::CAircraftModelList &models,
                                                const QString &tempDir);

        //! Validate aircraft.cfg entries
        static CStatusMessageList validateModelFiles(const CSimulatorInfo &simulator, const CAircraftModelList &models,
                                                     CAircraftModelList &validModels, CAircraftModelList &invalidModels,
                                                     bool ignoreEmpty, int stopAtFailedFiles,
                                                     std::atomic_bool &wasStopped, const QString &simulatorDir);
    };
} // namespace swift::misc::simulation

#endif // guard
