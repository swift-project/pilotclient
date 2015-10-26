/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/country.h"
#include <QPainter>

namespace BlackMisc
{

    CCountry::CCountry(const QString &iso, const QString &name) :
        IDatastoreObjectWithStringKey(iso.trimmed().toUpper()),
        m_name(name.trimmed())
    { }

    CIcon CCountry::toIcon() const
    {
        if (this->m_dbKey.length() == 2)
        {
            // relative to images
            return CIcon("flags/" + m_dbKey.toLower() + ".png",
                         this->convertToQString());
        }
        else
        {
            return CIcon::iconByIndex(CIcons::StandardIconEmpty);
        }
    }

    void CCountry::setIsoCode(const QString &iso)
    {
        m_dbKey = iso.trimmed().toUpper();
        Q_ASSERT_X(m_dbKey.length() == 2, Q_FUNC_INFO, "wromg ISO code");
    }

    bool CCountry::hasIsoCode() const
    {
        return m_dbKey.length() == 2;
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
    }

    bool CCountry::matchesCountryName(const QString &name) const
    {
        if (name.isEmpty() || m_name.isEmpty()) { return false; }
        if (name.length() < 5)
        {
            return m_name.length() == name.length() &&  m_name.startsWith(name, Qt::CaseInsensitive);
        }
        else
        {
            return m_name.contains(name, Qt::CaseInsensitive);
        }
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
        case IndexName:
            return CVariant::fromValue(m_name);
        case IndexIsoName:
            return CVariant::fromValue(getCombinedStringIsoName());
        case IndexNameIso:
            return CVariant::fromValue(getCombinedStringNameIso());
        default:
            return (IDatastoreObjectWithStringKey::canHandleIndex(index)) ?
                   IDatastoreObjectWithStringKey::propertyByIndex(index) :
                   CValueObject::propertyByIndex(index);
        }
    }

    void CCountry::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself()) { (*this) = variant.to<CCountry>(); return; }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode:
            this->setIsoCode(variant.toQString());
            break;
        case IndexName:
            this->setName(variant.toQString());
            break;
        default:
            return (IDatastoreObjectWithStringKey::canHandleIndex(index)) ?
                   IDatastoreObjectWithStringKey::setPropertyByIndex(variant, index) :
                   CValueObject::setPropertyByIndex(variant, index);
            break;
        }
    }

    int CCountry::comparePropertyByIndex(const CCountry &compareValue, const CPropertyIndex &index) const
    {
        if (index.isMyself()) { return getIsoCode().compare(compareValue.getIsoCode(), Qt::CaseInsensitive); }
        if (IDatastoreObjectWithStringKey::canHandleIndex(index)) { return IDatastoreObjectWithStringKey::comparePropertyByIndex(compareValue, index);}
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsoCode:
            return getIsoCode().compare(compareValue.getIsoCode(), Qt::CaseInsensitive);
        case IndexName:
            return getName().compare(compareValue.getName(), Qt::CaseInsensitive);
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
        QString iso(json.value(prefix + "id").toString());
        QString name(json.value(prefix + "country").toString());
        CCountry country(iso, name);
        country.setKeyAndTimestampFromDatabaseJson(json, prefix);
        return country;
    }

    bool CCountry::isValidIsoCode(const QString &isoCode)
    {
        return isoCode.length() == 2;
    }

} // namespace
