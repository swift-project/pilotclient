/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_CODECS_OPUSENCODER_H
#define BLACKSOUND_CODECS_OPUSENCODER_H

#include "opus/opus.h"
#include "blacksound/blacksoundexport.h"

#include <QByteArray>
#include <QVector>

namespace BlackSound::Codecs
{
    //! OPUS encoder
    class BLACKSOUND_EXPORT COpusEncoder
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
} // ns

#endif // guard
