// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "opusdecoder.h"

namespace swift::sound::codecs
{
    COpusDecoder::COpusDecoder(int sampleRate, int channels) : m_channels(channels)
    {
        int error;
        m_opusDecoder = opus_decoder_create(sampleRate, channels, &error);
    }

    COpusDecoder::~COpusDecoder() { opus_decoder_destroy(m_opusDecoder); }

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
            *decodedLength = opus_decode(m_opusDecoder, reinterpret_cast<const unsigned char *>(opusData.data()),
                                         dataLength, decoded.data(), count, 0);
        }
        decoded.resize(*decodedLength);
        return decoded;
    }

    void COpusDecoder::resetState()
    {
        if (!m_opusDecoder) { return; }
        opus_decoder_ctl(m_opusDecoder, OPUS_RESET_STATE);
    }
} // namespace swift::sound::codecs
