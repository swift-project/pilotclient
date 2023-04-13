/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_CLIENTQUERY_H
#define BLACKCORE_FSD_CLIENTQUERY_H

#include "blackcore/fsd/messagebase.h"
#include "blackcore/fsd/enums.h"

namespace BlackCore::Fsd
{
    //! This packet is used to query a client’s data.
    //!
    //! Current uses include requests for flight-plans, INF responses, realname details, current server and current frequency.
    //! All requests are sent directly to the client to be queried, currently, except the flight-plan request which is sent to
    //! the server. Therefore, the only client which will return an error is SERVER.
    //! Other clients will simply not reply if the code is unrecognised or request invalid.
    class BLACKCORE_EXPORT ClientQuery : public MessageBase
    {
    public:
        //! Constructor
        ClientQuery(const QString &sender, const QString &clientToBeQueried, ClientQueryType queryType, const QStringList &queryData = {});

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
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_queryType == rhs.m_queryType &&
               lhs.m_queryData == rhs.m_queryData;
    }

    //! Not equal to operator
    inline bool operator!=(const ClientQuery &lhs, const ClientQuery &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
