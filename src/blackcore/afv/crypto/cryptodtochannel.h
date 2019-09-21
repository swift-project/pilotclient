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
#include "cryptodtomode.h"

#include <QDateTime>
#include <QByteArray>

#include <limits>

namespace BlackCore
{
    namespace Afv
    {
        namespace Crypto
        {
            //! Crypto channel
            class CCryptoDtoChannel
            {
            public:
                //! Ctor
                CCryptoDtoChannel(QString channelTag, const QByteArray &aeadReceiveKey, const QByteArray &aeadTransmitKey, int receiveSequenceHistorySize = 10);

                //! Ctor
                CCryptoDtoChannel(CryptoDtoChannelConfigDto channelConfig, int receiveSequenceHistorySize = 10);

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
        } // ns
    } // ns
} // ns

#endif // guard
