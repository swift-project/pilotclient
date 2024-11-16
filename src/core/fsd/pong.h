// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_PONG_H
#define SWIFT_CORE_FSD_PONG_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! Sent or received as reply to a ping.
    class SWIFT_CORE_EXPORT Pong : public MessageBase
    {
    public:
        //! Constructor
        Pong(const QString &sender, const QString &receiver, const QString &timestamp);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Pong fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$PO"; }

        QString m_timestamp; //!< timestamp

    private:
        Pong();
    };

    //! Equal to operator
    inline bool operator==(const Pong &lhs, const Pong &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.m_timestamp == rhs.m_timestamp;
    }

    //! Not equal to operator
    inline bool operator!=(const Pong &lhs, const Pong &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // guard
