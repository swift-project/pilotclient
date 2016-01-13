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
    template <class OBJ, class CONTAINER, typename KEYTYPE>
    IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::IDatastoreObjectList()
    { }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    OBJ IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::findByKey(KEYTYPE key, const OBJ &notFound) const
    {
        return this->container().findFirstByOrDefault(&OBJ::getDbKey, key, notFound);
    }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    void IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::sortByKey()
    {
        this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getDbKey));
    }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    QList<KEYTYPE> IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::toDbKeyList() const
    {
        QList<KEYTYPE> keys;
        for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
        {
            if (!obj.hasValidDbKey()) { continue; }
            keys.append(obj.getDbKey());
        }
        return keys;
    }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    int IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::removeObjectsWithKeys(const QList<KEYTYPE> &keys)
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
        this->container() = newValues;
        return delta;
    }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    int IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::replaceOrAddObjectsByKey(const CONTAINER &container)
    {
        if (container.isEmpty()) { return 0; }
        if (this->container().isEmpty())
        {
            this->container() = container;
            return this->container().size();
        }
        CONTAINER newValues(this->container());
        const QList<KEYTYPE> keys(container.toDbKeyList());
        newValues.removeObjectsWithKeys(keys);
        newValues.push_back(container);
        int delta = newValues.size() - this->container().size();
        this->container() = newValues;
        return delta;
    }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::fromDatabaseJson(const QJsonArray &array)
    {
        CONTAINER container;
        for (const QJsonValue &value : array)
        {
            container.push_back(OBJ::fromDatabaseJson(value.toObject()));
        }
        return container;
    }

    template <class OBJ, class CONTAINER, typename KEYTYPE>
    CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::fromDatabaseJson(const QJsonArray &array, const QString &prefix)
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
    template class IDatastoreObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, int>;
    template class IDatastoreObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, int>;
    template class IDatastoreObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, int>;
    template class IDatastoreObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int>;
    template class IDatastoreObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, QString>;
    template class IDatastoreObjectList<BlackMisc::CCountry, BlackMisc::CCountryList, QString>;

} // namespace
