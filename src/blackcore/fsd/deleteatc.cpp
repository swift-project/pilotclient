// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/deleteatc.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    DeleteAtc::DeleteAtc() : MessageBase()
    {}

    DeleteAtc::DeleteAtc(const QString &sender, const QString &cid)
        : MessageBase(sender),
          m_cid(cid)
    {}

    QStringList DeleteAtc::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(m_cid);
        return tokens;
    }

    DeleteAtc DeleteAtc::fromTokens(const QStringList &tokens)
    {
        if (tokens.isEmpty())
        {
            BlackMisc::CLogMessage(static_cast<DeleteAtc *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return DeleteAtc(tokens[0], (tokens.size() >= 2) ? tokens[1] : QString());
    }
}
