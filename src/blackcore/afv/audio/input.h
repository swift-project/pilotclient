/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_AUDIO_AUDIO_INPUT_H
#define BLACKCORE_AFV_AUDIO_AUDIO_INPUT_H

#include "blacksound/sampleprovider/bufferedwaveprovider.h"
#include "blacksound/codecs/opusencoder.h"

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QString>
#include <QDateTime>
#include <QSharedPointer>

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            //! Input buffer
            class CAudioInputBuffer : public QIODevice
            {
                Q_OBJECT

            public:
                //! Inout buffer
                CAudioInputBuffer(QObject *parent = nullptr);

                void start();
                void stop();

                //! \copydoc QIODevice::readData
                virtual qint64 readData(char *data, qint64 maxlen) override;

                //! \copydoc QIODevice::writeData
                virtual qint64 writeData(const char *data, qint64 len) override;

            signals:
                //! Frame is available
                void frameAvailable(const QByteArray &frame);

            protected:
                void timerEvent(QTimerEvent *event) override;

            private:
                static constexpr qint64 frameSize = 960;
                qint64 m_lastFrameSent = -1;
                QByteArray m_buffer;
                int m_timerId = 0;
            };

            struct OpusDataAvailableArgs
            {
                uint sequenceCounter = 0;
                QByteArray audio;
            };

            struct InputVolumeStreamArgs
            {
                QAudioDeviceInfo DeviceNumber;
                float PeakRaw = 0.0;
                float PeakDB = -1 * std::numeric_limits<float>::infinity();
                float PeakVU = 0.0;
            };

            //! Input
            class CInput : public QObject
            {
                Q_OBJECT

            public:
                //! Ctor
                CInput(int sampleRate, QObject *parent = nullptr);

                bool started() const { return m_started; }

                int opusBytesEncoded() const { return m_opusBytesEncoded; }
                void setOpusBytesEncoded(int opusBytesEncoded) { m_opusBytesEncoded = opusBytesEncoded; }
                double volume() const { return m_volume; }
                void setVolume(double volume) { m_volume = volume; }

                void start(const QAudioDeviceInfo &inputDevice);
                void stop();

            signals:
                void inputVolumeStream(const InputVolumeStreamArgs &args);
                void opusDataAvailable(const OpusDataAvailableArgs &args);

            private:
                void audioInDataAvailable(const QByteArray &frame);

                static constexpr qint64 c_frameSize = 960;
                int m_sampleRate = 0;

                BlackSound::Codecs::COpusEncoder m_encoder;
                QScopedPointer<QAudioInput> m_audioInput;

                bool m_started = false;
                int m_opusBytesEncoded = 0;
                int m_sampleCount = 0;
                double m_volume = 1.0;
                qint16 m_maxSampleInput = 0.0;

                const int c_sampleCountPerEvent = 4800;
                const float maxDb = 0;
                const float minDb = -40;

                uint m_audioSequenceCounter = 0;

                CAudioInputBuffer m_audioInputBuffer;
            };
        } // ns
    } // ns
} // ns

#endif // guard
