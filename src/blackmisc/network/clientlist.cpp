// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/clientlist.h"

using namespace BlackMisc::Aviation;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CClient, CClientList)

namespace BlackMisc::Network
{
    CClientList::CClientList() {}

    CClientList::CClientList(const CSequence &other) : CSequence<CClient>(other)
    {}

    bool CClientList::hasCapability(const Aviation::CCallsign &callsign, CClient::Capability capability) const
    {
        return this->getCapabilities(callsign).testFlag(capability);
    }

    CClient::Capabilities CClientList::getCapabilities(const Aviation::CCallsign &callsign) const
    {
        if (this->isEmpty()) { return static_cast<CClient::Capabilities>(CClient::None); }
        return this->findFirstByCallsign(callsign).getCapabilities();
    }

    CClientList CClientList::filterPilotsByCallsign(const CCallsignSet &callsigns) const
    {
        CClientList filtered;
        for (const CClient &client : *this)
        {
            if (client.isAtc() || callsigns.contains(client.getCallsign()))
            {
                filtered.push_back(client);
            }
        }
        return filtered;
    }
} // namespace
