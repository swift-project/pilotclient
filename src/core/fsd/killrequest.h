// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_KILLREQUEST_H
#define SWIFT_CORE_FSD_KILLREQUEST_H

#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! Kill request initiated from the server or supervisor.
    //! Client needs to disconnect immediatly upon receiving it.
    class SWIFT_CORE_EXPORT KillRequest : public MessageBase
    {
    public:
        //! Constructor
        KillRequest(const QString &sender, const QString &receiver, const QString &reason);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static KillRequest fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$!!"; }

        QString m_reason; //!< reason for kill request/kicked

    private:
        KillRequest();
    };

    //! Equal to operator
    inline bool operator==(const KillRequest &lhs, const KillRequest &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.m_reason == rhs.m_reason;
    }

    //! Not equal to operator
    inline bool operator!=(const KillRequest &lhs, const KillRequest &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_KILLREQUEST_H
