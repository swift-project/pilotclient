#include "clientconnectiondata.h"
#include <QDebug>

qint64 ClientConnectionData::secondsSinceAuthentication() const
{
    return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc());
}

bool ClientConnectionData::isVoiceServerAlive() const
{
    return m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) > serverTimeout;
}

void ClientConnectionData::createCryptoChannels()
{
    if (! m_tokens.isValid)
    {
        qWarning() << "Tokens not set";
    }
    voiceCryptoChannel.reset(new CryptoDtoChannel(m_tokens.VoiceServer.channelConfig));
    // dataCryptoChannel.reset(new CryptoDtoChannel(m_tokens.DataServer.channelConfig));
}

bool ClientConnectionData::voiceServerAlive() const
{
    return timeSinceAuthentication() < serverTimeout ||
            m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) < serverTimeout;
}
