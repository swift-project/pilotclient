/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/metaclassprivate.h"

#include <tuple>

namespace BlackMisc
{
    namespace Simulation
    {
        CDistributorList::CDistributorList() { }

        CDistributorList::CDistributorList(const CSequence<CDistributor> &other) :
            CSequence<CDistributor>(other)
        { }

        CDistributor CDistributorList::findByKeyOrAlias(const QString &keyOrAlias) const
        {
            if (keyOrAlias.isEmpty()) { return CDistributor(); }
            for (const CDistributor &distributor : (*this))
            {
                if (distributor.matchesKeyOrAlias(keyOrAlias)) { return distributor; }
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
                return this->findByKeyOrAlias(k);
            }
            return CDistributor();
        }

        bool CDistributorList::matchesAnyKeyOrAlias(const QString &keyOrAlias) const
        {
            for (const CDistributor &distributor : (*this))
            {
                if (distributor.matchesKeyOrAlias(keyOrAlias)) { return true; }
            }
            return false;
        }

        QStringList CDistributorList::getDbKeysAndAliases(bool sort) const
        {
            if (this->isEmpty()) { return QStringList(); }
            QStringList sl;
            for (const CDistributor &d : *this)
            {
                if (!d.hasValidDbKey()) { continue; }
                sl.append(d.getDbKey());
                if (d.hasAlias1())
                {
                    sl.append(d.getAlias1());
                }
                if (d.hasAlias2())
                {
                    sl.append(d.getAlias2());
                }
            }
            if (sort) { sl.sort(); }
            return sl;
        }

        CDistributorList CDistributorList::matchesSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) const
        {
            if (this->isEmpty()) { return CDistributorList(); }
            CDistributorList distributors;
            for (const CDistributor &distributor : (*this))
            {
                if (distributor.matchesSimulator(simulator))
                {
                    distributors.push_back(distributor);
                }
            }
            return distributors;
        }
    } // namespace
} // namespace
