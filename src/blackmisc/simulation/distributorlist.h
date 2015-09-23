/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_DISTRIBUTORLIST_H
#define BLACKMISC_SIMULATION_DISTRIBUTORLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/datastoreobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Value object encapsulating a list of distributors.
        class BLACKMISC_EXPORT CDistributorList :
            public BlackMisc::CSequence<CDistributor>,
            public BlackMisc::IDatastoreObjectListWithStringKey<CDistributor, CDistributorList>,
            public BlackMisc::Mixin::MetaType<CDistributorList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDistributorList)

            //! Default constructor.
            CDistributorList();

            //! Construct from a base class object.
            CDistributorList(const CSequence<CDistributor> &other);

            //! Find by id or alias
            CDistributor findByIdOrAlias(const QString &name);

        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CDistributorList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CDistributor>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CDistributor>)

#endif //guard
