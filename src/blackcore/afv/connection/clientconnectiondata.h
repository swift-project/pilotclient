#ifndef CLIENTCONNECTIONDATA_H
#define CLIENTCONNECTIONDATA_H

#include "blackcore/afv/dto.h"
#include "apiserverconnection.h"
#include "blackcore/afv/crypto/cryptodtochannel.h"

#include <QDateTime>
#include <QtGlobal>
#include <QString>
#include <QScopedPointer>

struct ClientConnectionData
{
    ClientConnectionData() = default;

    qint64 secondsSinceAuthentication() const;

    bool isVoiceServerAlive() const;
    bool isDataServerAlive() const;

    /* TODO
    public long VoiceServerBytesSent { get; set; }
    public long VoiceServerBytesReceived { get; set; }
    public long DataServerBytesSent { get; set; }
    public long DataServerBytesReceived { get; set; }
    */

    void createCryptoChannels();

    qint64 timeSinceAuthentication() const { return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc()); }
    bool voiceServerAlive() const;

    QString m_userName;
    QString m_callsign;

    PostCallsignResponseDto m_tokens;

    QScopedPointer<CryptoDtoChannel> voiceCryptoChannel;

    QDateTime m_authenticatedDateTimeUtc;
    QDateTime m_lastVoiceServerHeartbeatAckUtc;

    bool m_receiveAudio = true;
    bool m_connected = false;

    static constexpr qint64 serverTimeout = 10;
};

#endif // CLIENTCONNECTIONDATA_H
