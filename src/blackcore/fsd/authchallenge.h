/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_AUTHCHALLENGE_H
#define BLACKCORE_FSD_AUTHCHALLENGE_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT AuthChallenge : public MessageBase
        {
        public:
            AuthChallenge(const QString &sender, const QString &target, const QString &challengeKey);

            virtual ~AuthChallenge() {}

            QStringList toTokens() const;
            static AuthChallenge fromTokens(const QStringList &tokens);
            static QString pdu() { return QStringLiteral("$ZC"); }

            QString m_challengeKey;

        private:
            AuthChallenge();
        };

        inline bool operator==(const AuthChallenge &lhs, const AuthChallenge &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_challengeKey == rhs.m_challengeKey;
        }

        inline bool operator!=(const AuthChallenge &lhs, const AuthChallenge &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
