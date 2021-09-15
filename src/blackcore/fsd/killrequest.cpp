/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/killrequest.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    KillRequest::KillRequest() : MessageBase()
    { }

    KillRequest::KillRequest(const QString &callsign, const QString &receiver, const QString &reason)
        : MessageBase(callsign, receiver),
            m_reason(reason)
    { }

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
            BlackMisc::CLogMessage(static_cast<KillRequest *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return KillRequest(tokens[0], tokens[1], tokens.size() > 2 ? tokens[2] : QString());
    }
}
