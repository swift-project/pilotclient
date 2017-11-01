/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/country.h"
#include "blackmisc/icons.h"
#include "blackmisc/stringutils.h"
#include <QJsonValue>
#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    CCountry::CCountry(const QString &iso, const QString &name) :
        IDatastoreObjectWithStringKey(iso.trimmed().toUpper()),
        m_name(name.trimmed())
    {
        this->setSimplifiedNameIfNotSame();
    }

    CIcon CCountry::toIcon() const
    {
        if (this->m_dbKey.length() == 2)
        {
            // relative to images
            return CIcon("flags/" + m_dbKey.toLower() + ".png", this->convertToQString());
        }
        else
        {
            return CIcon::iconByIndex(CIcons::StandardIconEmpty);
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

    bool CCountry::hasIsoCode() const
    {
        return m_dbKey.length() == 2;
    }

    bool CCountry::hasIso3Code() const
    {
        return m_iso3.length() == 3;
    }

    void CCountry::setAlias1(const QString &alias)
    {
        m_alias1 = alias.trimmed().toUpper();
    }

    void CCountry::setAlias2(const QString &alias)
    {
        m_alias2 = alias.trimmed().toUpper();
    }

    QString CCountry::getCombinedStringIsoName() const
    {
        if (!this->hasIsoCode()) { return QString(); }
        QString s(m_dbKey);
        if (this->m_name.isEmpty()) { return s; }
        s.append(" (").append(m_name).append(")");
        return s;
    }

    QString CCountry::getCombinedStringNameIso() const
    {
        if (!this->isValid()) { return QString(); }
        QString s(m_name);
        s.append(" - ").append(m_dbKey);
        return s;
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

    bool CCountry::isValid() const
    {
        return m_dbKey.length() == 2 && !m_name.isEmpty();
    }

    QString CCountry::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return this->getCombinedStringIsoName();
    }

    CVariant CCountry::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode:
            return CVariant::fromValue(m_dbKey);
        case IndexIso3Code:
            return CVariant::fromValue(getIso3Code());
        case IndexName:
            return CVariant::fromValue(m_name);
        case IndexIsoName:
            return CVariant::fromValue(getCombinedStringIsoName());
        case IndexAlias1:
            return CVariant::fromValue(this->getAlias1());
        case IndexAlias2:
            return CVariant::fromValue(this->getAlias2());
        case IndexHistoric:
            return CVariant::fromValue(this->isHistoric());
        default:
            return (IDatastoreObjectWithStringKey::canHandleIndex(index)) ?
                   IDatastoreObjectWithStringKey::propertyByIndex(index) :
                   CValueObject::propertyByIndex(index);
        }
    }

    void CCountry::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.to<CCountry>(); return; }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode:
            this->setIsoCode(variant.toQString());
            break;
        case IndexIso3Code:
            this->setIso3Code(variant.toQString());
            break;
        case IndexName:
            this->setName(variant.toQString());
            break;
        case IndexAlias1:
            this->setAlias1(variant.toQString());
            break;
        case IndexAlias2:
            this->setAlias1(variant.toQString());
            break;
        case IndexHistoric:
            this->setHistoric(variant.toBool());
            break;
        default:
            IDatastoreObjectWithStringKey::canHandleIndex(index) ?
            IDatastoreObjectWithStringKey::setPropertyByIndex(index, variant) :
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }

    int CCountry::comparePropertyByIndex(const CPropertyIndex &index, const CCountry &compareValue) const
    {
        if (index.isMyself()) { return getIsoCode().compare(compareValue.getIsoCode(), Qt::CaseInsensitive); }
        if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { return IDatastoreObjectWithStringKey::comparePropertyByIndex(index, compareValue);}
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode:
            return getIsoCode().compare(compareValue.getIsoCode(), Qt::CaseInsensitive);
        case IndexIso3Code:
            return getIso3Code().compare(compareValue.getIsoCode(), Qt::CaseInsensitive);
        case IndexName:
            return getName().compare(compareValue.getName(), Qt::CaseInsensitive);
        case IndexAlias1:
            return this->getAlias1().compare(compareValue.getAlias1(), Qt::CaseInsensitive);
        case IndexAlias2:
            return this->getAlias2().compare(compareValue.getAlias2(), Qt::CaseInsensitive);
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison possible");
        }
        return 0;
    }

    CCountry CCountry::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        if (!existsKey(json, prefix))
        {
            // when using relationship, this can be null
            return CCountry();
        }
        const QString iso(json.value(prefix + "id").toString());
        const QString name(json.value(prefix + "country").toString());
        const QString alias1(json.value(prefix + "alias1").toString());
        const QString alias2(json.value(prefix + "alias2").toString());
        const QString iso3(json.value(prefix + "iso3").toString());
        const QString historic(json.value(prefix + "historic").toString());
        CCountry country(iso, name);
        country.setLoadedFromDb(true);
        country.setAlias1(alias1);
        country.setAlias2(alias2);
        country.setIso3Code(iso3);
        country.setHistoric(stringToBool(historic));
        country.setKeyAndTimestampFromDatabaseJson(json, prefix);
        return country;
    }

    bool CCountry::isValidIsoCode(const QString &isoCode)
    {
        return isoCode.length() == 2;
    }

    void CCountry::setSimplifiedNameIfNotSame()
    {
        const QString simplified = removeAccents(this->m_name);
        this->m_simplifiedName = this->m_name == simplified ? "" : simplified;
    }
} // namespace
