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
} // namespace
