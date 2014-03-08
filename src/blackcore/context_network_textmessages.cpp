/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "coreruntime.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackCore
{
    /*
     * Radio text message received
     */
    void CContextNetwork::psFsdTextMessageReceived(const CTextMessageList &messages)
    {
        if (messages.isEmpty()) return;
        // this->log(Q_FUNC_INFO, messages.toQString());
        this->textMessagesReceived(messages); // relay
    }

} // namespace
