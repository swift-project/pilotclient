/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/fsdidentification.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    FSDIdentification::FSDIdentification() : MessageBase()
    {}

    FSDIdentification::FSDIdentification(const QString &callsign, const QString &receiver, const QString &serverVersion, const QString &initialChallenge)
        : MessageBase(callsign, receiver),
          m_serverVersion(serverVersion),
          m_initialChallenge(initialChallenge)
    {}

    QStringList FSDIdentification::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_serverVersion);
        tokens.push_back(m_initialChallenge);
        return tokens;
    }

    FSDIdentification FSDIdentification::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 4)
        {
            BlackMisc::CLogMessage(static_cast<FSDIdentification *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return FSDIdentification(tokens[0], tokens[1], tokens[2], tokens[3]);
    }
}
