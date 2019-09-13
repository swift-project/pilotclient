/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_MESSAGEBASE_H
#define BLACKCORE_FSD_MESSAGEBASE_H

#include "blackcore/blackcoreexport.h"

#include <QString>
#include <QStringList>

enum class MessageType
{
    Unknown,
    AddAtc,
    AddPilot,
    AtcDataUpdate,
    AuthChallenge,
    AuthResponse,
    ClientIdentification,
    ClientQuery,
    ClientResponse,
    DeleteATC,
    DeletePilot,
    FlightPlan,
    FsdIdentification,
    KillRequest,
    PilotDataUpdate,
    Ping,
    Pong,
    ServerError,
    TextMessage,
    PilotClientCom,
};

namespace BlackCore
{
    namespace Fsd
    {
        class BLACKCORE_EXPORT MessageBase
        {
        public:
            MessageBase() {}
            MessageBase(const QString &sender);
            MessageBase(const QString &sender, const QString &receiver);
            virtual ~MessageBase() {}

            void setCallsign(const QString &sender) { m_sender = sender; }
            QString sender() const { return m_sender; }

            void setReceiver(const QString &receiver) { m_receiver = receiver; }
            QString receiver() const { return m_receiver; }

            bool isValid() const { return m_isValid; }

            void setValid(bool isValid) { m_isValid = isValid; }

        protected:

            // Meta data
            // MessageType messageType = MessageType::Unknown;

            QString m_sender;
            QString m_receiver;

            bool m_isValid = true;
        };
    }
}

#endif // guard
