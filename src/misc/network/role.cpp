// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/role.h"
#include <QJsonValue>
#include <QtGlobal>
#include <QStringBuilder>

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CRole)

namespace swift::misc::network
{
    CRole::CRole(const QString &name, const QString &description)
        : m_name(name), m_description(description)
    {}

    QString CRole::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return u"Role: " % m_name %
               u" description: " % m_description %
               this->getDbKeyAsStringInParentheses(" ");
    }

    QVariant CRole::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: return QVariant::fromValue(m_name);
        case IndexDescription: return QVariant::fromValue(m_description);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CRole::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CRole>();
            return;
        }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index))
        {
            IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant);
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName: this->setName(variant.value<QString>()); break;
        case IndexDescription: this->setDescription(variant.value<QString>()); break;
        default: break;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    CRole CRole::fromDatabaseJson(const QJsonObject &json)
    {
        CRole role;
        role.setName(json.value("name").toString());
        role.setDescription(json.value("description").toString());
        role.setDbKey(json.value("idrole").toInt(-1));
        return role;
    }
} // ns
