/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "opusdecoder.h"

namespace BlackSound
{
    namespace Codecs
    {
        COpusDecoder::COpusDecoder(int sampleRate, int channels) : m_channels(channels)
        {
            int error;
            m_opusDecoder = opus_decoder_create(sampleRate, channels, &error);
        }

        COpusDecoder::~COpusDecoder()
        {
            opus_decoder_destroy(m_opusDecoder);
        }

        int COpusDecoder::frameCount(int bufferSize)
        {
            //  seems like bitrate should be required
            int bitrate = 16;
            int bytesPerSample = (bitrate / 8) * m_channels;
            return bufferSize / bytesPerSample;
        }

        QVector<qint16> COpusDecoder::decode(const QByteArray &opusData, int dataLength, int *decodedLength)
        {
            QVector<qint16> decoded(MaxDataBytes, 0);
            int count = frameCount(MaxDataBytes);

            if (!opusData.isEmpty())
            {
                *decodedLength = opus_decode(m_opusDecoder, reinterpret_cast<const unsigned char *>(opusData.data()), dataLength, decoded.data(), count, 0);
            }
            decoded.resize(*decodedLength);
            return decoded;
        }

        void COpusDecoder::resetState()
        {
            if (!m_opusDecoder) { return; }
            opus_decoder_ctl(m_opusDecoder, OPUS_RESET_STATE);
        }
    } // ns
} // ns
