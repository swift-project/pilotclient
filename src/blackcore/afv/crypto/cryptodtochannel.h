#ifndef CRYPTODTOCHANNEL_H
#define CRYPTODTOCHANNEL_H

#include "blackcore/afv/dto.h"
#include "cryptodtomode.h"

#include <QDateTime>
#include <QByteArray>

#include <limits>

class CryptoDtoChannel
{
public:

    CryptoDtoChannel(QString channelTag, const QByteArray &aeadReceiveKey, const QByteArray &aeadTransmitKey, int receiveSequenceHistorySize = 10);
    CryptoDtoChannel(CryptoDtoChannelConfigDto channelConfig, int receiveSequenceHistorySize = 10);

    QByteArray getTransmitKey(CryptoDtoMode mode);
    QByteArray getTransmitKey(CryptoDtoMode mode, uint &sequenceToSend);
    QString getChannelTag() const;
    QByteArray getReceiveKey(CryptoDtoMode mode);

    bool checkReceivedSequence(uint sequenceReceived);

private:
    bool contains(uint sequence);
    uint getMin(int &minIndex);


    QByteArray m_aeadTransmitKey;
    uint transmitSequence = 0;

    QByteArray m_aeadReceiveKey;

    uint *receiveSequenceHistory;
    int receiveSequenceHistoryDepth;
    int receiveSequenceSizeMaxSize;

    QByteArray hmacKey;

    QString ChannelTag;
    QDateTime LastTransmitUtc;
    QDateTime LastReceiveUtc;
};

#endif // CRYPTODTOCHANNEL_H
