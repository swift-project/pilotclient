// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/serverlist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CServer, CServerList)

namespace swift::misc::network
{
    CServerList::CServerList() {}

    CServerList::CServerList(const CSequence<CServer> &other) : CSequence<CServer>(other)
    {}

    bool CServerList::containsName(const QString &name) const
    {
        for (const CServer &s : *this)
        {
            if (s.matchesName(name)) { return true; }
        }
        return false;
    }

    bool CServerList::removeByName(const QString &name)
    {
        if (name.isEmpty()) { return false; }
        const CServerList copy(*this);
        bool removed = false;
        for (const CServer &server : copy)
        {
            if (!server.matchesName(name)) { continue; }
            this->remove(server);
            removed = true;
        }
        return removed;
    }

    void CServerList::removeUsers()
    {
        for (CServer &s : *this)
        {
            s.setUser(CUser());
        }
    }

    bool CServerList::containsAddressPort(const CServer &server)
    {
        for (const CServer &s : *this)
        {
            if (s.matchesAddressPort(server)) { return true; }
        }
        return false;
    }

    void CServerList::addIfAddressNotExists(const CServer &server)
    {
        if (!server.hasAddressAndPort() || server.getName().isEmpty()) { return; }
        if (!this->containsAddressPort(server))
        {
            this->push_back(server);
        }
    }

    void CServerList::addIfAddressNotExists(const CServerList &servers)
    {
        for (const CServer &s : servers)
        {
            this->addIfAddressNotExists(s);
        }
    }

    CServerList CServerList::findFsdServers() const
    {
        CServerList fsdServers;
        for (const CServer &s : *this)
        {
            if (s.isFsdServer()) { fsdServers.push_back(s); }
        }
        return fsdServers;
    }

    void CServerList::setFsdSetup(const CFsdSetup &setup)
    {
        for (CServer &s : *this)
        {
            s.setFsdSetup(setup);
        }
    }
} // namespace
