/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "authchallenge.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    AuthChallenge::AuthChallenge() : MessageBase ()
    { }

    AuthChallenge::AuthChallenge(const QString &sender, const QString &target, const QString &challengeKey) :
        MessageBase(sender, target),
        m_challengeKey(challengeKey)
    { }

    QStringList AuthChallenge::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_challengeKey);
        return tokens;
    }

    AuthChallenge AuthChallenge::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            BlackMisc::CLogMessage(static_cast<AuthChallenge *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }
        return AuthChallenge(tokens[0], tokens[1], tokens[2]);
    }
}



