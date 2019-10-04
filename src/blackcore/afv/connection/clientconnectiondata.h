/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CONNECTION_CLIENTCONNECTIONDATA_H
#define BLACKCORE_AFV_CONNECTION_CLIENTCONNECTIONDATA_H

#include "blackcore/afv/dto.h"
#include "apiserverconnection.h"
#include "blackcore/afv/crypto/cryptodtochannel.h"

#include <QDateTime>
#include <QtGlobal>
#include <QString>
#include <QScopedPointer>

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            //! Client connection data
            struct CClientConnectionData
            {
                //! Ctor
                CClientConnectionData() = default;

                //! Time since authentication
                qint64 secondsSinceAuthentication() const;

                //!  Servers alive @{
                bool isVoiceServerAlive() const;
                bool isDataServerAlive() const;
                //! @}

                /* TODO
                public long VoiceServerBytesSent { get; set; }
                public long VoiceServerBytesReceived { get; set; }
                public long DataServerBytesSent { get; set; }
                public long DataServerBytesReceived { get; set; }
                */

                //! Crypto channels for voice and data
                void createCryptoChannels();

                qint64 timeSinceAuthentication() const { return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc()); }
                bool voiceServerAlive() const;

                QString m_userName; //!< user name
                QString m_callsign; //!< callsign

                PostCallsignResponseDto m_tokens; //!< tokens
                QScopedPointer<Crypto::CCryptoDtoChannel> voiceCryptoChannel; //!< used crypto channel

                QDateTime m_authenticatedDateTimeUtc;
                QDateTime m_lastVoiceServerHeartbeatAckUtc;

                bool m_receiveAudio = true;  //!< audio?
                bool m_connected    = false; //!< connected?

                static constexpr qint64 ServerTimeoutSecs = 10; //!< timeout
            };
        } // ns
    } // ns
} // ns

#endif // guard
