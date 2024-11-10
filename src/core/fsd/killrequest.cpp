// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/killrequest.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    KillRequest::KillRequest() : MessageBase()
    {}

    KillRequest::KillRequest(const QString &callsign, const QString &receiver, const QString &reason)
        : MessageBase(callsign, receiver),
          m_reason(reason)
    {}

    QStringList KillRequest::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_reason);
        return tokens;
    }

    KillRequest KillRequest::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 2)
        {
            swift::misc::CLogMessage(static_cast<KillRequest *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return KillRequest(tokens[0], tokens[1], tokens.size() > 2 ? tokens[2] : QString());
    }
}
