/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "clientconnection.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

#include <QNetworkDatagram>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackCore::Afv::Crypto;

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            CClientConnection::CClientConnection(const QString &apiServer, QObject *parent) :
                QObject(parent),
                m_udpSocket(new QUdpSocket(this)),
                m_voiceServerTimer(new QTimer(this)),
                m_apiServerConnection(new CApiServerConnection(apiServer, this))
            {
                CLogMessage(this).debug(u"ClientConnection instantiated");

                // connect(&m_apiServerConnection, &ApiServerConnection::authenticationFinished, this, &ClientConnection::apiConnectionFinished);
                // connect(&m_apiServerConnection, &ApiServerConnection::addCallsignFinished,    this, &ClientConnection::addCallsignFinished);
                // connect(&m_apiServerConnection, &ApiServerConnection::removeCallsignFinished, this, &ClientConnection::removeCallsignFinished);

                connect(m_voiceServerTimer, &QTimer::timeout, this, &CClientConnection::voiceServerHeartbeat); // sends heartbeat to server
                connect(m_udpSocket, &QUdpSocket::readyRead,  this, &CClientConnection::readPendingDatagrams);
                connect(m_udpSocket, qOverload<QAbstractSocket::SocketError>(&QUdpSocket::error), this, &CClientConnection::handleSocketError);
            }

            void CClientConnection::connectTo(const QString &userName, const QString &password, const QString &callsign, const QString &client, ConnectionCallback callback)
            {
                if (m_connection.isConnected())
                {
                    CLogMessage(this).debug(u"Client already connected to network");
                    return;
                }

                m_connection.setUserName(userName);
                m_connection.setCallsign(callsign);

                QPointer<CClientConnection> myself(this);
                m_apiServerConnection->connectTo(userName, password, client, m_networkVersion,
                {
                    // callback called when connected
                    this, [ = ](bool authenticated)
                    {
                        // callback when connection has been established
                        if (!myself) { return; }

                        if (authenticated)
                        {
                            const QString cs = m_connection.getCallsign();
                            m_connection.setTokens(m_apiServerConnection->addCallsign(cs));
                            m_connection.setTsAuthenticatedToNow();
                            m_connection.createCryptoChannels();
                            m_connection.setTsHeartbeatToNow();
                            this->connectToVoiceServer();
                            // taskServerConnectionCheck.Start();

                            CLogMessage(this).info(u"Connected: '%1' to voice server, socket open: %2") << cs << boolToYesNo(m_udpSocket->isOpen());
                        }
                        else
                        {
                            m_connection.reset();
                        }

                        // Make sure crypto channels etc. are created
                        m_connection.setConnected(authenticated);

                        // callback of the calling parent
                        if (callback) { callback(authenticated); }
                    }
                });
            }

            void CClientConnection::disconnectFrom(const QString &reason)
            {
                if (!m_connection.isConnected())
                {
                    CLogMessage(this).debug(u"Client not connected");
                    return;
                }

                m_connection.setConnected(false);
                // TODO emit disconnected(reason)
                CLogMessage(this).debug(u"Disconnected client: %1") << reason;

                if (!m_connection.getCallsign().isEmpty())
                {
                    m_apiServerConnection->removeCallsign(m_connection.getCallsign());
                }

                // TODO connectionCheckCancelTokenSource.Cancel(); // Stops connection check loop
                disconnectFromVoiceServer();
                m_apiServerConnection->forceDisconnect();
                m_connection.reset();

                CLogMessage(this).debug(u"Disconnection complete");
            }

            void CClientConnection::updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers)
            {
                m_apiServerConnection->updateTransceivers(callsign, transceivers);
            }

            QVector<StationDto> CClientConnection::getAllAliasedStations()
            {
                return m_apiServerConnection->getAllAliasedStations();
            }

            bool CClientConnection::updateVoiceServerUrl(const QString &url)
            {
                if (!m_apiServerConnection) { return false; }
                return m_apiServerConnection->setUrl(url);
            }

            const QString &CClientConnection::getVoiceServerUrl() const
            {
                static const QString e;
                if (!m_apiServerConnection) { return e; }
                return m_apiServerConnection->getUrl();
            }

            void CClientConnection::connectToVoiceServer()
            {
                const QHostAddress localAddress(QHostAddress::AnyIPv4);
                m_udpSocket->bind(localAddress);
                m_voiceServerTimer->start(3000);

                CLogMessage(this).info(u"Connected to voice server '%1'") << m_connection.getTokens().VoiceServer.addressIpV4;
            }

            void CClientConnection::disconnectFromVoiceServer()
            {
                m_voiceServerTimer->stop();
                m_udpSocket->disconnectFromHost();
                CLogMessage(this).info(u"All TaskVoiceServer tasks stopped");
            }

            void CClientConnection::readPendingDatagrams()
            {
                while (m_udpSocket->hasPendingDatagrams())
                {
                    const QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
                    this->processMessage(datagram.data());
                }
            }

            void CClientConnection::processMessage(const QByteArray &messageDdata, bool loopback)
            {
                if (!m_connection.m_voiceCryptoChannel)
                {
                    BLACK_VERIFY_X(false, Q_FUNC_INFO, "processMessage used without crypto channel");
                    return;
                }

                CryptoDtoSerializer::Deserializer deserializer = CryptoDtoSerializer::deserialize(*m_connection.m_voiceCryptoChannel, messageDdata, loopback);

                if (deserializer.m_dtoNameBuffer == AudioRxOnTransceiversDto::getShortDtoName())
                {
                    // qDebug() << "Received audio data";
                    const AudioRxOnTransceiversDto audioOnTransceiverDto = deserializer.getDto<AudioRxOnTransceiversDto>();
                    if (m_connection.isReceivingAudio() && m_connection.isConnected())
                    {
                        emit audioReceived(audioOnTransceiverDto);
                    }
                }
                else if (deserializer.m_dtoNameBuffer == HeartbeatAckDto::getShortDtoName())
                {
                    m_connection.setTsHeartbeatToNow();
                    if (CBuildConfig::isLocalDeveloperDebugBuild()) { CLogMessage(this).debug(u"Received voice server heartbeat"); }
                }
                else
                {
                    CLogMessage(this).warning(u"Received unknown data: %1 %2") << QString(deserializer.m_dtoNameBuffer) << deserializer.m_dataLength;
                }
            }

            void CClientConnection::handleSocketError(QAbstractSocket::SocketError error)
            {
                Q_UNUSED(error)
                CLogMessage(this).debug(u"UDP socket error: '%1'") << m_udpSocket->errorString();
            }

            void CClientConnection::voiceServerHeartbeat()
            {
                if (!m_connection.m_voiceCryptoChannel || !m_udpSocket)
                {
                    BLACK_VERIFY_X(false, Q_FUNC_INFO, "voiceServerHeartbeat used without crypto channel or socket");
                    return;
                }

                const QUrl voiceServerUrl("udp://" + m_connection.getTokens().VoiceServer.addressIpV4);
                if (CBuildConfig::isLocalDeveloperDebugBuild()) { CLogMessage(this).debug(u"Sending voice server heartbeat to '%1'") << voiceServerUrl.host(); }
                HeartbeatDto keepAlive;
                keepAlive.callsign = m_connection.getCallsign().toStdString();
                const QByteArray dataBytes = CryptoDtoSerializer::serialize(*m_connection.m_voiceCryptoChannel, CryptoDtoMode::AEAD_ChaCha20Poly1305, keepAlive);
                m_udpSocket->writeDatagram(dataBytes, QHostAddress(voiceServerUrl.host()), static_cast<quint16>(voiceServerUrl.port()));
            }
        } // ns
    } // ns
} // ns
