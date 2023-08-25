// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/rawfsdmessagelist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CRawFsdMessage, CRawFsdMessageList)

namespace BlackMisc::Network
{
    CRawFsdMessageList::CRawFsdMessageList() {}

    CRawFsdMessageList::CRawFsdMessageList(const CSequence &other) : CSequence<CRawFsdMessage>(other)
    {}

    CRawFsdMessageList CRawFsdMessageList::findByPacketType(const QString &type) const
    {
        return this->findBy([&](const CRawFsdMessage &rawFsdMessage) {
            return rawFsdMessage.isPacketType(type);
        });
    }

    CRawFsdMessageList CRawFsdMessageList::findByContainsString(const QString &str) const
    {
        return this->findBy([&](const CRawFsdMessage &rawFsdMessage) {
            return rawFsdMessage.containsString(str);
        });
    }
} // namespace
