#include "cryptodtochannel.h"

CryptoDtoChannel::CryptoDtoChannel(QString channelTag, const QByteArray &aeadReceiveKey, const QByteArray &aeadTransmitKey, int receiveSequenceHistorySize)
{
    ChannelTag = channelTag;
    m_aeadReceiveKey = aeadReceiveKey;
    m_aeadTransmitKey = aeadTransmitKey;

    receiveSequenceSizeMaxSize = receiveSequenceHistorySize;
    if (receiveSequenceSizeMaxSize < 1)
        receiveSequenceSizeMaxSize = 1;
    receiveSequenceHistory = new uint[receiveSequenceSizeMaxSize];
    receiveSequenceHistoryDepth = 0;
}

CryptoDtoChannel::CryptoDtoChannel(CryptoDtoChannelConfigDto channelConfig, int receiveSequenceHistorySize)
{
    ChannelTag = channelConfig.channelTag;
    m_aeadReceiveKey = channelConfig.aeadReceiveKey;
    m_aeadTransmitKey = channelConfig.aeadTransmitKey;
    hmacKey = channelConfig.hmacKey;

    receiveSequenceSizeMaxSize = receiveSequenceHistorySize;
    if (receiveSequenceSizeMaxSize < 1)
        receiveSequenceSizeMaxSize = 1;
    receiveSequenceHistory = new uint[receiveSequenceSizeMaxSize];
    receiveSequenceHistoryDepth = 0;
}

QByteArray CryptoDtoChannel::getTransmitKey(CryptoDtoMode mode)
{
    switch (mode)
    {
    case CryptoDtoMode::AEAD_ChaCha20Poly1305: return m_aeadTransmitKey;
    case CryptoDtoMode::Undefined:
    case CryptoDtoMode::None:
        qFatal("GetTransmitKey called with wrong argument.");
    }

    return {};
}

QByteArray CryptoDtoChannel::getTransmitKey(CryptoDtoMode mode, uint &sequenceToSend)
{
    sequenceToSend = transmitSequence;
    transmitSequence++;
    LastTransmitUtc = QDateTime::currentDateTimeUtc();

    switch (mode)
    {
    case CryptoDtoMode::AEAD_ChaCha20Poly1305: return m_aeadTransmitKey;
    case CryptoDtoMode::Undefined:
    case CryptoDtoMode::None:
        qFatal("GetTransmitKey called with wrong argument.");
    }

    return {};
}

QString CryptoDtoChannel::getChannelTag() const
{
    return ChannelTag;
}

QByteArray CryptoDtoChannel::getReceiveKey(CryptoDtoMode mode)
{
    switch (mode)
    {
    case CryptoDtoMode::AEAD_ChaCha20Poly1305: return m_aeadReceiveKey;
    case CryptoDtoMode::Undefined:
    case CryptoDtoMode::None:
        qFatal("getReceiveKey called with wrong argument.");
    }

    return {};
}

bool CryptoDtoChannel::checkReceivedSequence(uint sequenceReceived)
{
    if (contains(sequenceReceived))
    {
        // Duplication or replay attack
        return false;
    }

    if (receiveSequenceHistoryDepth < receiveSequenceSizeMaxSize)                       //If the buffer has been filled...
    {
        receiveSequenceHistory[receiveSequenceHistoryDepth++] = sequenceReceived;
    }
    else
    {
        int minIndex;
        uint minValue = getMin(minIndex);
        if (sequenceReceived < minValue) { return false; }          // Possible replay attack
        receiveSequenceHistory[minIndex] = sequenceReceived;
    }

    LastReceiveUtc = QDateTime::currentDateTimeUtc();
    return true;
}

bool CryptoDtoChannel::contains(uint sequence)
{
    for (int i = 0; i < receiveSequenceHistoryDepth; i++)
    {
        if (receiveSequenceHistory[i] == sequence)
            return true;
    }
    return false;
}

uint CryptoDtoChannel::getMin(int &minIndex)
{
    uint minValue = std::numeric_limits<uint>::max();
    minIndex = -1;
    int index = -1;

    for (int i = 0; i < receiveSequenceHistoryDepth; i++)
    {
        index++;
        if (receiveSequenceHistory[i] <= minValue)
        {
            minValue = receiveSequenceHistory[i];
            minIndex = index;
        }
    }
    return minValue;
}
