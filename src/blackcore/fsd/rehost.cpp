/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "rehost.h"
#include "serializer.h"

#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore::Fsd
{
    Rehost::Rehost() : MessageBase()
    { }

    Rehost::Rehost(const QString &sender, const QString &hostname)
        : MessageBase(sender, {}),
            m_hostname(hostname)
    { }

    QStringList Rehost::toTokens() const
    {
        return { m_sender, m_hostname };
    }

    Rehost Rehost::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 2)
        {
            CLogMessage(static_cast<Rehost *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        return Rehost(tokens[0], tokens[1]);
    }
}
