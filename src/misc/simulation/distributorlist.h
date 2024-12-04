// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_DISTRIBUTORLIST_H
#define SWIFT_MISC_SIMULATION_DISTRIBUTORLIST_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/collection.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/orderablelist.h"
#include "misc/sequence.h"
#include "misc/simulation/distributor.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation, CDistributor, CDistributorList)

namespace swift::misc::simulation
{
    class CAircraftModel;
    class CSimulatorModel;

    //! Value object encapsulating a list of distributors.
    class SWIFT_MISC_EXPORT CDistributorList :
        public CSequence<CDistributor>,
        public db::IDatastoreObjectList<CDistributor, CDistributorList, QString>,
        public IOrderableList<CDistributor, CDistributorList>,
        public mixin::MetaType<CDistributorList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CDistributorList)
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
        CDistributor smartDistributorSelector(const CDistributor &distributorPattern,
                                              const CAircraftModel &model) const;

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
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CDistributorList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::CDistributor>)

#endif // SWIFT_MISC_SIMULATION_DISTRIBUTORLIST_H
