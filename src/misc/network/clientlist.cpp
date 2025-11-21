// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/clientlist.h"

using namespace swift::misc::aviation;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CClient, CClientList)

namespace swift::misc::network
{
    CClientList::CClientList(const CSequence &other) : CSequence<CClient>(other) {}
} // namespace swift::misc::network
