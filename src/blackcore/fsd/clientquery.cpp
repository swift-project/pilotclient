/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/clientquery.h"
#include "blackcore/fsd/serializer.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    ClientQuery::ClientQuery() : MessageBase()
    { }

    ClientQuery::ClientQuery(const QString &sender, const QString &clientToBeQueried, ClientQueryType queryType, const QStringList &queryData)
        : MessageBase(sender, clientToBeQueried),
            m_queryType(queryType),
            m_queryData(queryData)
    { }

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
            BlackMisc::CLogMessage(static_cast<ClientQuery *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        }

        QStringList payload;
        if (tokens.size() > 3) { payload = tokens.mid(3); }
        return ClientQuery(tokens[0], tokens[1], fromQString<ClientQueryType>(tokens[2]), payload);
    }
}

