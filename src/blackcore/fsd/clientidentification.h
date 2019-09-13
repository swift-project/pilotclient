/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_CLIENTIDENTIFICATION_H
#define BLACKCORE_FSD_CLIENTIDENTIFICATION_H

#include "messagebase.h"

namespace BlackCore
{
    namespace Fsd
    {
        //! This packet is sent by any client that supports the VATSIM client authentication protocol,
        //! in response to a $DI query from the server.
        //! Further information to the client authentication protocol is kept restricted.
        class BLACKCORE_EXPORT ClientIdentification : public MessageBase
        {
        public:
            ClientIdentification(const QString &sender, quint16 clientId, const QString &clientName, int clientVersionMajor, int clientVersionMinor,
                                 const QString &userCid, const QString &sysUid, const QString &initialChallenge);

            virtual ~ClientIdentification() {}

            QStringList toTokens() const;
            static ClientIdentification fromTokens(const QStringList &tokens);
            static QString pdu() { return "$ID"; }

            std::uint16_t m_clientId;
            QString m_clientName;
            int m_clientVersionMajor;
            int m_clientVersionMinor;
            QString m_userCid;
            QString m_sysUid;
            QString m_initialChallenge;

        private:
            ClientIdentification();
        };

        inline bool operator==(const ClientIdentification &lhs, const ClientIdentification &rhs)
        {
            return  lhs.sender() == rhs.sender() &&
                    lhs.receiver() == rhs.receiver() &&
                    lhs.m_clientId == rhs.m_clientId &&
                    lhs.m_clientName == rhs.m_clientName &&
                    lhs.m_clientVersionMajor == rhs.m_clientVersionMajor &&
                    lhs.m_clientVersionMinor == rhs.m_clientVersionMinor &&
                    lhs.m_userCid == rhs.m_userCid &&
                    lhs.m_sysUid == rhs.m_sysUid &&
                    lhs.m_initialChallenge == rhs.m_initialChallenge;
        }

        inline bool operator!=(const ClientIdentification &lhs, const ClientIdentification &rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // guard
