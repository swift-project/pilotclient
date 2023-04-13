/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_AUTHRESPONSE_H
#define BLACKCORE_FSD_AUTHRESPONSE_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Responds to an authentication challenge.
    //! The protocol for formulating and responding to auth challenges restricted.
    class BLACKCORE_EXPORT AuthResponse : public MessageBase
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
        AuthResponse();
    };

    //! Equal to operator
    inline bool operator==(const AuthResponse &lhs, const AuthResponse &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_response == rhs.m_response;
    }

    //! Not equal to operator
    inline bool operator!=(const AuthResponse &lhs, const AuthResponse &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
