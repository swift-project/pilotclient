/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/pong.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    Pong::Pong() : MessageBase()
    {}

    Pong::Pong(const QString &sender, const QString &receiver, const QString &timestamp)
        : MessageBase(sender, receiver),
          m_timestamp(timestamp)
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
            BlackMisc::CLogMessage(static_cast<Pong *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return Pong(tokens[0], tokens[1], tokens[2]);
    }
}
