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
        CAircraftModel CDatabaseUtils::consolidateOwnAircraftModelWithDbData(const CAircraftModel &model, bool force, bool *modified)
        {
            bool myModified = false;
            CAircraftModel ownModel = CDatabaseUtils::consolidateModelWithDbData(model, force, &myModified);
            // special case here, as we have some specific values for a local model

            if (myModified)
            {
                ownModel.updateMissingParts(model);
                ownModel.setFileName(model.getFileName());
                myModified = true;
            }
            if (ownModel.getModelType() != CAircraftModel::TypeOwnSimulatorModel)
            {
                ownModel.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                myModified = true;
            }
            if (modified) { *modified = myModified; }
            return ownModel;
        }

        CAircraftModel CDatabaseUtils::consolidateModelWithDbData(const CAircraftModel &model, bool force, bool *modified)
        {
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application object");
            Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "No web services");

            if (modified) { *modified = false; }
            if (!model.hasModelString()) { return model; }
            if (!force && model.hasValidDbKey()) { return model; }
            const int distributorOrder = model.getDistributorOrder(); // later restore that order

            CAircraftModel dbModel(sApp->getWebDataServices()->getModelForModelString(model.getModelString()));
            if (dbModel.isLoadedFromDb())
            {
                // take the db model as original
                if (modified) { *modified = true; }
                dbModel.updateMissingParts(model);
                dbModel.setDistributorOrder(distributorOrder);
                return dbModel;
            }

            // we try our best to update by DB data here
            // since we have no(!) DB model, we update each of it subobjects
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
            consolidatedModel.updateLocalFileNames(model);
            consolidatedModel.setDistributorOrder(distributorOrder);
            return consolidatedModel;
        }

        int CDatabaseUtils::consolidateModelsWithDbData(CAircraftModelList &models, bool force)
        {
            return CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(models, force, false);
        }

        int CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(CAircraftModelList &models, bool force, bool processEvents)
        {
            QTime timer;
            timer.start();
            int c = 0;
            if (models.isEmpty()) { return c; }
            for (CAircraftModel &model : models)
            {
                if (!force && model.isLoadedFromDb()) { continue; }
                bool modified = false;
                model = CDatabaseUtils::consolidateModelWithDbData(model, force, &modified);
                if (modified || model.hasValidDbKey())
                {
                    c++;
                    if (processEvents && c % 125 == 0) { sApp->processEventsFor(25); }
                }
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

        CAircraftModelList CDatabaseUtils::updateSimulatorForFsFamily(const CAircraftModelList &ownModels, int maxToStash, IProgressIndicator *progressIndicator, bool processEvents)
        {
            CAircraftModelList dbFsFamilyModels(sApp->getWebDataServices()->getModels().getAllFsFamilyModels());
            CAircraftModelList stashModels;
            if (dbFsFamilyModels.isEmpty() || ownModels.isEmpty()) { return stashModels; }
            const QSet<QString> dbKeys = dbFsFamilyModels.getModelStringSet();
            const int mexModelsCount = maxToStash >= 0 ? maxToStash : ownModels.size();
            if (mexModelsCount < 1) { return stashModels; }

            int c = 0; // counter
            for (const CAircraftModel &ownModel : ownModels)
            {
                c++;

                // process events
                if (processEvents && c % 500 == 0)
                {
                    if (progressIndicator)
                    {
                        const int percentage = c * 100 / mexModelsCount;
                        progressIndicator->updateProgressIndicatorAndProcessEvents(percentage);
                    }
                    else
                    {
                        sApp->processEventsFor(10);
                    }
                }

                // values to be skipped
                if (maxToStash >= 0 && maxToStash == stashModels.size()) { break; }
                if (!dbKeys.contains(ownModel.getModelString())) { continue; }
                if (ownModel.matchesSimulatorFlag(CSimulatorInfo::XPLANE)) { continue; }

                // in DB
                CAircraftModel dbModel = dbFsFamilyModels.findFirstByModelStringOrDefault(ownModel.getModelString());
                if (!dbModel.isLoadedFromDb()) {continue; }
                if (dbModel.getSimulator() == ownModel.getSimulator()) {continue; }

                // update simulator and add
                CSimulatorInfo simulator(dbModel.getSimulator());
                simulator.add(ownModel.getSimulator());
                dbModel.setSimulator(simulator);
                stashModels.push_back(dbModel);
            }
            return stashModels;
        }
    } // ns
} // ns
