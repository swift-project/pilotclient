/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "logcategory.h"

namespace BlackMisc
{
    void CLogCategory::registerMetadata()
    {
        qRegisterMetaType<CLogCategory>();
        qDBusRegisterMetaType<CLogCategory>();
    }

    uint CLogCategory::getValueHash() const
    {
        return qHash(TupleConverter<CLogCategory>::toMetaTuple(*this));
    }

    bool CLogCategory::operator ==(const CLogCategory &other) const
    {
        return TupleConverter<CLogCategory>::toMetaTuple(*this) == TupleConverter<CLogCategory>::toMetaTuple(other);
    }

    bool CLogCategory::operator !=(const CLogCategory &other) const
    {
        return TupleConverter<CLogCategory>::toMetaTuple(*this) != TupleConverter<CLogCategory>::toMetaTuple(other);
    }

    QString CLogCategory::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return m_string;
    }

    int CLogCategory::getMetaTypeId() const
    {
        return qMetaTypeId<CLogCategory>();
    }

    bool CLogCategory::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CLogCategory>()) { return true; }
        return this->CValueObject::isA(metaTypeId);
    }

    int CLogCategory::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CLogCategory &>(otherBase);
        return compare(TupleConverter<CLogCategory>::toMetaTuple(*this), TupleConverter<CLogCategory>::toMetaTuple(other));
    }

    void CLogCategory::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CLogCategory>::toMetaTuple(*this);
    }

    void CLogCategory::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CLogCategory>::toMetaTuple(*this);
    }
}
