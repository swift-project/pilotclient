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

#include <QDateTime>
#include <QtGlobal>
#include <QStringBuilder>

namespace BlackMisc
{
    namespace Db
    {
        QString IDatastoreObjectWithIntegerKey::getDbKeyAsString() const
        {
            if (m_dbKey < 0) { return QStringLiteral(""); }
            return QString::number(m_dbKey);
        }

        QString IDatastoreObjectWithIntegerKey::getDbKeyAsStringInParentheses(const QString &prefix) const
        {
            if (m_dbKey < 0) { return QStringLiteral(""); }
            return prefix % QStringLiteral("(") % QString::number(m_dbKey) % QStringLiteral(")");
        }

        void IDatastoreObjectWithIntegerKey::setDbKey(const QString &key)
        {
            bool ok;
            int k = key.toInt(&ok);
            if (!ok) { k = -1; }
            m_dbKey = k;
        }

        bool IDatastoreObjectWithIntegerKey::isLoadedFromDb() const
        {
            return this->hasValidDbKey();
        }

        bool IDatastoreObjectWithIntegerKey::matchesDbKeyState(Db::DbKeyStateFilter filter) const
        {
            if (filter == All) { return true; }
            const bool validKey = this->hasValidDbKey();
            return (validKey && filter.testFlag(Valid)) || (!validKey && filter.testFlag(Invalid));
        }

        const CIcon &IDatastoreObjectWithIntegerKey::toDatabaseIcon() const
        {
            static const CIcon empty;
            if (this->hasValidDbKey()) { return CIcon::iconByIndex(CIcons::StandardIconDatabaseKey16); }
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
            if (this->hasValidDbKey()) { return QJsonValue(m_dbKey); }
            return QJsonValue();
        }

        void IDatastoreObjectWithIntegerKey::setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            // this function is performance sensitive, as it is called for all DB data
            const int dbKey = json.value(prefix % QStringLiteral("id")).toInt(-1);
            this->setDbKey(dbKey);

            // we check 2 formats, the DB format and the backend object format
            QString timestampString(json.value(prefix % QStringLiteral("lastupdated")).toString());
            if (timestampString.isEmpty()) { timestampString = json.value(prefix % QStringLiteral("tsLastUpdated")).toString(); }
            const QDateTime ts(CDatastoreUtility::parseTimestamp(timestampString));
            this->setUtcTimestamp(ts);
        }

        bool IDatastoreObjectWithIntegerKey::existsKey(const QJsonObject &json, const QString &prefix)
        {
            const QJsonValue jv(json.value(prefix % QStringLiteral("id")));
            return !(jv.isNull() || jv.isUndefined());
        }

        CVariant IDatastoreObjectWithIntegerKey::propertyByIndex(const CPropertyIndex &index) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbIntegerKey: return CVariant::from(m_dbKey);
            case IndexDbKeyAsString: return CVariant::from(this->getDbKeyAsString());
            case IndexIsLoadedFromDb: return CVariant::from(this->hasValidDbKey());
            case IndexDatabaseIcon: return CVariant::from(this->toDatabaseIcon());
            default: break;
            }
            return CVariant();
        }

        void IDatastoreObjectWithIntegerKey::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbIntegerKey: m_dbKey = variant.toInt(); break;
            case IndexDbKeyAsString: m_dbKey = stringToDbKey(variant.toQString()); break;
            default: break;
            }
        }

        int IDatastoreObjectWithIntegerKey::comparePropertyByIndex(const CPropertyIndex &index, const IDatastoreObjectWithIntegerKey &compareValue) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbKeyAsString: // fall thru
            case IndexDbIntegerKey: return Compare::compare(m_dbKey, compareValue.getDbKey());
            case IndexDatabaseIcon: return Compare::compare(this->hasValidDbKey(), compareValue.hasValidDbKey());
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        bool IDatastoreObjectWithIntegerKey::canHandleIndex(const BlackMisc::CPropertyIndex &index)
        {
            if (ITimestampBased::canHandleIndex(index)) { return true; }
            const int i = index.frontCasted<int>();
            return (i >= static_cast<int>(IndexDbIntegerKey)) && (i <= static_cast<int>(IndexDatabaseIcon));
        }

        QJsonValue IDatastoreObjectWithStringKey::getDbKeyAsJsonValue() const
        {
            if (this->hasValidDbKey()) { return QJsonValue(m_dbKey); }
            static const QJsonValue null;
            return null;
        }

        QString IDatastoreObjectWithStringKey::getDbKeyAsStringInParentheses(const QString &prefix) const
        {
            if (m_dbKey.isEmpty()) { return ""; }
            return prefix % QStringLiteral("(") % m_dbKey % QStringLiteral(")");
        }

        bool IDatastoreObjectWithStringKey::matchesDbKeyState(Db::DbKeyStateFilter filter) const
        {
            if (filter == All) { return true; }
            return this->hasValidDbKey() && filter.testFlag(Valid);
        }

        const CIcon &IDatastoreObjectWithStringKey::toDatabaseIcon() const
        {
            static const CIcon empty;
            if (this->hasValidDbKey()) { return CIcon::iconByIndex(CIcons::StandardIconDatabaseKey16); }
            return empty;
        }

        void IDatastoreObjectWithStringKey::setKeyAndTimestampFromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            QString dbKey = json.value(prefix % QStringLiteral("id")).toString();
            QDateTime ts(CDatastoreUtility::parseTimestamp(json.value(prefix + "lastupdated").toString()));
            this->setDbKey(dbKey);
            this->setUtcTimestamp(ts);
        }

        bool IDatastoreObjectWithStringKey::existsKey(const QJsonObject &json, const QString &prefix)
        {
            const QJsonValue jv(json.value(prefix % QStringLiteral("id")));
            return !(jv.isNull() || jv.isUndefined());
        }

        CVariant IDatastoreObjectWithStringKey::propertyByIndex(const CPropertyIndex &index) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbKeyAsString: // fall thru
            case IndexDbStringKey: return CVariant::from(m_dbKey);
            case IndexDatabaseIcon: return CVariant::from(this->toDatabaseIcon());
            case IndexIsLoadedFromDb: return CVariant::from(m_loadedFromDb);
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
            case IndexDbKeyAsString:
                m_dbKey = variant.value<QString>();
                break;
            case IndexIsLoadedFromDb:
                m_loadedFromDb = variant.toBool();
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
            case IndexDbKeyAsString: // fall thru
            case IndexDbStringKey:  return m_dbKey.compare(compareValue.getDbKey());
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
            const int i = index.frontCasted<int>();
            return (i >= static_cast<int>(IndexDbStringKey)) && (i <= static_cast<int>(IndexDatabaseIcon));
        }
    }
} // namespace
