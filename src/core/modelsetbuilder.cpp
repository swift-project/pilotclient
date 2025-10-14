// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/modelsetbuilder.h"

#include <QStringList>
#include <Qt>
#include <QtGlobal>

#include "core/application.h"
#include "core/db/databaseutils.h"
#include "core/webdataservices.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/simulation/aircraftmodel.h"

using namespace swift::misc::simulation;
using namespace swift::core::db;

namespace swift::core
{
    CModelSetBuilder::CModelSetBuilder(QObject *parent) : QObject(parent)
    {
        // void
    }

    CAircraftModelList CModelSetBuilder::buildModelSet(const CSimulatorInfo &simulator,
                                                       const CAircraftModelList &models,
                                                       const CAircraftModelList &currentSet, Builder options,
                                                       const CDistributorList &distributors) const
    {
        if (models.isEmpty()) { return CAircraftModelList(); }
        CAircraftModelList modelSet;

        // Select by distributor:
        // I avoid an empty distributor set because it wipes out everything
        if (options.testFlag(GivenDistributorsOnly) && !distributors.isEmpty())
        {
            modelSet = models.findByDistributors(distributors);
        }
        else { modelSet = models; }

        // Only DB data?
        if (options.testFlag(OnlyDbData)) { modelSet.removeObjectsWithoutDbKey(); }
        else if (options.testFlag(OnlyDbIcaoCodes))
        {
            Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "No web data services");
            const QSet<QString> designators(sApp->getWebDataServices()->getAircraftIcaoCodes().allDesignators());
            modelSet = modelSet.findWithAircraftDesignator(designators);
        }
        else
        {
            if (!options.testFlag(ShowAllInstalledModells))
                // without any information we can not use them
                modelSet = modelSet.findWithKnownAircraftDesignator();
        }

        // Include only
        modelSet = modelSet.matchesSimulator(simulator);
        modelSet.setModelMode(CAircraftModel::Include); // in sets we only include, exclude means not present in set

        if (options.testFlag(Incremental))
        {
            if (!currentSet.isEmpty())
            {
                // update in full set
                CAircraftModelList copy(currentSet);
                copy.replaceOrAddModelsWithString(modelSet, Qt::CaseInsensitive);
                modelSet = copy;
            }
        }

        // sort by preferences if applicable
        modelSet.resetOrder();
        if (options.testFlag(SortByDistributors))
        {
            modelSet.updateDistributorOrder(distributors);
            modelSet.sortBy(&CAircraftModel::getDistributorOrder);
        }

        // DB consolidation
        if (options.testFlag(ConsolidateWithDb)) { CDatabaseUtils::consolidateModelsWithDbData(modelSet, true); }

        // result
        return modelSet;
    }
} // namespace swift::core
