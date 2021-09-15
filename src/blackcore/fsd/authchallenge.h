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

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! FSD Message: auth challenge
    class BLACKCORE_EXPORT AuthChallenge : public MessageBase
    {
    public:
        //! Constructor
        AuthChallenge(const QString &sender, const QString &target, const QString &challengeKey);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static AuthChallenge fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("$ZC"); }

        QString m_challengeKey; //!< key

    private:
        //! Ctor
        AuthChallenge();
    };

    //! Equal to operator
    inline bool operator==(const AuthChallenge &lhs, const AuthChallenge &rhs)
    {
        return  lhs.sender() == rhs.sender() &&
                lhs.receiver() == rhs.receiver() &&
                lhs.m_challengeKey == rhs.m_challengeKey;
    }

    //! Not equal to operator
    inline bool operator!=(const AuthChallenge &lhs, const AuthChallenge &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
