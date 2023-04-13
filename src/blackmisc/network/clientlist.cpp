/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
