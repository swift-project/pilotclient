/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_ClientResponse_H
#define BLACKCORE_FSD_ClientResponse_H

#include "blackcore/fsd/messagebase.h"
#include "blackcore/fsd/enums.h"

namespace BlackCore::Fsd
{
    //! This packet is used to respond to a client data request.
    class BLACKCORE_EXPORT ClientResponse : public MessageBase
    {
    public:
        //! Constructor
        ClientResponse(const QString &sender, const QString &receiver, ClientQueryType queryType, const QStringList &responseData);

        //! Unknow query?
        bool isUnknownQuery() const { return m_queryType == ClientQueryType::Unknown; }

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static ClientResponse fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$CR"; }

        //! @{
        //! Properties
        ClientQueryType m_queryType {};
        QStringList     m_responseData;
        //! @}

    private:
        ClientResponse();
    };

    //! Equal to operator
    inline bool operator==(const ClientResponse &lhs, const ClientResponse &rhs)
    {
        return  lhs.sender() == rhs.sender() &&
                lhs.receiver() == rhs.receiver() &&
                lhs.m_queryType == rhs.m_queryType &&
                lhs.m_responseData == rhs.m_responseData;
    }

    //! Not equal to operator
    inline bool operator!=(const ClientResponse &lhs, const ClientResponse &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
