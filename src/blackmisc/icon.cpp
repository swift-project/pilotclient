/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "icon.h"

namespace BlackMisc
{

    /*
     * Compare
     */
    int CIcon::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CIcon &>(otherBase);
        return compare(TupleConverter<CIcon>::toTuple(*this), TupleConverter<CIcon>::toTuple(other));
    }

    /*
     * Marshall to DBus
     */
    void CIcon::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CIcon>::toTuple(*this);
    }

    /*
     * Unmarshall from DBus
     */
    void CIcon::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CIcon>::toTuple(*this);
    }

    /*
     * Hash
     */
    uint CIcon::getValueHash() const
    {
        return qHash(TupleConverter<CIcon>::toTuple(*this));
    }

    /*
     * Pixmap
     */
    QPixmap CIcon::toPixmap() const
    {
        return CIcons::pixmapByIndex(getIndex(), this->m_rotateDegrees);
    }

    /*
     * Rotate
     */
    void CIcon::setRotation(const PhysicalQuantities::CAngle &rotate)
    {
        this->m_rotateDegrees = rotate.valueRounded(PhysicalQuantities::CAngleUnit::deg(), 0);
    }

    /*
     * Equal?
     */
    bool CIcon::operator ==(const CIcon &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CIcon>::toTuple(*this) == TupleConverter<CIcon>::toTuple(other);
    }

    /*
     * Unequal?
     */
    bool CIcon::operator !=(const CIcon &other) const
    {
        return !((*this) == other);
    }

    /*
     * metaTypeId
     */
    int CIcon::getMetaTypeId() const
    {
        return qMetaTypeId<CIcon>();
    }

    /*
     * is a
     */
    bool CIcon::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CIcon>()) { return true; }
        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Register metadata
     */
    void CIcon::registerMetadata()
    {
        qRegisterMetaType<CIcon>();
        qDBusRegisterMetaType<CIcon>();
    }

    /*
     * Members
     */
    const QStringList &CIcon::jsonMembers()
    {
        return TupleConverter<CIcon>::jsonMembers();
    }

    /*
     * String
     */
    QString CIcon::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s = QString(this->m_descriptiveText).append(" ").append(this->m_index);
        return s;
    }

    /*
     * To JSON
     */
    QJsonObject CIcon::toJson() const
    {
        return BlackMisc::serializeJson(CIcon::jsonMembers(), TupleConverter<CIcon>::toTuple(*this));
    }

    /*
     * From Json
     */
    void CIcon::fromJson(const QJsonObject &json)
    {
        BlackMisc::deserializeJson(json, CIcon::jsonMembers(), TupleConverter<CIcon>::toTuple(*this));
    }

} // namespace
