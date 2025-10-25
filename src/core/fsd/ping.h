// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_PING_H
#define SWIFT_CORE_FSD_PING_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! Ping. Needs to be answered with a pong.
    class SWIFT_CORE_EXPORT Ping : public MessageBase
    {
    public:
        //! Constructor
        Ping(const QString &sender, const QString &receiver, const QString &timestamp);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Ping fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$PI"; }

        QString m_timestamp; //!< timestamp

    private:
        Ping() = default;
    };

    //! Equal to operator
    inline bool operator==(const Ping &lhs, const Ping &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.m_timestamp == rhs.m_timestamp;
    }

    //! Not equal to operator
    inline bool operator!=(const Ping &lhs, const Ping &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_PING_H
