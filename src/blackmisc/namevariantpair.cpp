/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "namevariantpair.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{

    /*
     * Constructor
     */
    CNameVariantPair::CNameVariantPair(const QString &name, const CVariant &variant)
        : m_name(name), m_variant(variant)
    {  }

    /*
     * Convert to string
     */
    QString CNameVariantPair::convertToQString(bool i18n) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_variant.toString(i18n));
        return s;
    }

    /*
     * Marshall to DBus
     */
    void CNameVariantPair::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CNameVariantPair>::toTuple(*this);
    }

    /*
     * Unmarshall from DBus
     */
    void CNameVariantPair::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CNameVariantPair>::toTuple(*this);
    }

    /*
     * Equal?
     */
    bool CNameVariantPair::operator ==(const CNameVariantPair &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CNameVariantPair>::toTuple(*this) == TupleConverter<CNameVariantPair>::toTuple(other);
    }

    /*
     * Unequal?
     */
    bool CNameVariantPair::operator !=(const CNameVariantPair &other) const
    {
        return !((*this) == other);
    }

    /*
     * Hash
     */
    uint CNameVariantPair::getValueHash() const
    {
        return qHash(TupleConverter<CNameVariantPair>::toTuple(*this));
    }

    /*
     * metaTypeId
     */
    int CNameVariantPair::getMetaTypeId() const
    {
        return qMetaTypeId<CNameVariantPair>();
    }

    /*
     * is a
     */
    bool CNameVariantPair::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CNameVariantPair>()) { return true; }
        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Compare
     */
    int CNameVariantPair::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CNameVariantPair &>(otherBase);
        return compare(TupleConverter<CNameVariantPair>::toTuple(*this), TupleConverter<CNameVariantPair>::toTuple(other));
    }

    /*
     * Property by index
     */
    QVariant CNameVariantPair::propertyByIndex(int index) const
    {
        switch (index)
        {
        case IndexName:
            return QVariant(this->m_name);
        case IndexVariant:
            return this->m_variant.toQVariant();
        default:
            break;
        }

        Q_ASSERT_X(false, "CNameVariantPair", "index unknown");
        QString m = QString("no property, index ").append(QString::number(index));
        return QVariant::fromValue(m);
    }

    /*
     * Property by index (setter)
     */
    void CNameVariantPair::setPropertyByIndex(const QVariant &variant, int index)
    {
        switch (index)
        {
        case IndexName:
            this->setName(variant.value<QString>());
            break;
        case IndexVariant:
            this->m_variant = variant;
            break;
        default:
            Q_ASSERT_X(false, "CNameVariantPair", "index unknown");
            break;
        }
    }

    /*
     * Register metadata
     */
    void CNameVariantPair::registerMetadata()
    {
        qRegisterMetaType<CNameVariantPair>();
        qDBusRegisterMetaType<CNameVariantPair>();
    }

    /*
     * To JSON
     */
    QJsonObject CNameVariantPair::toJson() const
    {
        return BlackMisc::serializeJson(TupleConverter<CNameVariantPair>::toMetaTuple(*this));
    }

    /*
     * From JSON
     */
    void CNameVariantPair::fromJson(const QJsonObject &json)
    {
        BlackMisc::deserializeJson(json, TupleConverter<CNameVariantPair>::toMetaTuple(*this));
    }

    /*
     * Members
     */
    const QStringList &CNameVariantPair::jsonMembers()
    {
        return TupleConverter<CNameVariantPair>::jsonMembers();
    }

} // namespace
