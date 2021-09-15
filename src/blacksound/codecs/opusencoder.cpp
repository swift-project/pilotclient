/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "opusencoder.h"

namespace BlackSound::Codecs
{
    COpusEncoder::COpusEncoder(int sampleRate, int channels, int application)
    {
        int error;
        opusEncoder = opus_encoder_create(sampleRate, channels, application, &error);
    }

    COpusEncoder::~COpusEncoder()
    {
        opus_encoder_destroy(opusEncoder);
    }

    void COpusEncoder::setBitRate(int bitRate)
    {
        opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(bitRate));
    }

    QByteArray COpusEncoder::encode(const QVector<qint16> &pcmSamples, int samplesLength, int *encodedLength)
    {
        QByteArray encoded(maxDataBytes, 0);
        int length = opus_encode(opusEncoder, reinterpret_cast<const opus_int16 *>(pcmSamples.data()), samplesLength, reinterpret_cast<unsigned char *>(encoded.data()), maxDataBytes);
        *encodedLength = length;
        encoded.truncate(length);
        return encoded;
    }
} // ns
