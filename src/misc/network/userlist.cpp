// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/userlist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CUser, CUserList)

namespace swift::misc::network
{
    CUserList::CUserList() {}

    CUserList::CUserList(const CSequence &other) : CSequence<CUser>(other)
    {}
} // namespace
