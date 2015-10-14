/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/predicates.h"

namespace BlackMisc
{
    namespace Simulation
    {
        CDistributorList::CDistributorList() { }

        CDistributorList::CDistributorList(const CSequence<CDistributor> &other) :
            CSequence<CDistributor>(other)
        { }

        CDistributor CDistributorList::findByIdOrAlias(const QString &idOrAlias)
        {
            if (idOrAlias.isEmpty()) { return CDistributor(); }
            for (const CDistributor &distributor : (*this))
            {
                if (distributor.matchesIdOrAlias(idOrAlias)) { return distributor; }
            }
            return CDistributor();
        }

        CDistributor CDistributorList::smartDistributorSelector(const CDistributor &distributorPattern)
        {
            if (distributorPattern.hasValidDbKey())
            {
                QString k(distributorPattern.getDbKey());
                CDistributor d(this->findByKey(k));
                if (d.hasCompleteData()) { return d; }

                // more lenient search
                return this->findByIdOrAlias(k);
            }
            return CDistributor();
        }

    } // namespace
} // namespace
