/* Copyright (C) 2020
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "revbclientparts.h"
#include "serializer.h"

#include "blackmisc/logmessage.h"

namespace BlackCore
{
    namespace Fsd
    {
        RevBClientParts::RevBClientParts()
        { }

        RevBClientParts::RevBClientParts(const QString &sender, const QString &partsval1, const QString &partsval2, const QString &partsval3)
            : MessageBase(sender),
              m_partsval1(partsval1),
              m_partsval2(partsval2),
              m_partsval3(partsval3)

        { }

        QStringList RevBClientParts::toTokens() const
        {
            QStringList tokens;
            tokens.push_back(m_sender);
            tokens.push_back(m_partsval1);
            tokens.push_back(m_partsval2);
            tokens.push_back(m_partsval3);
            return tokens;
        }


        RevBClientParts RevBClientParts::fromTokens(const QStringList &tokens)
        {
            if (tokens.size() < 4)
            {
                BlackMisc::CLogMessage(static_cast<RevBClientParts *>(nullptr)).debug(u"Wrong number of arguments.");
                return {};
            }
            return RevBClientParts(tokens[0], tokens[1], tokens[2], tokens[3]);
        }

    }

}

