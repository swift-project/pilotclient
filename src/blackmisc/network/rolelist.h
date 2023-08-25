// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_ROLELIST_H
#define BLACKMISC_NETWORK_ROLELIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/network/role.h"
#include "blackmisc/sequence.h"

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CRole, CRoleList)

namespace BlackMisc::Network
{
    class CRole;

    //! Value object encapsulating a list of servers.
    class BLACKMISC_EXPORT CRoleList :
        public CSequence<CRole>,
        public BlackMisc::Mixin::MetaType<CRoleList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CRoleList)
        using CSequence::CSequence;

        //! Default constructor.
        CRoleList();

        //! Has role?
        bool hasRole(const QString &roleName) const;

        //! Has role?
        bool hasRole(const CRole &role) const;

        //! Has any role?
        bool hasAnyRole(const QStringList &roles) const;

        //! Construct from a base class object.
        CRoleList(const CSequence<CRole> &other);

        //! Roles as string
        QString namesAsString(const QString &separator = ", ") const;

        //! From our database JSON format
        static CRoleList fromDatabaseJson(const QJsonArray &array);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CRoleList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CRole>)

#endif // guard
