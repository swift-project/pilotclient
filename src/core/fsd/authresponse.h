// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_AUTHRESPONSE_H
#define SWIFT_CORE_FSD_AUTHRESPONSE_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! Responds to an authentication challenge.
    //! The protocol for formulating and responding to auth challenges restricted.
    class SWIFT_CORE_EXPORT AuthResponse : public MessageBase
    {
    public:
        //! Constructor
        AuthResponse(const QString &sender, const QString &receiver, const QString &response);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static AuthResponse fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("$ZR"); }

        QString m_response; //!< response

    private:
        AuthResponse() = default;
    };

    //! Equal to operator
    inline bool operator==(const AuthResponse &lhs, const AuthResponse &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.m_response == rhs.m_response;
    }

    //! Not equal to operator
    inline bool operator!=(const AuthResponse &lhs, const AuthResponse &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_AUTHRESPONSE_H
