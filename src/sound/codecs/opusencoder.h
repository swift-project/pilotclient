// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_CODECS_OPUSENCODER_H
#define SWIFT_SOUND_CODECS_OPUSENCODER_H

#include <QByteArray>
#include <QVector>

#include "opus/opus.h"

#include "sound/swiftsoundexport.h"

namespace swift::sound::codecs
{
    //! OPUS encoder
    class SWIFT_SOUND_EXPORT COpusEncoder
    {
    public:
        //! Ctor
        COpusEncoder(int sampleRate, int channels, int application = OPUS_APPLICATION_VOIP);

        //! Dtor
        ~COpusEncoder();

        //! Non copyable
        COpusEncoder(const COpusEncoder &temp_obj) = delete;

        //! Non assignable
        COpusEncoder &operator=(const COpusEncoder &temp_obj) = delete;

        //! Bit rate
        void setBitRate(int bitRate);

        //! Encode
        QByteArray encode(const QVector<qint16> &pcmSamples, int samplesLength, int *encodedLength);

    private:
        OpusEncoder *opusEncoder = nullptr;

        static constexpr int maxDataBytes = 4000;
    };
} // namespace swift::sound::codecs

#endif // SWIFT_SOUND_CODECS_OPUSENCODER_H
