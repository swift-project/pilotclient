/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "planeinforequest.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    PlaneInfoRequest::PlaneInfoRequest() : MessageBase()
    { }

    PlaneInfoRequest::PlaneInfoRequest(const QString &sender, const QString &receiver)
        : MessageBase(sender, receiver)
    { }

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
            BlackMisc::CLogMessage(static_cast<PlaneInfoRequest *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return PlaneInfoRequest(tokens[0], tokens[1]);
    }
}
