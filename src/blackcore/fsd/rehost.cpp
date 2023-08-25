// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "rehost.h"
#include "serializer.h"

#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore::Fsd
{
    Rehost::Rehost() : MessageBase()
    {}

    Rehost::Rehost(const QString &sender, const QString &hostname)
        : MessageBase(sender, {}),
          m_hostname(hostname)
    {}

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
