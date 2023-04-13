/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/role.h"
#include "blackmisc/network/rolelist.h"

#include <QJsonValue>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CRole, CRoleList)

namespace BlackMisc::Network
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
