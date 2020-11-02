/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/db/dbinfolist.h"
#include "blackmisc/db/artifactlist.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/json.h"

#include <QJsonObject>
#include <QJsonValue>
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace BlackMisc
{
    namespace Db
    {
        template <class OBJ, class CONTAINER, typename KEYTYPE>
        IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::IDatastoreObjectList()
        {
            constexpr bool hasIntegerKey = std::is_base_of<IDatastoreObjectWithIntegerKey, OBJ>::value && std::is_same<int, KEYTYPE>::value;
            constexpr bool hasStringKey = std::is_base_of<IDatastoreObjectWithStringKey, OBJ>::value && std::is_base_of<QString, KEYTYPE>::value;
            static_assert(hasIntegerKey || hasStringKey, "ObjectType needs to implement IDatastoreObjectWithXXXXKey and have appropriate KeyType");
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        OBJ IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::findByKey(KEYTYPE key, const OBJ &notFound) const
        {
            return this->container().findFirstByOrDefault(&OBJ::getDbKey, key, notFound);
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::findByKeys(const QSet<KEYTYPE> &keys) const
        {
            CONTAINER objects;
            if (keys.isEmpty()) { return objects; }
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!keys.contains(obj.getDbKey())) { continue; }
                objects.push_back(obj);
            }
            return objects;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::findObjectsWithDbKey() const
        {
            CONTAINER objects;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                objects.push_back(obj);
            }
            return objects;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::findObjectsWithoutDbKey() const
        {
            CONTAINER objects;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (obj.hasValidDbKey()) { continue; }
                objects.push_back(obj);
            }
            return objects;
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        OBJ IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::maxKeyObject() const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            const OBJ max = *std::max_element(this->container().begin(), this->container().end(), [](const OBJ & obj1, const OBJ & obj2)
            {
                bool v1 = obj1.hasValidDbKey();
                bool v2 = obj2.hasValidDbKey();
                if (v1 && v2)
                {
                    return obj1.getDbKey() < obj2.getDbKey();
                }
                return v2;
            });
            return max;
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        void IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::sortByKey()
        {
            this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getDbKey));
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        QSet<KEYTYPE> IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::toDbKeySet() const
        {
            QSet<KEYTYPE> keys;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                keys.insert(obj.getDbKey());
            }
            return keys;
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        QMap<KEYTYPE, OBJ> IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::toDbKeyValueMap() const
        {
            QMap<KEYTYPE, OBJ> map;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                map.insert(obj.getDbKey(), obj);
            }
            return map;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        QSet<QString> IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::toDbKeyStringSet() const
        {
            QSet<QString> keys;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                keys.insert(obj.getDbKeyAsString());
            }
            return keys;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        QString IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::dbKeysAsString(const QString &separator) const
        {
            if (ITimestampObjectList<OBJ, CONTAINER>::container().isEmpty()) { return {}; }
            const QSet<QString> keys = IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::toDbKeyStringSet();
            QString s;
            for (const QString &k : keys)
            {
                if (s.isEmpty())
                {
                    s += k;
                }
                else
                {
                    s += separator + k;
                }
            }
            return s;
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        KEYTYPE IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::getMaxKey(bool *ok) const
        {
            QSet<KEYTYPE> keys(this->toDbKeySet());
            if (keys.isEmpty())
            {
                if (ok) { *ok = false; }
                return KEYTYPE();
            }
            KEYTYPE max = *std::max_element(keys.begin(), keys.end());
            if (ok) { *ok = true; }
            return max;
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        int IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::removeObjectsWithKeys(const QSet<KEYTYPE> &keys)
        {
            if (keys.isEmpty()) { return 0; }
            if (this->container().isEmpty()) { return 0; }
            CONTAINER newValues;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (keys.contains(obj.getDbKey())) { continue; }
                newValues.push_back(obj);
            }
            const int delta = this->container().size() - newValues.size();
            if (delta > 0) { this->container() = newValues; }
            return delta;
        }

        template <class OBJ, class CONTAINER, typename KEYTYPE>
        int IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::removeObjectsWithoutDbKey()
        {
            if (this->container().isEmpty()) { return 0; }
            CONTAINER newValues;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
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
            const QSet<KEYTYPE> keys(container.toDbKeySet());
            newValues.removeObjectsWithKeys(keys);
            int removeSize = newValues.size(); // size after removing data
            newValues.push_back(container);
            this->container() = newValues;
            return newValues.size() - removeSize;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        QDateTime IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::oldestDbTimestamp() const
        {
            CONTAINER objs(this->container());
            objs.removeObjectsWithoutDbKey();
            if (objs.isEmpty()) { return QDateTime(); }
            return objs.oldestDbTimestamp();
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        int IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::countWithValidDbKey(bool withKey) const
        {
            int count = 0;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (obj.hasValidDbKey() && withKey) { count++; }
            }
            return count;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        int IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::countWithValidDbKey() const
        {
            return this->countWithValidDbKey(true);
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        bool IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::containsAnyObjectWithoutKey() const
        {
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { return true; }
            }
            return false;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        bool IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::containsDbKey(KEYTYPE key) const
        {
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey())  { continue; }
                if (obj.getDbKey() == key) { return true; }
            }
            return false;
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::fromMultipleJsonFormats(const QJsonObject &jsonObject)
        {
            // also accept cache format
            if (jsonObject.isEmpty())
            {
                const CONTAINER c;
                return c;
            }

            // cache or settings format?
            if (Json::looksLikeSwiftDataObjectJson(jsonObject))
            {
                const QJsonObject cacheObj = Json::swiftDataObjectValue(jsonObject);
                CONTAINER container;
                Private::CValueObjectMetaInfoHelper::convertFromMemoizedJson(cacheObj, container, true, 0); // handles both, memoized or "normal" convertFromJson
                return container;
            }

            // plain vanilla container, does not match memoized objects
            if (Json::looksLikeSwiftContainerJson(jsonObject))
            {
                CONTAINER container;
                container.convertFromJson(jsonObject);
                return container;
            }

            // still as type/value pair
            if (Json::looksLikeSwiftTypeValuePairJson(jsonObject))
            {
                const QJsonObject valueObject = jsonObject.value("value").toObject();
                CONTAINER container;
                Private::CValueObjectMetaInfoHelper::convertFromMemoizedJson(valueObject, container, true, 0); // handles both, memoized or "normal" convertFromJson
                return container;
            }

            // DB format, as array
            if (jsonObject.contains("data"))
            {
                return IDatastoreObjectList::fromDatabaseJson(jsonObject.value("data").toArray());
            }

            // no idea what this is
            throw CJsonException("Unsupported JSON format");
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        CONTAINER IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::fromMultipleJsonFormats(const QString &jsonString)
        {
            // also accept cache format
            if (jsonString.isEmpty())
            {
                const CONTAINER c;
                return c;
            }

            const QJsonObject jo = Json::jsonObjectFromString(jsonString, false);
            return IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::fromMultipleJsonFormats(jo);
        }

        template<class OBJ, class CONTAINER, typename KEYTYPE>
        QDateTime IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::latestDbTimestamp() const
        {
            CONTAINER objs(this->container());
            objs.removeObjectsWithoutDbKey();
            if (objs.isEmpty()) { return QDateTime(); }
            return objs.latestTimestamp();
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
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAircraftCategory, BlackMisc::Aviation::CAircraftCategoryList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Db::CDbInfo, BlackMisc::Db::CDbInfoList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Db::CArtifact, BlackMisc::Db::CArtifactList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Db::CDistribution, BlackMisc::Db::CDistributionList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, QString>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::CCountry, BlackMisc::CCountryList, QString>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList, int>;
        //! \endcond
    } // ns
} // ns
