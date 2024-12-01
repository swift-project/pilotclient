// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_BUFFEREDWAVEPROVIDER_H
#define SWIFT_SOUND_BUFFEREDWAVEPROVIDER_H

#include <QAudioFormat>
#include <QByteArray>
#include <QVector>

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Buffered wave generator
    class SWIFT_SOUND_EXPORT CBufferedWaveProvider : public ISampleProvider
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
} // namespace swift::sound::sample_provider

#endif // SWIFT_SOUND_BUFFEREDWAVEPROVIDER_H
