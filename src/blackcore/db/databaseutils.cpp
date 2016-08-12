/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "databaseutils.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Db
    {
        CAircraftModel CDatabaseUtils::consolidateModelWithDbData(const CAircraftModel &model, bool force)
        {
            return CDatabaseUtils::consolidateModelWithDbData(model, force, nullptr);
        }

        CAircraftModel CDatabaseUtils::consolidateModelWithDbData(const CAircraftModel &model, bool force, bool *modified)
        {
            if (modified) { *modified = false; }
            if (!model.hasModelString()) { return model; }
            if (!force && model.hasValidDbKey()) { return model; }

            CAircraftModel dbModel(sApp->getWebDataServices()->getModelForModelString(model.getModelString()));
            if (dbModel.hasValidDbKey())
            {
                if (modified) { *modified = true; }
                dbModel.updateByLocalFileNames(model);
                return dbModel;
            }

            // we try to best update by DB data here
            // we have no(!) DB model, so we update each of it subobjects
            CAircraftModel consolidatedModel(model); // copy over
            if (!consolidatedModel.getLivery().hasValidDbKey())
            {
                const CLivery dbLivery(sApp->getWebDataServices()->smartLiverySelector(consolidatedModel.getLivery()));
                if (dbLivery.hasValidDbKey())
                {
                    if (modified) { *modified = true; }
                    consolidatedModel.setLivery(dbLivery);
                }
            }
            if (!consolidatedModel.getAircraftIcaoCode().hasValidDbKey() && consolidatedModel.hasAircraftDesignator())
            {
                // try to find DB aircraft ICAO here
                const CAircraftIcaoCode dbIcao(sApp->getWebDataServices()->smartAircraftIcaoSelector(consolidatedModel.getAircraftIcaoCode()));
                if (dbIcao.hasValidDbKey())
                {
                    if (modified) { *modified = true; }
                    consolidatedModel.setAircraftIcaoCode(dbIcao);
                }
            }

            const CDistributor dbDistributor(sApp->getWebDataServices()->getDistributors().smartDistributorSelector(model.getDistributor(), model));
            if (dbDistributor.isLoadedFromDb())
            {
                if (modified) { *modified = true; }
                consolidatedModel.setDistributor(dbDistributor);
            }
            consolidatedModel.updateByLocalFileNames(model);
            return consolidatedModel;
        }

        int CDatabaseUtils::consolidateModelsWithDbData(CAircraftModelList &models, bool force)
        {
            QTime timer;
            timer.start();
            int c = 0;
            if (models.isEmpty()) { return c; }
            for (CAircraftModel &model : models)
            {
                if (!force && model.hasValidDbKey()) { continue; }
                bool modified = false;
                model = CDatabaseUtils::consolidateModelWithDbData(model, force, &modified);
                if (modified || model.hasValidDbKey()) { c++; }
            }
            CLogMessage().debug() << "Consolidated " << models.size() << " in " << timer.elapsed() << "ms";
            return c;
        }

        int CDatabaseUtils::consolidateModelsWithDbModelAndDistributor(CAircraftModelList &models, bool force)
        {
            int c = 0;
            if (models.isEmpty()) { return c; }
            for (CAircraftModel &model : models)
            {
                if (!force && model.hasValidDbKey()) { continue; }
                const CAircraftModel dbModel(sApp->getWebDataServices()->getModelForModelString(model.getModelString()));
                if (dbModel.hasValidDbKey())
                {
                    model = dbModel;
                    c++;
                    continue;
                }
                const CDistributor distributor = sApp->getWebDataServices()->smartDistributorSelector(model.getDistributor(), model);
                if (distributor.isLoadedFromDb())
                {
                    model.setDistributor(distributor);
                }
            }
            return c;
        }

        CAircraftModel CDatabaseUtils::consolidateModelWithDbDistributor(const CAircraftModel &model, bool force)
        {
            if (!force && model.getDistributor().isLoadedFromDb()) { return model; }
            const CDistributor distributor = sApp->getWebDataServices()->smartDistributorSelector(model.getDistributor(), model);
            if (!distributor.isLoadedFromDb()) { return model; }
            CAircraftModel newModel(model);
            newModel.setDistributor(distributor);
            return newModel;
        }

        int CDatabaseUtils::consolidateModelsWithDbDistributor(CAircraftModelList &models, bool force)
        {
            int c = 0;
            if (models.isEmpty()) { return c; }
            for (CAircraftModel &model : models)
            {
                if (model.hasValidDbKey() || model.getDistributor().hasValidDbKey()) { continue; }
                model = CDatabaseUtils::consolidateModelWithDbDistributor(model, force);
                if (model.getDistributor().hasValidDbKey()) { c++; }
            }
            return c;
        }
    } // ns
} // ns
