// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DATASTORE_H
#define SWIFT_MISC_DB_DATASTORE_H

#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QString>
#include <QVersionNumber>

#include "config/buildconfig.h"
#include "misc/propertyindex.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"

namespace swift::misc
{
    class CIcon;
    namespace db
    {
        //! State of key (in DB, ...?)
        enum DbKeyState
        {
            Undefined = 0,
            Valid = 1 << 0,
            Invalid = 1 << 1,
            All = Valid | Invalid
        };

        //! Supposed to be used only in filter operations
        Q_DECLARE_FLAGS(DbKeyStateFilter, DbKeyState)

        /*!
         * Class from which a derived class can inherit datastore-related functions.
         */
        class SWIFT_MISC_EXPORT IDatastoreObject : public ITimestampBased
        {
            // since we use different keys all the compares, set, get are in the derived class
            // in general we can say, it was a bad decision to use different key types
            // IndexDbIntegerKey = CPropertyIndexRef::GlobalIndexIDatastore for future usage

        public:
            //! Version info
            const QString &getVersion() const { return m_version; }

            //! Version as QVersion
            QVersionNumber getQVersion() const;

            //! Having a version?s
            bool hasVersion() const { return !m_version.isEmpty(); }

            //! Version info
            void setVersion(const QString &version) { m_version = version; }

        protected:
            //! Set versionn and timestamp values
            void setTimestampVersionFromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

            QString m_version; //!< version info
        };

        /*!
         * Class from which a derived class can inherit datastore-related functions.
         */
        class SWIFT_MISC_EXPORT IDatastoreObjectWithIntegerKey : public IDatastoreObject
        {
        public:
            //! Property index
            enum ColumnIndex
            {
                IndexDbIntegerKey = CPropertyIndexRef::GlobalIndexIDatastoreInteger,
                IndexDbKeyAsString,
                IndexIsLoadedFromDb,
                IndexDatabaseIcon,
                IndexVersion,
                IndexEndMarker //!< keep as last element
            };

            //! Get DB key.
            int getDbKey() const { return m_dbKey; }

            //! DB key as string
            QString getDbKeyAsString() const;

            //! Key as JSON value, or null
            QJsonValue getDbKeyAsJsonValue() const;

            //! Db key in parentheses, e.g. "(3)"
            QString getDbKeyAsStringInParentheses(const QString &prefix = {}) const;

            //! Set the DB key
            void setDbKey(int key) { m_dbKey = key; }

            //! DB key passed as string
            void setDbKey(const QString &key);

            //! Has valid DB key
            bool hasValidDbKey() const { return m_dbKey >= 0; }

            //! Same DB key and hence equal
            bool isDbEqual(const IDatastoreObjectWithIntegerKey &other) const
            {
                return other.isLoadedFromDb() && this->isLoadedFromDb() && other.getDbKey() == this->getDbKey();
            }

            //! Loaded from DB
            //! \remarks here not really needed, but added to have similar signature as IDatastoreObjectWithStringKey
            bool isLoadedFromDb() const;

            //! Matches filter?
            bool matchesDbKeyState(db::DbKeyStateFilter filter) const;

            //! Database icon if this has valid key, otherwise empty
            const CIcon &toDatabaseIcon() const;

            //! Invalid key
            static int invalidDbKey() { return -1; }

            //! Convert string to DB key
            static int stringToDbKey(const QString &candidate);

            //! The key index
            static const CPropertyIndex &keyIndex()
            {
                static const CPropertyIndex k(IndexDbIntegerKey);
                return k;
            }

        protected:
            //! Constructor
            IDatastoreObjectWithIntegerKey() {}

            //! Constructor
            IDatastoreObjectWithIntegerKey(int key) : m_dbKey(key) {}

            //! Set key and timestamp values
            void setKeyVersionTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

            //! Is a key available?
            static bool existsKey(const QJsonObject &json, const QString &prefix = QString());

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index,
                                       const IDatastoreObjectWithIntegerKey &compareValue) const;

            //! Can given index be handled?
            static bool canHandleIndex(swift::misc::CPropertyIndexRef index);

            int m_dbKey = -1; //!< key
        };

        /*!
         * Class from which a derived class can inherit datastore-related functions.
         */
        class SWIFT_MISC_EXPORT IDatastoreObjectWithStringKey : public IDatastoreObject
        {
        public:
            //! Property index
            enum ColumnIndex
            {
                IndexDbStringKey = CPropertyIndexRef::GlobalIndexIDatastoreString,
                IndexDbKeyAsString,
                IndexIsLoadedFromDb,
                IndexDatabaseIcon,
                IndexVersion,
                IndexEndMarker //!< keep as last element
            };

            //! Get DB key.
            const QString &getDbKey() const { return m_dbKey; }

            //! DB key as string
            QString getDbKeyAsString() const { return getDbKey(); }

            //! Key as JSON value, or null
            QJsonValue getDbKeyAsJsonValue() const;

            //! Db key in parentheses, e.g. "(3)"
            QString getDbKeyAsStringInParentheses(const QString &prefix = {}) const;

            //! Set the DB key
            void setDbKey(const QString &key) { m_dbKey = key.trimmed().toUpper(); }

            //! Has valid DB key
            bool hasValidDbKey() const { return !m_dbKey.isEmpty(); }

            //! Same DB key and hence equal
            bool isDbEqual(const IDatastoreObjectWithStringKey &other) const
            {
                return other.isLoadedFromDb() && this->isLoadedFromDb() && other.getDbKey() == this->getDbKey();
            }

            //! Loaded from DB
            bool isLoadedFromDb() const { return m_loadedFromDb; }

            //! Mark as loaded from DB
            void setLoadedFromDb(bool loaded) { m_loadedFromDb = loaded; }

            //! Matches filter?
            bool matchesDbKeyState(db::DbKeyStateFilter filter) const;

            //! Database icon if this has valid key, otherwise empty
            const CIcon &toDatabaseIcon() const;

            //! Invalid key
            static QString invalidDbKey() { return {}; }

            //! The key index
            static const CPropertyIndex &keyIndex()
            {
                static const CPropertyIndex k(IndexDbStringKey);
                return k;
            }

        protected:
            //! Constructor
            IDatastoreObjectWithStringKey() {}

            //! Constructor
            IDatastoreObjectWithStringKey(const QString &key) : m_dbKey(key) {}

            //! Set key and timestamp values
            void setKeyVersionTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

            //! Is a key available?
            static bool existsKey(const QJsonObject &json, const QString &prefix = QString());

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(swift::misc::CPropertyIndexRef index,
                                       const IDatastoreObjectWithStringKey &compareValue) const;

            //! Can given index be handled
            static bool canHandleIndex(swift::misc::CPropertyIndexRef index);

            QString m_dbKey; //!< key
            bool m_loadedFromDb =
                false; //!< as we have no artificial key, it can happen key is set, but not loaded from DB
        };
    } // namespace db
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::db::DbKeyState)
Q_DECLARE_METATYPE(swift::misc::db::DbKeyStateFilter)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::db::DbKeyStateFilter)

#endif // guard
