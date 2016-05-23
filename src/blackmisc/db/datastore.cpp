/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/db/datastore.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/icon.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"

#include <QDateTime>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Db
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

        const CIcon &IDatastoreObjectWithIntegerKey::toDatabaseIcon() const
        {
            static const CIcon empty;
            if (this->hasValidDbKey()) { return CIconList::iconByIndex(CIcons::StandardIconDatabaseKey16); }
            return empty;
        }

        int IDatastoreObjectWithIntegerKey::stringToDbKey(const QString &candidate)
        {
            if (candidate.isEmpty()) { return invalidDbKey(); }
            bool ok;
            int k = candidate.toInt(&ok);
            return ok ? k : invalidDbKey();
        }

        QJsonValue IDatastoreObjectWithIntegerKey::getDbKeyAsJsonValue() const
        {
            if (this->hasValidDbKey()) { return QJsonValue(this->m_dbKey); }
            return QJsonValue();
        }

        void IDatastoreObjectWithIntegerKey::setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            int dbKey = json.value(prefix + "id").toInt(-1);
            const QString timestampString(json.value(prefix + "lastupdated").toString());
            const QDateTime ts(CDatastoreUtility::parseTimestamp(timestampString));
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
            case IndexDatabaseIcon: return CVariant::from(this->toDatabaseIcon());
            default: break;
            }
            return CVariant();
        }

        void IDatastoreObjectWithIntegerKey::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
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

        int IDatastoreObjectWithIntegerKey::comparePropertyByIndex(const CPropertyIndex &index, const IDatastoreObjectWithIntegerKey &compareValue) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbIntegerKey: return Compare::compare(this->m_dbKey, compareValue.getDbKey());
            case IndexDatabaseIcon: return Compare::compare(this->hasValidDbKey(), compareValue.hasValidDbKey());
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        bool IDatastoreObjectWithIntegerKey::canHandleIndex(const BlackMisc::CPropertyIndex &index)
        {
            if (ITimestampBased::canHandleIndex(index)) { return true;}
            int i = index.frontCasted<int>();
            return (i >= static_cast<int>(IndexDbIntegerKey)) && (i <= static_cast<int>(IndexDatabaseIcon));
        }

        QJsonValue IDatastoreObjectWithStringKey::getDbKeyAsJsonValue() const
        {
            if (this->hasValidDbKey()) { return QJsonValue(this->m_dbKey); }
            static const QJsonValue null;
            return null;
        }

        const CIcon &IDatastoreObjectWithStringKey::toDatabaseIcon() const
        {
            static const CIcon empty;
            if (this->hasValidDbKey()) { return CIconList::iconByIndex(CIcons::StandardIconDatabaseKey16); }
            return empty;
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
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbStringKey: return CVariant::from(this->m_dbKey);
            case IndexDatabaseIcon: return CVariant::from(this->toDatabaseIcon());
            default:
                break;
            }
            return CVariant();
        }

        void IDatastoreObjectWithStringKey::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbStringKey:
                this->m_dbKey = variant.value<QString>();
                break;
            default:
                break;
            }
        }

        int IDatastoreObjectWithStringKey::comparePropertyByIndex(const CPropertyIndex &index, const IDatastoreObjectWithStringKey &compareValue) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbStringKey:  return this->m_dbKey.compare(compareValue.getDbKey());
            case IndexDatabaseIcon: return Compare::compare(this->hasValidDbKey(), compareValue.hasValidDbKey());
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        bool IDatastoreObjectWithStringKey::canHandleIndex(const CPropertyIndex &index)
        {
            if (index.isEmpty()) { return false; }
            if (ITimestampBased::canHandleIndex(index)) { return true;}
            int i = index.frontCasted<int>();
            return (i >= static_cast<int>(IndexDbStringKey)) && (i <= static_cast<int>(IndexDatabaseIcon));
        }
    }
} // namespace
