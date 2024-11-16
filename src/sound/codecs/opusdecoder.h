// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_CODECS_OPUSDECODER_H
#define SWIFT_SOUND_CODECS_OPUSDECODER_H

#include <QVector>

#include "opus/opus.h"

#include "sound/swiftsoundexport.h"

namespace swift::sound::codecs
{
    //! OPUS decoder
    class SWIFT_SOUND_EXPORT COpusDecoder
    {
    public:
        //! Ctor
        COpusDecoder(int sampleRate, int channels);

        //! Dtor
        ~COpusDecoder();

        //! @{
        //! Not copyable and assignable
        COpusDecoder(const COpusDecoder &decoder) = delete;
        COpusDecoder &operator=(COpusDecoder const &) = delete;
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
} // namespace swift::sound::codecs

#endif // guard
