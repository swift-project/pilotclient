// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_SERVERLIST_H
#define BLACKMISC_NETWORK_SERVERLIST_H

#include "blackmisc/network/server.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CServer, CServerList)

namespace BlackMisc::Network
{
    //! Value object encapsulating a list of servers.
    class BLACKMISC_EXPORT CServerList :
        public CSequence<CServer>,
        public Mixin::MetaType<CServerList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CServerList)
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
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CServerList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CServer>)

#endif // guard
