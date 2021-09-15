/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "rawfsdmessagelist.h"

namespace BlackMisc::Network
{
    CRawFsdMessageList::CRawFsdMessageList() { }

    CRawFsdMessageList::CRawFsdMessageList(const CSequence &other) : CSequence<CRawFsdMessage>(other)
    { }

    CRawFsdMessageList CRawFsdMessageList::findByPacketType(const QString &type) const
    {
        return this->findBy([ & ](const CRawFsdMessage &rawFsdMessage)
        {
                return rawFsdMessage.isPacketType(type);
        });
    }

    CRawFsdMessageList CRawFsdMessageList::findByContainsString(const QString &str) const
    {
        return this->findBy([ & ](const CRawFsdMessage &rawFsdMessage)
        {
            return rawFsdMessage.containsString(str);
        });
    }
} // namespace
