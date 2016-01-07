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
#include "blackmisc/countrylist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
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
    OBJ IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::findByKey(int key, const OBJ &notFound) const
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
    QList<int> IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::toDbKeyList() const
    {
        QList<int> keys;
        for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
        {
            if (!obj.hasValidDbKey()) { continue; }
            keys.append(obj.getDbKey());
        }
        return keys;
    }

    template <class OBJ, class CONTAINER>
    int IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::removeObjectsWithKeys(const QList<int> &keys)
    {
        if (keys.isEmpty()) { return 0; }
        if (this->container().isEmpty()) { return 0; }
        CONTAINER newValues;
        for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
        {
            if (keys.contains(obj.getDbKey())) { continue; }
            newValues.push_back(obj);
        }
        int delta = this->container().size() - newValues.size();
        *this = newValues;
        return delta;
    }

    template <class OBJ, class CONTAINER>
    int IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::removeObjectsWithKeys(const QStringList &keys)
    {
        if (keys.isEmpty()) { return 0; }
        if (this->container().isEmpty()) { return 0; }
        CONTAINER newValues;
        for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
        {
            if (keys.contains(obj.getDbKey(), Qt::CaseInsensitive)) { continue; }
            newValues.push_back(obj);
        }
        int delta = this->container().size() - newValues.size();
        *this = newValues;
        return delta;
    }

    template <class OBJ, class CONTAINER>
    void IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::sortByKey()
    {
        this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getDbKey));
    }

    template <class OBJ, class CONTAINER>
    QStringList IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::toDbKeyList() const
    {
        QStringList keys;
        for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
        {
            if (!obj.hasValidDbKey()) { continue; }
            keys.append(obj.getDbKey());
        }
        return keys;
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
    CONTAINER IDatastoreObjectListWithIntegerKey<OBJ, CONTAINER>::fromDatabaseJson(const QJsonArray &array, const QString &prefix)
    {
        CONTAINER container;
        for (const QJsonValue &value : array)
        {
            container.push_back(OBJ::fromDatabaseJson(value.toObject(), prefix));
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

    template <class OBJ, class CONTAINER>
    CONTAINER IDatastoreObjectListWithStringKey<OBJ, CONTAINER>::fromDatabaseJson(const QJsonArray &array, const QString &prefix)
    {
        CONTAINER container;
        for (const QJsonValue &value : array)
        {
            container.push_back(OBJ::fromDatabaseJson(value.toObject(), prefix));
        }
        return container;
    }

    // see here for the reason of thess forward instantiations
    // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
    template class IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    template class IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    template class IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;
    template class IDatastoreObjectListWithIntegerKey<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    template class IDatastoreObjectListWithStringKey<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    template class IDatastoreObjectListWithStringKey<BlackMisc::CCountry, BlackMisc::CCountryList>;

} // namespace
