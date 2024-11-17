// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/pong.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    Pong::Pong() : MessageBase() {}

    Pong::Pong(const QString &sender, const QString &receiver, const QString &timestamp)
        : MessageBase(sender, receiver), m_timestamp(timestamp)
    {}

    QStringList Pong::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_timestamp);
        return tokens;
    }

    Pong Pong::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            swift::misc::CLogMessage(static_cast<Pong *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return Pong(tokens[0], tokens[1], tokens[2]);
    }
} // namespace swift::core::fsd
