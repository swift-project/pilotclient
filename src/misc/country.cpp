// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/country.h"

#include <QJsonValue>
#include <QStringBuilder>
#include <Qt>
#include <QtGlobal>

#include "misc/icons.h"
#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CCountry)

namespace swift::misc
{
    CCountry::CCountry(const QString &iso, const QString &name)
        : IDatastoreObjectWithStringKey(iso.trimmed().toUpper()), m_name(name.trimmed())
    {
        this->setSimplifiedNameIfNotSame();
    }

    CIcons::IconIndex CCountry::toIcon() const
    {
        // if (m_dbKey.length() == 2)
        //{
        //     // relative to images
        //     return CIcon(u"flags/" % m_dbKey.toLower() % u".png", this->convertToQString());
        // }
        // else
        {
            return CIcons::StandardIconEmpty;
        }
    }

    void CCountry::setIsoCode(const QString &iso)
    {
        const QString code(iso.trimmed().toUpper());
        m_dbKey = (code.length() == 2) ? code : "";
    }

    void CCountry::setIso3Code(const QString &iso)
    {
        const QString code(iso.trimmed().toUpper());
        m_iso3 = (code.length() == 3) ? code : "";
    }

    bool CCountry::hasIsoCode() const { return m_dbKey.length() == 2; }

    bool CCountry::hasIso3Code() const { return m_iso3.length() == 3; }

    void CCountry::setAlias1(const QString &alias) { m_alias1 = alias.trimmed().toUpper(); }

    void CCountry::setAlias2(const QString &alias) { m_alias2 = alias.trimmed().toUpper(); }

    QString CCountry::getCombinedStringIsoName() const
    {
        if (!this->hasIsoCode()) { return QString(); }
        QString s(m_dbKey);
        if (m_name.isEmpty()) { return s; }
        return u" (" % m_name % u')';
    }

    QString CCountry::getCombinedStringNameIso() const
    {
        if (!this->isValid()) { return QString(); }
        return m_name % u" -  " % m_dbKey;
    }

    void CCountry::setName(const QString &countryName)
    {
        m_name = countryName.trimmed();
        this->setSimplifiedNameIfNotSame();
    }

    bool CCountry::matchesCountryName(const QString &name) const
    {
        if (name.isEmpty() || m_name.isEmpty()) { return false; }
        if (caseInsensitiveStringCompare(name, this->getDbKey())) { return true; } // exact ISO match
        if (caseInsensitiveStringCompare(name, this->getIso3Code())) { return true; } // exact ISO3 match
        if (name.length() < 5)
        {
            // contains would be too fuzzy for short names
            return caseInsensitiveStringCompare(name, m_name) || caseInsensitiveStringCompare(name, m_simplifiedName);
        }
        else
        {
            return m_name.contains(name, Qt::CaseInsensitive) || m_simplifiedName.contains(name, Qt::CaseInsensitive);
        }
    }

    bool CCountry::matchesAlias(const QString &alias) const
    {
        return caseInsensitiveStringCompare(alias, m_alias1) || caseInsensitiveStringCompare(alias, m_alias2);
    }

    bool CCountry::isValid() const { return m_dbKey.length() == 2 && !m_name.isEmpty(); }

    QString CCountry::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return this->getCombinedStringIsoName();
    }

    QVariant CCountry::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode: return QVariant::fromValue(m_dbKey);
        case IndexIso3Code: return QVariant::fromValue(getIso3Code());
        case IndexName: return QVariant::fromValue(m_name);
        case IndexIsoName: return QVariant::fromValue(getCombinedStringIsoName());
        case IndexAlias1: return QVariant::fromValue(this->getAlias1());
        case IndexAlias2: return QVariant::fromValue(this->getAlias2());
        case IndexHistoric: return QVariant::fromValue(this->isHistoric());
        default:
            return (IDatastoreObjectWithStringKey::canHandleIndex(index)) ?
                       IDatastoreObjectWithStringKey::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
        }
    }

    void CCountry::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CCountry>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode: this->setIsoCode(variant.toString()); break;
        case IndexIso3Code: this->setIso3Code(variant.toString()); break;
        case IndexName: this->setName(variant.toString()); break;
        case IndexAlias1: this->setAlias1(variant.toString()); break;
        case IndexAlias2: this->setAlias2(variant.toString()); break;
        case IndexHistoric: this->setHistoric(variant.toBool()); break;
        default:
            IDatastoreObjectWithStringKey::canHandleIndex(index) ?
                IDatastoreObjectWithStringKey::setPropertyByIndex(index, variant) :
                CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }

    int CCountry::comparePropertyByIndex(CPropertyIndexRef index, const CCountry &compareValue) const
    {
        if (index.isMyself()) { return getIsoCode().compare(compareValue.getIsoCode(), Qt::CaseInsensitive); }
        if (IDatastoreObjectWithStringKey::canHandleIndex(index))
        {
            return IDatastoreObjectWithStringKey::comparePropertyByIndex(index, compareValue);
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode: return getIsoCode().compare(compareValue.getIsoCode(), Qt::CaseInsensitive);
        case IndexIso3Code: return getIso3Code().compare(compareValue.getIsoCode(), Qt::CaseInsensitive);
        case IndexName: return getName().compare(compareValue.getName(), Qt::CaseInsensitive);
        case IndexAlias1: return this->getAlias1().compare(compareValue.getAlias1(), Qt::CaseInsensitive);
        case IndexAlias2: return this->getAlias2().compare(compareValue.getAlias2(), Qt::CaseInsensitive);
        default: break;
        }
        return CValueObject::comparePropertyByIndex(index, compareValue);
    }

    CCountry CCountry::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        if (!existsKey(json, prefix))
        {
            // when using relationship, this can be null
            return CCountry();
        }
        const QString iso(json.value(prefix % u"id").toString());
        const QString name(json.value(prefix % u"country").toString());
        const QString alias1(json.value(prefix % u"alias1").toString());
        const QString alias2(json.value(prefix % u"alias2").toString());
        const QString iso3(json.value(prefix % u"iso3").toString());
        const QString historic(json.value(prefix % u"historic").toString());
        CCountry country(iso, name);
        country.setLoadedFromDb(true);
        country.setAlias1(alias1);
        country.setAlias2(alias2);
        country.setIso3Code(iso3);
        country.setHistoric(stringToBool(historic));
        country.setKeyVersionTimestampFromDatabaseJson(json, prefix);
        return country;
    }

    bool CCountry::isValidIsoCode(const QString &isoCode) { return isoCode.length() == 2; }

    void CCountry::setSimplifiedNameIfNotSame()
    {
        const QString simplified = simplifyAccents(m_name);
        m_simplifiedName = m_name == simplified ? "" : simplified;
    }
} // namespace swift::misc
