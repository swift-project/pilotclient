// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/clientquery.h"

#include "core/fsd/serializer.h"
#include "misc/logmessage.h"

namespace swift::core::fsd
{
    ClientQuery::ClientQuery() : MessageBase() {}

    ClientQuery::ClientQuery(const QString &sender, const QString &clientToBeQueried, ClientQueryType queryType,
                             const QStringList &queryData)
        : MessageBase(sender, clientToBeQueried), m_queryType(queryType), m_queryData(queryData)
    {}

    QStringList ClientQuery::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(toQString(m_queryType));
        tokens.append(m_queryData);
        return tokens;
    }

    ClientQuery ClientQuery::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            swift::misc::CLogMessage(static_cast<ClientQuery *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        QStringList payload;
        if (tokens.size() > 3) { payload = tokens.mid(3); }
        return { tokens[0], tokens[1], fromQString<ClientQueryType>(tokens[2]), payload };
    }
} // namespace swift::core::fsd
