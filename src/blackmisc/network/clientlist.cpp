/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "clientlist.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        CClientList::CClientList() { }

        CClientList::CClientList(const CSequence &other) : CSequence<CClient>(other)
        { }

        bool CClientList::hasCapability(const Aviation::CCallsign &callsign, CClient::Capability capability) const
        {
            return this->getCapabilities(callsign).testFlag(capability);
        }

        CClient::Capabilities CClientList::getCapabilities(const Aviation::CCallsign &callsign) const
        {
            if (this->isEmpty()) { return static_cast<CClient::Capabilities>(CClient::None); }
            return this->findFirstByCallsign(callsign).getCapabilities();
        }
    } // namespace
} // namespace
