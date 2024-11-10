// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "opusencoder.h"

namespace swift::sound::codecs
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
