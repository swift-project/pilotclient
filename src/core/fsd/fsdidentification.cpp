// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/fsdidentification.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    FSDIdentification::FSDIdentification() : MessageBase() {}

    FSDIdentification::FSDIdentification(const QString &callsign, const QString &receiver, const QString &serverVersion,
                                         const QString &initialChallenge)
        : MessageBase(callsign, receiver), m_serverVersion(serverVersion), m_initialChallenge(initialChallenge)
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
            swift::misc::CLogMessage(static_cast<FSDIdentification *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return FSDIdentification(tokens[0], tokens[1], tokens[2], tokens[3]);
    }
} // namespace swift::core::fsd
