// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AFV_CRYPTO_CRYPTODTOCHANNEL_H
#define SWIFT_CORE_AFV_CRYPTO_CRYPTODTOCHANNEL_H

#include "core/afv/dto.h"
#include "core/afv/crypto/cryptodtomode.h"

#include <QDateTime>
#include <QByteArray>
#include <QVector>

#include <limits>

namespace swift::core::afv::crypto
{
    //! Crypto channel
    class CCryptoDtoChannel
    {
    public:
        //! Ctor
        CCryptoDtoChannel(const CryptoDtoChannelConfigDto &channelConfig, int receiveSequenceHistorySize = 10);

        //! @{
        //! Transmit key
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
        QString m_channelTag;
        QDateTime m_LastTransmitUtc;
        QDateTime m_lastReceiveUtc;
    };
} // ns

#endif // guard
