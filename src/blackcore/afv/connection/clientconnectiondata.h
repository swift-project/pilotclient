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
#include "blackcore/afv/connection/apiserverconnection.h"
#include "blackmisc/logcategories.h"
#include "blackcore/afv/crypto/cryptodtochannel.h"

#include <QDateTime>
#include <QtGlobal>
#include <QString>
#include <QScopedPointer>

namespace BlackCore::Afv::Connection
{
    //! Client connection data
    class CClientConnectionData
    {
    public:
        //! Categories
        static const QStringList &getLogCategories();

        //! Ctor
        CClientConnectionData() = default;

        //! Time since authentication
        qint64 secondsSinceAuthentication() const;

        //! @{
        //!  Servers alive
        bool isVoiceServerAlive() const;
        bool isDataServerAlive() const;
        //! @}

        //! @{
        //! Is connected?
        bool isConnected() const { return m_connected; }
        void setConnected(bool connected) { m_connected = connected; }
        //! @}

        //! @{
        //! Receiving audio?
        bool isReceivingAudio() const { return m_receiveAudio; }
        void setReceiveAudio(bool receive) { m_receiveAudio = receive; }
        //! @}

        //! Crypto channels for voice and data
        void createCryptoChannels();

        //! @{
        //! Tokens
        const PostCallsignResponseDto &getTokens() const { return m_tokens; }
        void setTokens(const PostCallsignResponseDto &dto) { m_tokens = dto; }
        //! @}

        //! @{
        //! Callsign
        const QString &getCallsign() const { return m_callsign; }
        void setCallsign(const QString &callsign) { m_callsign = callsign; }
        //! @}

        //! @{
        //! Uername
        const QString &getUserName() const { return m_userName; }
        void setUserName(const QString &un) { m_userName = un; }
        //! @}

        //! @{
        //! Timestamps
        void setTsAuthenticatedToNow();
        void setTsHeartbeatToNow();
        //! @}

        //! Reset all login related data
        void reset();

        /* TODO
        public long VoiceServerBytesSent     { get; set; }
        public long VoiceServerBytesReceived { get; set; }
        public long DataServerBytesSent      { get; set; }
        public long DataServerBytesReceived  { get; set; }
        */

        QScopedPointer<Crypto::CCryptoDtoChannel> m_voiceCryptoChannel; //!< used crypto channel

    private:
        //! Time since authentication
        qint64 timeSinceAuthenticationSecs() const { return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc()); }

        //! Is the voice server alive?
        bool voiceServerAlive() const;

        QString m_userName; //!< user name
        QString m_callsign; //!< callsign

        PostCallsignResponseDto m_tokens; //!< tokens

        QDateTime m_authenticatedDateTimeUtc;
        QDateTime m_lastVoiceServerHeartbeatAckUtc;
        QDateTime m_lastDataServerHeartbeatAckUtc;

        bool m_receiveAudio = true; //!< audio?
        bool m_connected = false; //!< connected?

        static constexpr qint64 ServerTimeoutSecs = 10; //!< timeout
    };
} // ns

#endif // guard
