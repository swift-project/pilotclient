// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/rawfsdmessagelist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CRawFsdMessage, CRawFsdMessageList)

namespace swift::misc::network
{
    CRawFsdMessageList::CRawFsdMessageList() {}

    CRawFsdMessageList::CRawFsdMessageList(const CSequence &other) : CSequence<CRawFsdMessage>(other) {}

    CRawFsdMessageList CRawFsdMessageList::findByPacketType(const QString &type) const
    {
        return this->findBy([&](const CRawFsdMessage &rawFsdMessage) { return rawFsdMessage.isPacketType(type); });
    }

    CRawFsdMessageList CRawFsdMessageList::findByContainsString(const QString &str) const
    {
        return this->findBy([&](const CRawFsdMessage &rawFsdMessage) { return rawFsdMessage.containsString(str); });
    }
} // namespace swift::misc::network
