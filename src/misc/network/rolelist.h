// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_ROLELIST_H
#define SWIFT_MISC_NETWORK_ROLELIST_H

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/collection.h"
#include "misc/network/role.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CRole, CRoleList)

namespace swift::misc::network
{
    class CRole;

    //! Value object encapsulating a list of servers.
    class SWIFT_MISC_EXPORT CRoleList :
        public CSequence<CRole>,
        public swift::misc::mixin::MetaType<CRoleList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CRoleList)
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
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CRoleList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CRole>)

#endif // guard
