/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelsetbuilder.h"
#include "application.h"

using namespace BlackMisc::Simulation;

namespace BlackCore
{
    CModelSetBuilder::CModelSetBuilder(QObject *parent) : QObject(parent)
    {
        // void
    }

    CAircraftModelList CModelSetBuilder::buildModelSet(const CAircraftModelList &models, Builder oprions, const CDistributorList &onlyByDistributors) const
    {
        if (models.isEmpty()) { return CAircraftModelList(); }
        CAircraftModelList modelSet;
        if (oprions.testFlag(FilterDistributos))
        {
            modelSet = models.byDistributor(onlyByDistributors);
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
            modelSet = modelSet.withAircraftDesignator(designators);
        }
        else
        {
            // without any information we can not use them
            modelSet = modelSet.withKnownAircraftDesignator();
        }
        return modelSet;
    }
}
