/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_DATABASEUTILS_H
#define BLACKCORE_DB_DATABASEUTILS_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/progress.h"
#include "blackmisc/simulation/aircraftmodel.h"

namespace BlackCore
{
    namespace Db
    {
        //! Read information about data from Database
        class BLACKCORE_EXPORT CDatabaseUtils
        {
        public:
            //! No constructor
            CDatabaseUtils() = delete;

            //! Consolidate model data with DB data
            static BlackMisc::Simulation::CAircraftModel consolidateModelWithDbData(const BlackMisc::Simulation::CAircraftModel &model, bool force);

            //! Consolidate model data with DB data
            static BlackMisc::Simulation::CAircraftModel consolidateModelWithDbData(const BlackMisc::Simulation::CAircraftModel &model, bool force, bool *modified);

            //! Consolidate models with DB data
            static int consolidateModelsWithDbData(BlackMisc::Simulation::CAircraftModelList &models, bool force);

            //! Consolidate models with DB data
            static int consolidateModelsWithDbDataAllowsGuiRefresh(BlackMisc::Simulation::CAircraftModelList &models, bool force, bool processEvents);

            //! Consolidate models with DB data (simpler/faster version of CAircraftModel::consolidateModelWithDbData)
            static int consolidateModelsWithDbModelAndDistributor(BlackMisc::Simulation::CAircraftModelList &models, bool force);

            //! Consolidate model data with DB distributor
            static BlackMisc::Simulation::CAircraftModel consolidateModelWithDbDistributor(const BlackMisc::Simulation::CAircraftModel &model, bool force);

            //! Consolidate model data with DB distributors
            static int consolidateModelsWithDbDistributor(BlackMisc::Simulation::CAircraftModelList &models, bool force);

            //! Create stash models if the DB models miss that simulator
            static BlackMisc::Simulation::CAircraftModelList updateSimulatorForFsFamily(const BlackMisc::Simulation::CAircraftModelList &ownModels, int maxToStash = -1, BlackCore::IProgressIndicator *progressIndicator = nullptr, bool processEvents = true);
        };
    } // ns
} // ns
#endif // guard
