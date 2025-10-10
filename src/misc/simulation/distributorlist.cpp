// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/distributorlist.h"

#include <tuple>

#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatorinfo.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::simulation, CDistributor, CDistributorList)

namespace swift::misc::simulation
{
    CDistributorList::CDistributorList(const CSequence<CDistributor> &other) : CSequence<CDistributor>(other) {}

    CDistributor CDistributorList::findByKeyOrAlias(const QString &keyOrAlias) const
    {
        if (keyOrAlias.isEmpty()) { return {}; }
        for (const CDistributor &distributor : (*this))
        {
            if (distributor.matchesKeyOrAlias(keyOrAlias)) { return distributor; }
        }
        return {};
    }

    CDistributor CDistributorList::findByModelData(const CAircraftModel &model) const
    {
        // some stuipd hardcoded resolutions for distributors
        if (model.getDistributor().hasValidDbKey()) { return model.getDistributor(); }
        if (model.getModelString().startsWith("WOA", Qt::CaseInsensitive)) { return this->findByKeyOrAlias("WOAI"); }
        if (model.getDescription().contains("WOA", Qt::CaseInsensitive)) { return this->findByKeyOrAlias("WOAI"); }
        if (model.getDescription().contains("IVAO", Qt::CaseInsensitive)) { return this->findByKeyOrAlias("IVAO"); }
        if (model.getModelString().startsWith("PAI ", Qt::CaseInsensitive)) { return this->findByKeyOrAlias("PAI"); }
        if (model.getDescription().startsWith("Project AI ", Qt::CaseInsensitive))
        {
            return this->findByKeyOrAlias("PAI");
        }

        return {};
    }

    CDistributorList CDistributorList::findFsFamilyStandard() const
    {
        return this->findByKeys(CDistributor::standardAllFsFamily());
    }

    CDistributor CDistributorList::smartDistributorSelector(const CDistributor &distributor) const
    {
        // key is not necessarily a DB key, so use complete data, happens when key is set from raw data
        if (distributor.isLoadedFromDb()) { return distributor; }
        if (distributor.hasValidDbKey())
        {
            const QString key(distributor.getDbKey());
            CDistributor d(this->findByKey(key));
            if (d.hasCompleteData()) { return d; }

            // more lenient search
            return this->findByKeyOrAlias(key);
        }
        return {};
    }

    CDistributor CDistributorList::smartDistributorSelector(const CDistributor &distributorPattern,
                                                            const CAircraftModel &model) const
    {
        const CDistributor d = this->smartDistributorSelector(distributorPattern);
        // key is not necessarily a DB key, so use complete data, happens when key is set from raw data
        if (d.hasCompleteData()) { return d; }
        return this->findByModelData(model);
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
        if (this->isEmpty()) { return {}; }
        QStringList sl;
        for (const CDistributor &d : *this)
        {
            if (!d.hasValidDbKey()) { continue; }
            sl.append(d.getDbKey());
            if (d.hasAlias1()) { sl.append(d.getAlias1()); }
            if (d.hasAlias2()) { sl.append(d.getAlias2()); }
        }
        if (sort) { sl.sort(); }
        return sl;
    }

    CDistributorList CDistributorList::matchesSimulator(const CSimulatorInfo &simulator) const
    {
        if (this->isEmpty()) { return {}; }
        CDistributorList distributors;
        for (const CDistributor &distributor : (*this))
        {
            if (distributor.matchesSimulator(simulator)) { distributors.push_back(distributor); }
        }
        return distributors;
    }

    CDistributorList CDistributorList::matchesAnyFsFamily() const
    {
        return matchesSimulator(CSimulatorInfo::AllFsFamily);
    }

    bool CDistributorList::isCompletelyFromDb() const { return !this->contains(&CDistributor::isLoadedFromDb, false); }

    int CDistributorList::removeIfNotLoadedFromDb() { return this->removeIf(&CDistributor::isLoadedFromDb, false); }
} // namespace swift::misc::simulation
