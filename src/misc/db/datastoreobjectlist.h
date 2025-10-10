// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DATABASEOBJECTLIST_H
#define SWIFT_MISC_DB_DATABASEOBJECTLIST_H

#include <QJsonArray>
#include <QMap>
#include <QSet>
#include <QString>

#include "misc/db/datastore.h"
#include "misc/jsonexception.h"
#include "misc/mapbuilder.h"
#include "misc/setbuilder.h"
#include "misc/timestampobjectlist.h"

namespace swift::misc::db
{
    //! List of objects read from database.
    //! Such objects should implement \sa ITimestampBased and \sa IDatastoreObjectWithIntegerKey or \sa
    //! IDatastoreObjectWithStringKey
    template <class OBJ, class CONTAINER, typename KEYTYPE>
    class IDatastoreObjectList : public ITimestampObjectList<OBJ, CONTAINER>
    {
        static constexpr bool hasIntegerKey =
            std::is_base_of_v<IDatastoreObjectWithIntegerKey, OBJ> && std::is_same_v<int, KEYTYPE>;
        static constexpr bool hasStringKey =
            std::is_base_of_v<IDatastoreObjectWithStringKey, OBJ> && std::is_base_of_v<QString, KEYTYPE>;
        static_assert(hasIntegerKey || hasStringKey,
                      "ObjectType needs to implement IDatastoreObjectWithXXXXKey and have appropriate KeyType");

    public:
        //! Object with key, notFound otherwise
        OBJ findByKey(KEYTYPE key, const OBJ &notFound = OBJ()) const
        {
            return this->container().findFirstByOrDefault(&OBJ::getDbKey, key, notFound);
        }

        //! Object with key, notFound otherwise
        CONTAINER findByKeys(const QSet<KEYTYPE> &keys) const
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

        //! Objects with DB key
        CONTAINER findObjectsWithDbKey() const
        {
            CONTAINER objects;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                objects.push_back(obj);
            }
            return objects;
        }

        //! Objects without DB key
        CONTAINER findObjectsWithoutDbKey() const
        {
            CONTAINER objects;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (obj.hasValidDbKey()) { continue; }
                objects.push_back(obj);
            }
            return objects;
        }

        //! Object with max.key
        OBJ maxKeyObject() const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            const OBJ max = *std::max_element(this->container().begin(), this->container().end(),
                                              [](const OBJ &obj1, const OBJ &obj2) {
                                                  bool v1 = obj1.hasValidDbKey();
                                                  bool v2 = obj2.hasValidDbKey();
                                                  if (v1 && v2) { return obj1.getDbKey() < obj2.getDbKey(); }
                                                  return v2;
                                              });
            return max;
        }

        //! Sort by timestamp
        void sortByKey() { this->container().sort(swift::misc::predicates::MemberLess(&OBJ::getDbKey)); }

        //! All keys as set
        QSet<KEYTYPE> toDbKeySet() const
        {
            CSetBuilder<KEYTYPE> keys;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                keys.insert(obj.getDbKey());
            }
            return keys;
        }

        //! As map with DB key/object
        QMap<KEYTYPE, OBJ> toDbKeyValueMap() const
        {
            CMapBuilder<KEYTYPE, OBJ> map;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                map.insert(obj.getDbKey(), obj);
            }
            return map;
        }

        //! All keys as string set (also integer keys will be converted to string)
        QSet<QString> toDbKeyStringSet() const
        {
            CSetBuilder<QString> keys;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                keys.insert(obj.getDbKeyAsString());
            }
            return keys;
        }

        //! The DB keys as string
        QString dbKeysAsString(const QString &separator) const
        {
            if (ITimestampObjectList<OBJ, CONTAINER>::container().isEmpty()) { return {}; }
            const QSet<QString> keys = IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::toDbKeyStringSet();
            QString s;
            for (const QString &k : keys)
            {
                if (s.isEmpty()) { s += k; }
                else { s += separator + k; }
            }
            return s;
        }

        //! Max.key value (making sense with integer key)
        KEYTYPE getMaxKey(bool *ok = nullptr) const
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

        //! Remove objects with keys
        int removeObjectsWithKeys(const QSet<KEYTYPE> &keys)
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

        //! Remove objects without key
        int removeObjectsWithoutDbKey()
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

        //! Update or insert data (based on DB key)
        int replaceOrAddObjectsByKey(const CONTAINER &container)
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

        //! Latest DB timestamp (means objects with DB key)
        QDateTime latestDbTimestamp() const
        {
            CONTAINER objs(this->container());
            objs.removeObjectsWithoutDbKey();
            if (objs.isEmpty()) { return {}; }
            return objs.latestTimestamp();
        }

        //! Latest DB timestamp (means objects with DB key)
        QDateTime oldestDbTimestamp() const
        {
            CONTAINER objs(this->container());
            objs.removeObjectsWithoutDbKey();
            if (objs.isEmpty()) { return {}; }
            return objs.oldestDbTimestamp();
        }

        //! Number of objects with/without key
        int countWithValidDbKey(bool withKey) const
        {
            int count = 0;
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (obj.hasValidDbKey() && withKey) { count++; }
            }
            return count;
        }

        //! Number of entries with valid DB key
        int countWithValidDbKey() const { return this->countWithValidDbKey(true); }

        //! Any object without key?
        bool containsAnyObjectWithoutKey() const
        {
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { return true; }
            }
            return false;
        }

        //! Contains object with key?
        bool containsDbKey(KEYTYPE key) const
        {
            for (const OBJ &obj : ITimestampObjectList<OBJ, CONTAINER>::container())
            {
                if (!obj.hasValidDbKey()) { continue; }
                if (obj.getDbKey() == key) { return true; }
            }
            return false;
        }

        //! From multiple JSON formats
        //! \remark supports native swift C++ format, DB format, and cache format
        static CONTAINER fromMultipleJsonFormats(const QJsonObject &jsonObject)
        {
            // also accept cache format
            if (jsonObject.isEmpty())
            {
                const CONTAINER c;
                return c;
            }

            // cache or settings format?
            if (json::looksLikeSwiftDataObjectJson(jsonObject))
            {
                const QJsonObject cacheObj = json::swiftDataObjectValue(jsonObject);
                CONTAINER container;
                private_ns::CValueObjectMetaInfoHelper::convertFromMemoizedJson(
                    cacheObj, container, true, 0); // handles both, memoized or "normal" convertFromJson
                return container;
            }

            // plain vanilla container, does not match memoized objects
            if (json::looksLikeSwiftContainerJson(jsonObject))
            {
                CONTAINER container;
                container.convertFromJson(jsonObject);
                return container;
            }

            // still as type/value pair
            if (json::looksLikeSwiftTypeValuePairJson(jsonObject))
            {
                const QJsonObject valueObject = jsonObject.value("value").toObject();
                CONTAINER container;
                private_ns::CValueObjectMetaInfoHelper::convertFromMemoizedJson(
                    valueObject, container, true, 0); // handles both, memoized or "normal" convertFromJson
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

        //! From multiple JSON formats
        //! \remark supports native swift C++ format, DB format, and cache format
        static CONTAINER fromMultipleJsonFormats(const QString &jsonString)
        {
            // also accept cache format
            if (jsonString.isEmpty())
            {
                const CONTAINER c;
                return c;
            }

            const QJsonObject jo = json::jsonObjectFromString(jsonString, false);
            return IDatastoreObjectList<OBJ, CONTAINER, KEYTYPE>::fromMultipleJsonFormats(jo);
        }

        //! From DB JSON with default prefixes
        //! \remark Specialized classes might have their own fromDatabaseJson implementation
        static CONTAINER fromDatabaseJson(const QJsonArray &array)
        {
            CONTAINER container;
            for (const QJsonValue &value : array) { container.push_back(OBJ::fromDatabaseJson(value.toObject())); }
            return container;
        }

        //! From DB JSON
        //! \remark Specialized classes might have their own fromDatabaseJson implementation
        static CONTAINER fromDatabaseJson(const QJsonArray &array, const QString &prefix)
        {
            CONTAINER container;
            for (const QJsonValue &value : array)
            {
                container.push_back(OBJ::fromDatabaseJson(value.toObject(), prefix));
            }
            return container;
        }

    protected:
        //! Constructor
        IDatastoreObjectList() = default;
    };
} // namespace swift::misc::db

#endif // SWIFT_MISC_DB_DATABASEOBJECTLIST_H
