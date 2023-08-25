// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/userlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CUser, CUserList)

namespace BlackMisc::Network
{
    CUserList::CUserList() {}

    CUserList::CUserList(const CSequence &other) : CSequence<CUser>(other)
    {}
} // namespace
