/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_CODECS_OPUSDECODER_H
#define BLACKSOUND_CODECS_OPUSDECODER_H

#include "blacksound/blacksoundexport.h"
#include "opus/opus.h"

#include <QVector>

namespace BlackSound::Codecs
{
    //! OPUS decoder
    class BLACKSOUND_EXPORT COpusDecoder
    {
    public:
        //! Ctor
        COpusDecoder(int sampleRate, int channels);

        //! Dtor
        ~COpusDecoder();

        //! Not copyable and assignable
        //! @{
        COpusDecoder(const COpusDecoder &decoder) = delete;
        COpusDecoder& operator=(COpusDecoder const&) = delete;
        //! @}

        //! Frame count
        int frameCount(int bufferSize);

        //! Decode
        QVector<qint16> decode(const QByteArray &opusData, int dataLength, int *decodedLength);

        //! Reset
        void resetState();

    private:
        OpusDecoder *m_opusDecoder = nullptr;
        int m_channels;

        static constexpr int MaxDataBytes = 4000;
    };
} // ns

#endif // guard
