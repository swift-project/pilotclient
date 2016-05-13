/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/modelsetbuilder.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QStringList>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Simulation;

namespace BlackCore
{
    CModelSetBuilder::CModelSetBuilder(QObject *parent) : QObject(parent)
    {
        // void
    }

    CAircraftModelList CModelSetBuilder::buildModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &models, const CAircraftModelList &currentSet, Builder oprions, const CDistributorList &onlyByDistributors) const
    {
        if (models.isEmpty()) { return CAircraftModelList(); }
        CAircraftModelList modelSet;

        // I avoid and empty distributor set wipes out everything
        if (oprions.testFlag(FilterDistributos) && !onlyByDistributors.isEmpty())
        {
            modelSet = models.findByDistributors(onlyByDistributors);
        }
        else
        {
            modelSet = models;
        }

        if (oprions.testFlag(OnlyDbData))
        {
            modelSet.removeObjectsWithoutDbKey();
        }
        else if (oprions.testFlag(OnlyDbIcaoCodes))
        {
            Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "No web data services");
            const QStringList designators(sApp->getWebDataServices()->getAircraftIcaoCodes().allIcaoCodes());
            modelSet = modelSet.findWithAircraftDesignator(designators);
        }
        else
        {
            // without any information we can not use them
            modelSet = modelSet.findWithKnownAircraftDesignator();
        }
        modelSet.setModelMode(CAircraftModel::Include); // in sets we only include, exclude means not present in set
        if (oprions.testFlag(Incremental))
        {
            if (currentSet.isEmpty()) { return modelSet; }
            CAircraftModelList copy(currentSet);
            copy.replaceOrAddModelsWithString(modelSet, Qt::CaseInsensitive); // incremental
            return copy.matchesSimulator(simulator);
        }
        else
        {
            return modelSet.matchesSimulator(simulator);
        }
    }
} // ns
