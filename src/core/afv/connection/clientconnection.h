// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AFV_CONNECTION_CLIENTCONNECTION_H
#define SWIFT_CORE_AFV_CONNECTION_CLIENTCONNECTION_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QUdpSocket>
#include <QUuid>

#include "core/afv/connection/apiserverconnection.h"
#include "core/afv/connection/clientconnectiondata.h"
#include "core/afv/crypto/cryptodtoserializer.h"
#include "core/afv/dto.h"
#include "misc/verify.h"

namespace swift::core::afv::connection
{
    //! Client connection
    class CClientConnection : public QObject
    {
        Q_OBJECT

    public:
        //! Connection status
        enum ConnectionStatus
        {
            Disconnected, //!< Not connected
            Connected, //!< Connection established
        };
        Q_ENUM(ConnectionStatus)

        //! Ctor
        CClientConnection(const QString &apiServer, QObject *parent = nullptr);

        //! Connect
        //! \remark ASYNC, calling callback when done
        void connectTo(const QString &userName, const QString &password, const QString &callsign, const QString &client,
                       ConnectionCallback callback);

        //! Disconnect
        void disconnectFrom(const QString &reason = {});

        //! Is connected?
        bool isConnected() const { return m_connection.isConnected(); }

        //! Is alive?
        bool isVoiceServerAlive() const { return m_connection.isVoiceServerAlive(); }

        //! @{
        //! Receiving audio?
        void setReceiveAudio(bool value) { m_connection.setReceiveAudio(value); }
        bool receiveAudio() const { return m_connection.isReceivingAudio(); }
        bool receiveAudioDto() const { return m_receiveAudioDto; }
        void setReceiveAudioDto(bool receiveAudioDto) { m_receiveAudioDto = receiveAudioDto; }
        //! @}

        //! Send voice DTO to server
        template <typename T>
        void sendToVoiceServer(const T &dto)
        {
            if (!m_connection.m_voiceCryptoChannel || !m_udpSocket)
            {
                SWIFT_VERIFY_X(false, Q_FUNC_INFO, "sendVoice used without crypto channel or socket");
                return;
            }
            const QUrl voiceServerUrl("udp://" + m_connection.getTokens().VoiceServer.addressIpV4);
            const QByteArray dataBytes = crypto::CryptoDtoSerializer::serialize(
                *m_connection.m_voiceCryptoChannel, crypto::CryptoDtoMode::AEAD_ChaCha20Poly1305, dto);
            m_udpSocket->writeDatagram(dataBytes, QHostAddress(voiceServerUrl.host()),
                                       static_cast<quint16>(voiceServerUrl.port()));
        }

        //! Update transceivers
        void updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers);

        //! All aliased stations
        QVector<StationDto> getAllAliasedStations();

        //! Update the voice server URL
        bool updateVoiceServerUrl(const QString &url);

        //! Get the voice server URL
        const QString &getVoiceServerUrl() const;

        //! Authenticated since when
        qint64 secondsSinceAuthentication() const { return m_connection.secondsSinceAuthentication(); }

        //! @{
        //! User data
        const QString &getUserName() const { return m_connection.getUserName(); }
        const QString &getCallsign() const { return m_connection.getCallsign(); }
        const QString &getPassword() const
        {
            static const QString e;
            return m_apiServerConnection ? m_apiServerConnection->getPassword() : e;
        }
        const QString &getClient() const
        {
            static const QString e;
            return m_apiServerConnection ? m_apiServerConnection->getClient() : e;
        }
        const QUuid &getNetworkVersion() const { return m_networkVersion; }
        //! @}

    signals:
        //! Audio has been received
        void audioReceived(const AudioRxOnTransceiversDto &dto);

    private:
        void connectToVoiceServer();
        void disconnectFromVoiceServer();

        void readPendingDatagrams();
        void processMessage(const QByteArray &messageDdata, bool loopback = false);
        void handleSocketError(QAbstractSocket::SocketError error);

        void voiceServerHeartbeat();

        const QUuid m_networkVersion = QUuid("3a5ddc6d-cf5d-4319-bd0e-d184f772db80");

        // Data
        CClientConnectionData m_connection;

        // Voice server
        QUdpSocket *m_udpSocket = nullptr;
        QTimer *m_voiceServerTimer = nullptr;

        // API server
        CApiServerConnection *m_apiServerConnection = nullptr;

        // Properties
        bool m_receiveAudioDto = true;
    };
} // namespace swift::core::afv::connection

#endif // SWIFT_CORE_AFV_CONNECTION_CLIENTCONNECTION_H
