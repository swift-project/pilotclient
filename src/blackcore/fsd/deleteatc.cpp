/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
