// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/revbclientparts.h"
#include "core/fsd/serializer.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    RevBClientParts::RevBClientParts()
    {}

    RevBClientParts::RevBClientParts(const QString &sender, const QString &partsval1, const QString &partsval2, const QString &partsval3)
        : MessageBase(sender),
          m_partsval1(partsval1),
          m_partsval2(partsval2),
          m_partsval3(partsval3)

    {}

    QStringList RevBClientParts::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_partsval1);
        tokens.push_back(m_partsval2);
        tokens.push_back(m_partsval3);
        return tokens;
    }

    RevBClientParts RevBClientParts::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 4)
        {
            swift::misc::CLogMessage(static_cast<RevBClientParts *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }
        return RevBClientParts(tokens[0], tokens[1], tokens[2], tokens[3]);
    }

}
