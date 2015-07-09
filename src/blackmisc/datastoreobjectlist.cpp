/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/datastoreobjectlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include <algorithm>
#include <iterator>

namespace BlackMisc
{
    template <class OBJ, class CONTAINER>
    IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::IDatastoreObjectListWithIntegerKey()
    { }

    template <class OBJ, class CONTAINER>
    IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::IDatastoreObjectListWithStringKey()
    { }

    template <class OBJ, class CONTAINER>
    OBJ IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::findByKey(int key, const OBJ &notFound ) const
    {
        return this->container().findFirstByOrDefault(&OBJ::getDbKey, key, notFound);
    }

    template <class OBJ, class CONTAINER>
    OBJ IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::findByKey(const QString &key, const OBJ &notFound) const
    {
        return this->container().findFirstByOrDefault(&OBJ::getDbKey, key, notFound);
    }

    template <class OBJ, class CONTAINER>
    void IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::sortByKey()
    {
        this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getDbKey));
    }

    template <class OBJ, class CONTAINER>
    void IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::sortByKey()
    {
        this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getDbKey));
    }

    template <class OBJ, class CONTAINER>
    CONTAINER IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::fromDatabaseJson(const QJsonArray &array)
    {
        CONTAINER container;
        for (const QJsonValue &value : array)
        {
            container.push_back(OBJ::fromDatabaseJson(value.toObject()));
        }
        return container;
    }

    template <class OBJ, class CONTAINER>
    CONTAINER IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::fromDatabaseJson(const QJsonArray &array)
    {
        CONTAINER container;
        for (const QJsonValue &value : array)
        {
            container.push_back(OBJ::fromDatabaseJson(value.toObject()));
        }
        return container;
    }

    // see here for the reason of thess forward instantiations
    // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
    template class IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    template class IDatastoreObjectListWithIntegerKey<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    template class IDatastoreObjectListWithStringKey<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;

} // namespace
