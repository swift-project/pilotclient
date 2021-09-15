/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/modelsetbuilder.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/databaseutils.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QStringList>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Simulation;
using namespace BlackCore::Db;

namespace BlackCore
{
    CModelSetBuilder::CModelSetBuilder(QObject *parent) : QObject(parent)
    {
        // void
    }

    CAircraftModelList CModelSetBuilder::buildModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &models, const CAircraftModelList &currentSet, Builder options, const CDistributorList &distributors) const
    {
        if (models.isEmpty()) { return CAircraftModelList(); }
        CAircraftModelList modelSet;

        // Select by distributor:
        // I avoid an empty distributor set because it wipes out everything
        if (options.testFlag(GivenDistributorsOnly) && !distributors.isEmpty())
        {
            modelSet = models.findByDistributors(distributors);
        }
        else
        {
            modelSet = models;
        }

        // Only DB data?
        if (options.testFlag(OnlyDbData))
        {
            modelSet.removeObjectsWithoutDbKey();
        }
        else if (options.testFlag(OnlyDbIcaoCodes))
        {
            Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "No web data services");
            const QSet<QString> designators(sApp->getWebDataServices()->getAircraftIcaoCodes().allDesignators());
            modelSet = modelSet.findWithAircraftDesignator(designators);
        }
        else
        {
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
        if (options.testFlag(ConsolidateWithDb))
        {
            CDatabaseUtils::consolidateModelsWithDbData(modelSet, true);
        }

        // result
        return modelSet;
    }
} // ns
