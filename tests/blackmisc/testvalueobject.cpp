/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testvalueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    /*
     * Convert to string
     */
    QString CTestValueObject::convertToQString(bool /*i18n*/) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_description);
        return s;
    }

    /*
     * metaTypeId
     */
    int CTestValueObject::getMetaTypeId() const
    {
        return qMetaTypeId<CTestValueObject>();
    }

    /*
     * is a
     */
    bool CTestValueObject::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CTestValueObject>()) { return true; }

        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Compare
     */
    int CTestValueObject::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CTestValueObject &>(otherBase);

        return compare(TupleConverter<CTestValueObject>::toTuple(*this), TupleConverter<CTestValueObject>::toTuple(other));
    }

    /*
     * Marshall to DBus
     */
    void CTestValueObject::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CTestValueObject>::toTuple(*this);
    }

    /*
     * Unmarshall from DBus
     */
    void CTestValueObject::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CTestValueObject>::toTuple(*this);
    }

    /*
     * Equal?
     */
    bool CTestValueObject::operator ==(const CTestValueObject &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CTestValueObject>::toTuple(*this) == TupleConverter<CTestValueObject>::toTuple(other);
    }

    /*
     * Unequal?
     */
    bool CTestValueObject::operator !=(const CTestValueObject &other) const
    {
        return !((*this) == other);
    }

    /*
     * Less than?
     */
    bool CTestValueObject::operator <(const CTestValueObject &other) const
    {
        if (this == &other) return false;
        return TupleConverter<CTestValueObject>::toTuple(*this) < TupleConverter<CTestValueObject>::toTuple(other);
    }

    /*
     * Hash
     */
    uint CTestValueObject::getValueHash() const
    {
        return qHash(TupleConverter<CTestValueObject>::toTuple(*this));
    }

    /*
     * Property by index
     */
    CVariant CTestValueObject::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return this->toCVariant(); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDescription:
            return CVariant::fromValue(this->m_description);
        case IndexName:
            return CVariant::fromValue(this->m_name);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    /*
     * Property by index (setter)
     */
    void CTestValueObject::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself())
        {
            this->convertFromCVariant(variant);
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDescription:
            this->setDescription(variant.value<QString>());
            break;
        case IndexName:
            this->setName(variant.value<QString>());
            break;
        default:
            CValueObject::setPropertyByIndex(variant, index);
            break;
        }
    }

    /*
     * Register metadata
     */
    void CTestValueObject::registerMetadata()
    {
        qRegisterMetaType<CTestValueObject>();
        qDBusRegisterMetaType<CTestValueObject>();
    }

    /*
     * Members
     */
    const QStringList &CTestValueObject::jsonMembers()
    {
        return TupleConverter<CTestValueObject>::jsonMembers();
    }

    /*
     * To JSON
     */
    QJsonObject CTestValueObject::toJson() const
    {
        return BlackMisc::serializeJson(CTestValueObject::jsonMembers(), TupleConverter<CTestValueObject>::toTuple(*this));
    }

    /*
     * From Json
     */
    void CTestValueObject::convertFromJson(const QJsonObject &json)
    {
        BlackMisc::deserializeJson(json, CTestValueObject::jsonMembers(), TupleConverter<CTestValueObject>::toTuple(*this));
    }

} // namespace
