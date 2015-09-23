/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/datastore.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{

    QString IDatastoreObjectWithIntegerKey::getDbKeyAsString() const
    {
        if (this->m_dbKey < 0) { return ""; }
        return QString::number(this->m_dbKey);
    }

    QString IDatastoreObjectWithIntegerKey::getDbKeyAsStringInParentheses() const
    {
        if (this->m_dbKey < 0) { return ""; }
        return "(" + QString::number(this->m_dbKey) + ")";
    }

    void IDatastoreObjectWithIntegerKey::setDbKey(const QString &key)
    {
        bool ok;
        int k = key.toInt(&ok);
        if (!ok) { k = -1; }
        this->m_dbKey = k;
    }

    void IDatastoreObjectWithIntegerKey::setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        int dbKey = json.value(prefix + "id").toInt(-1);
        QString timestampString(json.value(prefix + "lastupdated").toString());
        QDateTime ts(CDatastoreUtility::parseTimestamp(timestampString));
        this->setDbKey(dbKey);
        this->setUtcTimestamp(ts);
    }

    bool IDatastoreObjectWithIntegerKey::existsKey(const QJsonObject &json, const QString &prefix)
    {
        return !json.value(prefix + "id").isNull();
    }

    CVariant IDatastoreObjectWithIntegerKey::propertyByIndex(const CPropertyIndex &index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbIntegerKey: return CVariant::from(this->m_dbKey);
        default:
            break;
        }
        return CVariant();
    }

    void IDatastoreObjectWithIntegerKey::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(variant, index);
            return;
        }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbIntegerKey:
            this->m_dbKey = variant.toInt();
            break;
        default:
            break;
        }
    }

    bool IDatastoreObjectWithIntegerKey::canHandleIndex(const BlackMisc::CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index)) { return true;}
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexDbIntegerKey)) && (i <= static_cast<int>(IndexDbIntegerKey));
    }

    void IDatastoreObjectWithStringKey::setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        QString dbKey = json.value(prefix + "id").toString();
        QDateTime ts(CDatastoreUtility::parseTimestamp(json.value(prefix + "lastupdated").toString()));
        this->setDbKey(dbKey);
        this->setUtcTimestamp(ts);
    }

    bool IDatastoreObjectWithStringKey::existsKey(const QJsonObject &json, const QString &prefix)
    {
        return !json.value(prefix + "id").isNull();
    }

    CVariant IDatastoreObjectWithStringKey::propertyByIndex(const CPropertyIndex &index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbStringKey: return CVariant::from(this->m_dbKey);
        default:
            break;
        }
        return CVariant();
    }

    void IDatastoreObjectWithStringKey::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(variant, index);
            return;
        }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbStringKey:
            this->m_dbKey = variant.value<QString>();
            break;
        default:
            break;
        }
    }

    bool IDatastoreObjectWithStringKey::canHandleIndex(const CPropertyIndex &index)
    {
        if (ITimestampBased::canHandleIndex(index)) { return true;}
        int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexDbStringKey)) && (i <= static_cast<int>(IndexDbStringKey));
    }

} // namespace
