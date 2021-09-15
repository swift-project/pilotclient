/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_BUFFEREDWAVEPROVIDER_H
#define BLACKSOUND_BUFFEREDWAVEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QAudioFormat>
#include <QByteArray>
#include <QVector>

namespace BlackSound::SampleProvider
{
    //! Buffered wave generator
    class BLACKSOUND_EXPORT CBufferedWaveProvider : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CBufferedWaveProvider(const QAudioFormat &format, QObject *parent = nullptr);

        //! Add samples
        void addSamples(const QVector<float> &samples);

        //! ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Bytes from buffer
        int getBufferedBytes() const { return m_audioBuffer.size(); }

        //! Clear the buffer
        void clearBuffer();

    private:
        QVector<float> m_audioBuffer;
        qint32 m_maxBufferSize;
    };
} // ns

#endif // guard
