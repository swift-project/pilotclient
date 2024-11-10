// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/planeinforequest.h"

#include "misc/logmessage.h"

namespace BlackCore::Fsd
{
    PlaneInfoRequest::PlaneInfoRequest() : MessageBase()
    {}

    PlaneInfoRequest::PlaneInfoRequest(const QString &sender, const QString &receiver)
        : MessageBase(sender, receiver)
    {}

    QStringList PlaneInfoRequest::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back("PIR");
        return tokens;
    }

    PlaneInfoRequest PlaneInfoRequest::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            swift::misc::CLogMessage(static_cast<PlaneInfoRequest *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return PlaneInfoRequest(tokens[0], tokens[1]);
    }
}
