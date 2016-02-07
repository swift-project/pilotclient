/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DATASTORE_H
#define BLACKMISC_DATASTORE_H

#include "blackmiscexport.h"
#include "timestampbased.h"
#include "propertyindex.h"
#include "variant.h"

namespace BlackMisc
{
    /*!
     * Class from which a derived class can inherit datastore-related functions.
     */
    class BLACKMISC_EXPORT IDatastoreObjectWithIntegerKey : public ITimestampBased
    {
    public:
        //! Property index
        enum ColumnIndex
        {
            IndexDbIntegerKey = CPropertyIndex::GlobalIndexIDatastoreInteger
        };

        //! Get DB key.
        int getDbKey() const { return m_dbKey; }

        //! DB key as string
        QString getDbKeyAsString() const;

        //! Key as JSON value, or null
        QJsonValue getDbKeyAsJsonValue() const;

        //! Db ley in parentheses, e.g. "(3)"
        QString getDbKeyAsStringInParentheses() const;

        //! Set the DB key
        void setDbKey(int key) { m_dbKey = key; }

        //! DB key passed as string
        void setDbKey(const QString &key);

        //! Has valid DB key
        bool hasValidDbKey() const { return m_dbKey >= 0; }

        //! Invalid key
        static int invalidDbKey() { return -1; }

        //! Convert string to DB key
        static int stringToDbKey(const QString &candidate);

    protected:
        //! Constructor
        IDatastoreObjectWithIntegerKey() {}

        //! Constructor
        IDatastoreObjectWithIntegerKey(int key) : m_dbKey(key) {}

        //! Set key and timestamp values
        void setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! Is a key available?
        static bool existsKey(const QJsonObject &json, const QString &prefix = QString());

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! Compare by index
        int comparePropertyByIndex(const IDatastoreObjectWithIntegerKey &compareValue, const CPropertyIndex &index) const;

        //! Can given index be handled?
        static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

        int m_dbKey = -1; //!< key
    };

    /*!
     * Class from which a derived class can inherit datastore-related functions.
     */
    class BLACKMISC_EXPORT IDatastoreObjectWithStringKey : public ITimestampBased
    {
    public:
        //! Property index
        enum ColumnIndex
        {
            IndexDbStringKey = CPropertyIndex::GlobalIndexIDatastoreString
        };

        //! Get DB key.
        const QString &getDbKey() const { return m_dbKey; }

        //! Key as JSON value, or null
        QJsonValue getDbKeyAsJsonValue() const;

        //! Set the DB key
        void setDbKey(const QString &key) { m_dbKey = key.trimmed().toUpper(); }

        //! Has valid DB key
        bool hasValidDbKey() const { return !m_dbKey.isEmpty(); }

        //! Invalid key
        static QString invalidDbKey() { return ""; }

    protected:
        //! Constructor
        IDatastoreObjectWithStringKey() {}

        //! Constructor
        IDatastoreObjectWithStringKey(const QString &key) : m_dbKey(key) {}

        //! Set key and timestamp values
        void setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! Is a key available?
        static bool existsKey(const QJsonObject &json, const QString &prefix = QString());

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! Compare by index
        int comparePropertyByIndex(const IDatastoreObjectWithStringKey &compareValue, const BlackMisc::CPropertyIndex &index) const;

        //! Can given index be handled
        static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

        QString m_dbKey; //!< key
    };

} // namespace

#endif // guard
