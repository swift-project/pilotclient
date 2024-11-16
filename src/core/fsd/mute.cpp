// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/mute.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    Mute::Mute(const QString &sender, const QString &receiver, bool mute)
        : MessageBase(sender, receiver),
          m_mute(mute)
    {}

    QStringList Mute::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(QString::number(m_mute ? 1 : 0));
        return tokens;
    }

    Mute Mute::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            swift::misc::CLogMessage(static_cast<Mute *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return Mute(tokens[0], tokens[1], tokens[2] == QStringLiteral("1"));
    }
} // namespace swift::core::fsd
