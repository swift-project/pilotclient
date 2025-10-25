// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/db/datastore.h"

#include <QDateTime>
#include <QStringBuilder>
#include <QtGlobal>

#include "misc/comparefunctions.h"
#include "misc/db/datastoreutility.h"
#include "misc/icon.h"

namespace swift::misc::db
{
    QVersionNumber IDatastoreObject::getQVersion() const { return QVersionNumber::fromString(this->getVersion()); }

    void IDatastoreObject::setTimestampVersionFromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        // we check 2 formats, the DB format and the backend object format
        QString timestampString(json.value(prefix % u"lastupdated").toString());
        if (timestampString.isEmpty()) { timestampString = json.value(prefix % u"tsLastUpdated").toString(); }
        const QDateTime ts(CDatastoreUtility::parseTimestamp(timestampString));
        this->setUtcTimestamp(ts);

        // version
        this->setVersion(json.value(prefix % u"version").toString());
    }

    QString IDatastoreObjectWithIntegerKey::getDbKeyAsString() const
    {
        if (m_dbKey < 0) { return {}; }
        return QString::number(m_dbKey);
    }

    QString IDatastoreObjectWithIntegerKey::getDbKeyAsStringInParentheses(const QString &prefix) const
    {
        if (m_dbKey < 0) { return {}; }
        return prefix % u'(' % QString::number(m_dbKey) % u')';
    }

    void IDatastoreObjectWithIntegerKey::setDbKey(const QString &key)
    {
        bool ok {};
        const int k = key.toInt(&ok);
        m_dbKey = ok ? k : -1;
    }

    bool IDatastoreObjectWithIntegerKey::isLoadedFromDb() const { return this->hasValidDbKey(); }

    bool IDatastoreObjectWithIntegerKey::matchesDbKeyState(db::DbKeyStateFilter filter) const
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
        bool ok {};
        int k = candidate.toInt(&ok);
        return ok ? k : invalidDbKey();
    }

    QJsonValue IDatastoreObjectWithIntegerKey::getDbKeyAsJsonValue() const
    {
        if (this->hasValidDbKey()) { return { m_dbKey }; }
        return {};
    }

    void IDatastoreObjectWithIntegerKey::setKeyVersionTimestampFromDatabaseJson(const QJsonObject &json,
                                                                                const QString &prefix)
    {
        // this function is performance sensitive, as it is called for all DB data
        const int dbKey = json.value(prefix % u"id").toInt(-1);
        this->setDbKey(dbKey);
        IDatastoreObject::setTimestampVersionFromDatabaseJson(json, prefix);
    }

    bool IDatastoreObjectWithIntegerKey::existsKey(const QJsonObject &json, const QString &prefix)
    {
        const QJsonValue jv(json.value(prefix % u"id"));
        return !(jv.isNull() || jv.isUndefined());
    }

    QVariant IDatastoreObjectWithIntegerKey::propertyByIndex(CPropertyIndexRef index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbIntegerKey: return QVariant::fromValue(m_dbKey);
        case IndexDbKeyAsString: return QVariant::fromValue(this->getDbKeyAsString());
        case IndexIsLoadedFromDb: return QVariant::fromValue(this->hasValidDbKey());
        case IndexDatabaseIcon: return QVariant::fromValue(this->toDatabaseIcon());
        case IndexVersion: return QVariant::fromValue(this->getVersion());
        default: break;
        }
        return {};
    }

    void IDatastoreObjectWithIntegerKey::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbIntegerKey: m_dbKey = variant.toInt(); break;
        case IndexDbKeyAsString: m_dbKey = stringToDbKey(variant.toString()); break;
        case IndexVersion: this->setVersion(variant.toString()); break;
        default: break;
        }
    }

    int IDatastoreObjectWithIntegerKey::comparePropertyByIndex(CPropertyIndexRef index,
                                                               const IDatastoreObjectWithIntegerKey &compareValue) const
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            return ITimestampBased::comparePropertyByIndex(index, compareValue);
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbKeyAsString: // fall thru
        case IndexDbIntegerKey: return Compare::compare(m_dbKey, compareValue.getDbKey());
        case IndexDatabaseIcon: return Compare::compare(this->hasValidDbKey(), compareValue.hasValidDbKey());
        case IndexVersion: return this->getVersion().compare(compareValue.getVersion());
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }

    bool IDatastoreObjectWithIntegerKey::canHandleIndex(swift::misc::CPropertyIndexRef index)
    {
        if (ITimestampBased::canHandleIndex(index)) { return true; }
        const int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexDbIntegerKey)) && (i < static_cast<int>(IndexEndMarker));
    }

    QJsonValue IDatastoreObjectWithStringKey::getDbKeyAsJsonValue() const
    {
        if (this->hasValidDbKey()) { return { m_dbKey }; }
        static const QJsonValue null;
        return null;
    }

    QString IDatastoreObjectWithStringKey::getDbKeyAsStringInParentheses(const QString &prefix) const
    {
        if (m_dbKey.isEmpty()) { return {}; }
        return prefix % '(' % m_dbKey % ')';
    }

    bool IDatastoreObjectWithStringKey::matchesDbKeyState(db::DbKeyStateFilter filter) const
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

    void IDatastoreObjectWithStringKey::setKeyVersionTimestampFromDatabaseJson(const QJsonObject &json,
                                                                               const QString &prefix)
    {
        QString dbKey = json.value(prefix % u"id").toString();
        this->setDbKey(dbKey);
        IDatastoreObject::setTimestampVersionFromDatabaseJson(json, prefix);
    }

    bool IDatastoreObjectWithStringKey::existsKey(const QJsonObject &json, const QString &prefix)
    {
        const QJsonValue jv(json.value(prefix % u"id"));
        return !(jv.isNull() || jv.isUndefined());
    }

    QVariant IDatastoreObjectWithStringKey::propertyByIndex(CPropertyIndexRef index) const
    {
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbKeyAsString: // fall thru
        case IndexDbStringKey: return QVariant::fromValue(m_dbKey);
        case IndexDatabaseIcon: return QVariant::fromValue(this->toDatabaseIcon());
        case IndexIsLoadedFromDb: return QVariant::fromValue(m_loadedFromDb);
        case IndexVersion: return QVariant::fromValue(this->getVersion());
        default: break;
        }
        return {};
    }

    void IDatastoreObjectWithStringKey::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbStringKey:
        case IndexDbKeyAsString: m_dbKey = variant.value<QString>(); break;
        case IndexIsLoadedFromDb: m_loadedFromDb = variant.toBool(); break;
        case IndexVersion: this->setVersion(variant.toString()); break;
        default: break;
        }
    }

    int IDatastoreObjectWithStringKey::comparePropertyByIndex(CPropertyIndexRef index,
                                                              const IDatastoreObjectWithStringKey &compareValue) const
    {
        if (ITimestampBased::canHandleIndex(index))
        {
            return ITimestampBased::comparePropertyByIndex(index, compareValue);
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbKeyAsString: // fall thru
        case IndexDbStringKey: return m_dbKey.compare(compareValue.getDbKey());
        case IndexDatabaseIcon: return Compare::compare(this->hasValidDbKey(), compareValue.hasValidDbKey());
        case IndexVersion: return this->getVersion().compare(compareValue.getVersion());
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }

    bool IDatastoreObjectWithStringKey::canHandleIndex(CPropertyIndexRef index)
    {
        if (index.isEmpty()) { return false; }
        if (ITimestampBased::canHandleIndex(index)) { return true; }
        const int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexDbStringKey)) && (i < static_cast<int>(IndexEndMarker));
    }
} // namespace swift::misc::db
