// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
        QStringList m_responseData;
        //! @}

    private:
        ClientResponse();
    };

    //! Equal to operator
    inline bool operator==(const ClientResponse &lhs, const ClientResponse &rhs)
    {
        return lhs.sender() == rhs.sender() &&
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
