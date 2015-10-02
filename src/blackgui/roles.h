/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ROLES_H
#define BLACKGUI_ROLES_H

#include "blackgui/blackguiexport.h"
#include <QStringList>

namespace BlackGui
{
    /*!
     * Roles
     */
    class BLACKGUI_EXPORT CRoles
    {
    public:
        //! Has role?
        bool hasRole(const QString &role) const;

        //! Admin?
        bool isAdmin() const;

        //! Set admin
        void setAdmin(bool admin);

        //! Roles
        static CRoles &roles();

    private:
        //! Constructor
        CRoles();

        QStringList m_roles;
    };

} // ns

#endif // guard
