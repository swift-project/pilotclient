/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "roles.h"

namespace BlackGui
{
    CRoles::CRoles() { }

    bool CRoles::hasRole(const QString &role) const
    {
        return m_roles.contains(role);
    }

    bool CRoles::isAdmin() const
    {
        return m_roles.contains("ADMIN");
    }

    void CRoles::setAdmin(bool admin)
    {
        if (admin)
        {
            if (isAdmin()) { return; }
            m_roles.append("ADMIN");
        }
        else
        {
            m_roles.removeAll("ADMIN");
        }
    }

    CRoles &CRoles::roles()
    {
        static CRoles roles;
        return roles;
    }

} // roles
