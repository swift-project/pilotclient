// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_SERVERLIST_H
#define SWIFT_MISC_NETWORK_SERVERLIST_H

#include <QMetaType>
#include <QString>

#include "misc/collection.h"
#include "misc/network/server.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CServer, CServerList)

namespace swift::misc::network
{
    //! Value object encapsulating a list of servers.
    class SWIFT_MISC_EXPORT CServerList :
        public CSequence<CServer>,
        public mixin::MetaType<CServerList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CServerList)
        using CSequence::CSequence;

        //! Default constructor.
        CServerList();

        //! Construct from a base class object.
        CServerList(const CSequence<CServer> &other);

        //! Contains name
        bool containsName(const QString &name) const;

        //! Remove by name
        bool removeByName(const QString &name);

        //! Remove passwords
        void removeUsers();

        //! Contains server with same address/port
        bool containsAddressPort(const CServer &server);

        //! Add if address not already exists
        void addIfAddressNotExists(const CServer &server);

        //! Add if address not already exists
        void addIfAddressNotExists(const CServerList &servers);

        //! Find all FSD servers
        CServerList findFsdServers() const;

        //! Set FSD setup for all entries
        void setFsdSetup(const CFsdSetup &setup);
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CServerList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CServer>)

#endif // guard
