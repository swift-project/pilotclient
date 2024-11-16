// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/authchallenge.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    AuthChallenge::AuthChallenge() : MessageBase() {}

    AuthChallenge::AuthChallenge(const QString &sender, const QString &target, const QString &challengeKey)
        : MessageBase(sender, target), m_challengeKey(challengeKey)
    {}

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
            swift::misc::CLogMessage(static_cast<AuthChallenge *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }
        return AuthChallenge(tokens[0], tokens[1], tokens[2]);
    }
} // namespace swift::core::fsd
