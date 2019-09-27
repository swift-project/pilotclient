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
                double PeakRaw = 0.0;
                double PeakDB = -1 * std::numeric_limits<double>::infinity();
                double PeakVU = 0.0;
            };

            //! Output buffer
            class CAudioOutputBuffer : public QIODevice
            {
                Q_OBJECT

            public:
                //! Ctor
                CAudioOutputBuffer(BlackSound::SampleProvider::ISampleProvider *sampleProvider, QObject *parent = nullptr);

                //! Related provider
                BlackSound::SampleProvider::ISampleProvider *m_sampleProvider = nullptr;

                //! Set the format
                void setAudioFormat(const QAudioFormat &format) { m_outputFormat = format; }

            signals:
                void outputVolumeStream(const OutputVolumeStreamArgs &args);

            protected:
                virtual qint64 readData(char *data, qint64 maxlen) override;
                virtual qint64 writeData(const char *data, qint64 len) override;

            private:
                QAudioFormat m_outputFormat;

                double m_maxSampleOutput = 0;
                int m_sampleCount = 0;
                const int SampleCountPerEvent = 4800;
                const double maxDb =   0;
                const double minDb = -40;
            };

            //! Output
            class Output : public QObject
            {
                Q_OBJECT
            public:
                //! Ctor
                Output(QObject *parent = nullptr);

                //! Dtor
                virtual ~Output() override
                {
                    this->stop();
                }

                //! Start output
                void start(const QAudioDeviceInfo &outputDevice, BlackSound::SampleProvider::ISampleProvider *sampleProvider);

                //! Stop output
                void stop();

                //! Corresponding device
                const QAudioDeviceInfo &device() const { return m_device; }

            signals:
                //! Streaming data
                void outputVolumeStream(const OutputVolumeStreamArgs &args);

            private:
                bool m_started = false;

                QAudioDeviceInfo m_device;
                QScopedPointer<QAudioOutput> m_audioOutputCom1;
                CAudioOutputBuffer *m_audioOutputBuffer = nullptr;
            };
        } // ns
    } // ns
} // ns

#endif // guard
