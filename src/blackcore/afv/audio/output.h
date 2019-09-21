/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_AUDIO_OUTPUT_H
#define BLACKCORE_AFV_AUDIO_OUTPUT_H

#include "blacksound/sampleprovider/sampleprovider.h"

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            //! Stream args
            struct OutputVolumeStreamArgs
            {
                QAudioDeviceInfo DeviceNumber;
                float PeakRaw = 0.0;
                float PeakDB = -1 * std::numeric_limits<float>::infinity();
                float PeakVU = 0.0;
            };

            class CAudioOutputBuffer : public QIODevice
            {
                Q_OBJECT

            public:
                //! Ctor
                CAudioOutputBuffer(BlackSound::SampleProvider::ISampleProvider *sampleProvider, QObject *parent = nullptr);

                BlackSound::SampleProvider::ISampleProvider *m_sampleProvider = nullptr;

                void setAudioFormat(const QAudioFormat &format) { m_outputFormat = format; }

            signals:
                void outputVolumeStream(const OutputVolumeStreamArgs &args);

            protected:
                virtual qint64 readData(char *data, qint64 maxlen) override;
                virtual qint64 writeData(const char *data, qint64 len) override;

            private:
                QAudioFormat m_outputFormat;

                float m_maxSampleOutput = 0;
                int m_sampleCount = 0;
                const int c_sampleCountPerEvent = 4800;
                const float maxDb = 0;
                const float minDb = -40;
            };

            class Output : public QObject
            {
                Q_OBJECT
            public:
                Output(QObject *parent = nullptr);

                void start(const QAudioDeviceInfo &device, BlackSound::SampleProvider::ISampleProvider *sampleProvider);
                void stop();

            signals:
                void outputVolumeStream(const OutputVolumeStreamArgs &args);

            private:
                bool m_started = false;

                QScopedPointer<QAudioOutput> m_audioOutputCom1;
                CAudioOutputBuffer *m_audioOutputBuffer;
            };
        } // ns
    } // ns
} // ns

#endif // guard
