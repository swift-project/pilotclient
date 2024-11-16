// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_CLIENTQUERY_H
#define SWIFT_CORE_FSD_CLIENTQUERY_H

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"

namespace swift::core::fsd
{
    //! This packet is used to query a client’s data.
    //!
    //! Current uses include requests for flight-plans, INF responses, realname details, current server and current
    //! frequency. All requests are sent directly to the client to be queried, currently, except the flight-plan request
    //! which is sent to the server. Therefore, the only client which will return an error is SERVER. Other clients will
    //! simply not reply if the code is unrecognised or request invalid.
    class SWIFT_CORE_EXPORT ClientQuery : public MessageBase
    {
    public:
        //! Constructor
        ClientQuery(const QString &sender, const QString &clientToBeQueried, ClientQueryType queryType,
                    const QStringList &queryData = {});

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static ClientQuery fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$CQ"; }

        //! @{
        //! Properties
        ClientQueryType m_queryType = ClientQueryType::Unknown;
        QStringList m_queryData;
        //! @}

    private:
        ClientQuery();
    };

    //! Equal to operator
    inline bool operator==(const ClientQuery &lhs, const ClientQuery &rhs)
    {
        return lhs.sender() == rhs.sender() && lhs.receiver() == rhs.receiver() && lhs.m_queryType == rhs.m_queryType &&
               lhs.m_queryData == rhs.m_queryData;
    }

    //! Not equal to operator
    inline bool operator!=(const ClientQuery &lhs, const ClientQuery &rhs) { return !(lhs == rhs); }
} // namespace swift::core::fsd

#endif // guard
