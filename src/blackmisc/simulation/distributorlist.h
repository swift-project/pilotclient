// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_DISTRIBUTORLIST_H
#define BLACKMISC_SIMULATION_DISTRIBUTORLIST_H

#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/orderablelist.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QString>
#include <QStringList>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Simulation, CDistributor, CDistributorList)

namespace BlackMisc::Simulation
{
    class CAircraftModel;
    class CSimulatorModel;

    //! Value object encapsulating a list of distributors.
    class BLACKMISC_EXPORT CDistributorList :
        public CSequence<CDistributor>,
        public Db::IDatastoreObjectList<CDistributor, CDistributorList, QString>,
        public IOrderableList<CDistributor, CDistributorList>,
        public Mixin::MetaType<CDistributorList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDistributorList)
        using CSequence::CSequence;

        //! Default constructor.
        CDistributorList();

        //! Construct from a base class object.
        CDistributorList(const CSequence<CDistributor> &other);

        //! Find by id or alias
        CDistributor findByKeyOrAlias(const QString &keyOrAlias) const;

        //! Find by model string
        //! \remark model strings may have a pattern which makes it impossible to find the distributor
        CDistributor findByModelData(const CAircraftModel &model) const;

        //! Find the FS family standard distributors
        CDistributorList findFsFamilyStandard() const;

        //! Best match by given pattern
        CDistributor smartDistributorSelector(const CDistributor &distributorPattern) const;

        //! Best match by given pattern
        CDistributor smartDistributorSelector(const CDistributor &distributorPattern, const CAircraftModel &model) const;

        //! At least is matching key or alias
        bool matchesAnyKeyOrAlias(const QString &keyOrAlias) const;

        //! All DB keys and aliases
        QStringList getDbKeysAndAliases(bool sort) const;

        //! Find for given simulator
        CDistributorList matchesSimulator(const CSimulatorInfo &simulator) const;

        //! Find all for all FS simulators
        CDistributorList matchesAnyFsFamily() const;

        //! All data from DB?
        bool isCompletelyFromDb() const;

        //! Remove distributors not from DB
        int removeIfNotLoadedFromDb();
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CDistributorList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CDistributor>)

#endif // guard
