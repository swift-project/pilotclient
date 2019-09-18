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
            class AudioInputBuffer : public QIODevice
            {
                Q_OBJECT

            public:
                AudioInputBuffer() {}

                void start();
                void stop();

                qint64 readData(char *data, qint64 maxlen) override;
                qint64 writeData(const char *data, qint64 len) override;

            signals:
                void frameAvailable(const QByteArray &frame);

            private:
                static constexpr qint64 frameSize = 960;
                QByteArray m_buffer;
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

            class Input : public QObject
            {
                Q_OBJECT

            public:
                Input(int sampleRate, QObject *parent = nullptr);

                bool started() const;

                int opusBytesEncoded() const;
                void setOpusBytesEncoded(int opusBytesEncoded);

                float volume() const;
                void setVolume(float volume);

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
                float m_volume = 1.0f;
                int m_sampleCount = 0;
                float m_maxSampleInput = 0;

                const int c_sampleCountPerEvent = 4800;
                const float maxDb = 0;
                const float minDb = -40;

                uint m_audioSequenceCounter = 0;

                AudioInputBuffer m_audioInputBuffer;
            };
        } // ns
    } // ns
} // ns

#endif // guard
