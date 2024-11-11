// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_CLIENTIDENTIFICATION_H
#define BLACKCORE_FSD_CLIENTIDENTIFICATION_H

#include "blackcore/fsd/messagebase.h"
#include "config/buildconfig.h"

namespace BlackCore::Fsd
{
    //! This packet is sent by any client that supports the VATSIM client authentication protocol,
    //! in response to a $DI query from the server.
    //! Further information to the client authentication protocol is kept restricted.
    class BLACKCORE_EXPORT ClientIdentification : public MessageBase
    {
    public:
        //! Constructor
        ClientIdentification(const QString &sender, quint16 clientId, const QString &clientName, int clientVersionMajor, int clientVersionMinor,
                             const QString &userCid, const QString &sysUid, const QString &initialChallenge);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static ClientIdentification fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$ID"; }

        //! @{
        //! Properties
        std::uint16_t m_clientId = 0;
        QString m_clientName;
        int m_clientVersionMajor = swift::config::CBuildConfig::getVersion().majorVersion();
        int m_clientVersionMinor = swift::config::CBuildConfig::getVersion().minorVersion();
        QString m_userCid;
        QString m_sysUid;
        QString m_initialChallenge;
        //! @}

    private:
        ClientIdentification();
    };

    //! Equal to operator
    inline bool operator==(const ClientIdentification &lhs, const ClientIdentification &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_clientId == rhs.m_clientId &&
               lhs.m_clientName == rhs.m_clientName &&
               lhs.m_clientVersionMajor == rhs.m_clientVersionMajor &&
               lhs.m_clientVersionMinor == rhs.m_clientVersionMinor &&
               lhs.m_userCid == rhs.m_userCid &&
               lhs.m_sysUid == rhs.m_sysUid &&
               lhs.m_initialChallenge == rhs.m_initialChallenge;
    }

    //! Not equal to operator
    inline bool operator!=(const ClientIdentification &lhs, const ClientIdentification &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
