// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/clientresponse.h"

#include "core/fsd/serializer.h"
#include "misc/logmessage.h"

namespace swift::core::fsd
{
    ClientResponse::ClientResponse(const QString &sender, const QString &receiver, ClientQueryType queryType,
                                   const QStringList &responseData)
        : MessageBase(sender, receiver), m_queryType(queryType), m_responseData(responseData)
    {}

    QStringList ClientResponse::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(toQString(m_queryType));
        tokens.append(m_responseData);
        return tokens;
    }

    ClientResponse ClientResponse::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            swift::misc::CLogMessage(static_cast<ClientResponse *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }

        QStringList responseData;
        if (tokens.size() > 3) { responseData = tokens.mid(3); }
        return { tokens[0], tokens[1], fromQString<ClientQueryType>(tokens[2]), responseData };
    }
} // namespace swift::core::fsd
