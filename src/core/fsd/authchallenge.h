// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_AUTHCHALLENGE_H
#define SWIFT_CORE_FSD_AUTHCHALLENGE_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! FSD Message: auth challenge
    class SWIFT_CORE_EXPORT AuthChallenge : public MessageBase
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
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() &&
               lhs.m_challengeKey == rhs.m_challengeKey;
    }

    //! Not equal to operator
    inline bool operator!=(const AuthChallenge &lhs, const AuthChallenge &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // guard
