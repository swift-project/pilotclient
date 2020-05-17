/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "clientresponse.h"
#include "serializer.h"

#include "blackmisc/logmessage.h"

namespace BlackCore
{
    namespace Fsd
    {
        ClientResponse::ClientResponse() : MessageBase()
        { }

        ClientResponse::ClientResponse(const QString &sender, const QString &receiver, ClientQueryType queryType, const QStringList &responseData)
            : MessageBase(sender, receiver),
              m_queryType(queryType),
              m_responseData(responseData)
        { }

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
                BlackMisc::CLogMessage(static_cast<ClientResponse *>(nullptr)).warning(u"Wrong number of arguments.");
                return {};
            }

            QStringList responseData;
            if (tokens.size() > 3) { responseData = tokens.mid(3); }
            return ClientResponse(tokens[0], tokens[1], fromQString<ClientQueryType>(tokens[2]), responseData);
        }
    }
}
