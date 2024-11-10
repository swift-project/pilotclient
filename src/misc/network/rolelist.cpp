// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/role.h"
#include "misc/network/rolelist.h"

#include <QJsonValue>

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CRole, CRoleList)

namespace swift::misc::network
{
    CRoleList::CRoleList() {}

    bool CRoleList::hasRole(const QString &roleName) const
    {
        return this->contains(&CRole::getName, roleName.trimmed().toUpper());
    }

    bool CRoleList::hasRole(const CRole &role) const
    {
        return hasRole(role.getName());
    }

    bool CRoleList::hasAnyRole(const QStringList &roles) const
    {
        for (const QString &r : roles)
        {
            if (this->hasRole(r)) { return true; }
        }
        return false;
    }

    CRoleList::CRoleList(const CSequence<CRole> &other) : CSequence<CRole>(other)
    {}

    QString CRoleList::namesAsString(const QString &separator) const
    {
        QStringList rolesString;
        rolesString.reserve(size());
        for (const CRole &role : (*this))
        {
            rolesString.append(role.getName());
        }
        return rolesString.join(separator);
    }

    CRoleList CRoleList::fromDatabaseJson(const QJsonArray &array)
    {
        CRoleList roles;
        for (const QJsonValue &value : array)
        {
            roles.push_back(CRole::fromDatabaseJson(value.toObject()));
        }
        return roles;
    }

} // namespace
