/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CRYPTO_CRYPTODTOCHANNEL_H
#define BLACKCORE_AFV_CRYPTO_CRYPTODTOCHANNEL_H

#include "blackcore/afv/dto.h"
#include "blackcore/afv/crypto/cryptodtomode.h"

#include <QDateTime>
#include <QByteArray>
#include <QVector>

#include <limits>

namespace BlackCore::Afv::Crypto
{
    //! Crypto channel
    class CCryptoDtoChannel
    {
    public:
        //! Ctor
        CCryptoDtoChannel(const QString &channelTag, const QByteArray &aeadReceiveKey, const QByteArray &aeadTransmitKey, int receiveSequenceHistorySize = 10);

        //! Ctor
        CCryptoDtoChannel(const CryptoDtoChannelConfigDto &channelConfig, int receiveSequenceHistorySize = 10);

        //! Transmit key
        //! @{
        QByteArray getTransmitKey(CryptoDtoMode mode);
        QByteArray getTransmitKey(CryptoDtoMode mode, uint &sequenceToSend);
        //! @}

        //! Channel tag
        QString getChannelTag() const;

        //! Receiver key
        QByteArray getReceiveKey(CryptoDtoMode mode);

        //! check the received sequence
        bool checkReceivedSequence(uint sequenceReceived);

    private:
        bool contains(uint sequence) const;
        uint getMin(int &minIndex) const;

        QByteArray m_aeadTransmitKey;
        QByteArray m_aeadReceiveKey;

        uint m_transmitSequence = 0;
        QVector<uint> m_receiveSequenceHistory;
        int m_receiveSequenceHistoryDepth;
        int m_receiveSequenceSizeMaxSize;

        QByteArray m_hmacKey;
        QString    m_channelTag;
        QDateTime  m_LastTransmitUtc;
        QDateTime  m_lastReceiveUtc;
    };
} // ns

#endif // guard
