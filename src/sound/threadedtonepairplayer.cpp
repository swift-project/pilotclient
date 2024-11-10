// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "threadedtonepairplayer.h"
#include "misc/logmessage.h"
#include "sound/audioutilities.h"

#include <QTimer>

using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::sound;

namespace swift::sound
{
    CThreadedTonePairPlayer::CThreadedTonePairPlayer(QObject *owner, const QString &name, const CAudioDeviceInfo &device)
        : CContinuousWorker(owner, name),
          m_deviceInfo(device)
    {}

    void CThreadedTonePairPlayer::play(int volume, const QList<CTonePair> &tonePairs)
    {
        QPointer<CThreadedTonePairPlayer> myself(this);
        QMutexLocker ml(&m_mutex);
        if (m_audioOutput->state() != QAudio::StoppedState) { return; }

        m_bufferData = this->getAudioByTonePairs(tonePairs);
        m_audioOutput->setVolume(static_cast<qreal>(0.01 * volume));
        QTimer::singleShot(0, this, [=] {
            if (myself) { myself->playBuffer(); }
        });
    }

    bool CThreadedTonePairPlayer::reinitializeAudio(const CAudioDeviceInfo &device)
    {
        if (this->getAudioDevice() == device) { return false; }
        {
            QMutexLocker ml(&m_mutex);
            m_deviceInfo = device;
        }
        this->initialize();
        return true;
    }

    CAudioDeviceInfo CThreadedTonePairPlayer::getAudioDevice() const
    {
        QMutexLocker ml(&m_mutex);
        return m_deviceInfo;
    }

    void CThreadedTonePairPlayer::initialize()
    {
        QMutexLocker ml(&m_mutex);
        CLogMessage(this).info(u"CThreadedTonePairPlayer for device '%1'") << m_deviceInfo.getName();

        QAudioFormat format;
        format.setSampleRate(44100);
        format.setChannelCount(1);
        format.setSampleFormat(QAudioFormat::Int16);
        static_assert(Q_BYTE_ORDER == Q_LITTLE_ENDIAN);

        // find best device
        const QAudioDevice selectedDevice = getHighestCompatibleOutputDevice(m_deviceInfo, format);
        m_audioFormat = format;
        m_audioOutput = new QAudioSink(selectedDevice, m_audioFormat, this);
        connect(m_audioOutput, &QAudioSink::stateChanged, this, &CThreadedTonePairPlayer::handleStateChanged);
    }

    void CThreadedTonePairPlayer::beforeQuit() noexcept
    {
        QMutexLocker ml(&m_mutex);
        CLogMessage(this).info(u"CThreadedTonePairPlayer quit for '%1'") << m_deviceInfo.getName();

        if (m_audioOutput)
        {
            m_audioOutput->stop();
            m_audioOutput->disconnect();
        }

        m_buffer.close();
    }

    void CThreadedTonePairPlayer::handleStateChanged(QAudio::State newState)
    {
        QMutexLocker ml(&m_mutex);
        switch (newState)
        {
        case QAudio::IdleState: m_audioOutput->stop(); break;
        default: break;
        }
    }

    void CThreadedTonePairPlayer::playBuffer()
    {
        QMutexLocker ml(&m_mutex);
        if (!m_audioOutput || m_audioOutput->state() == QAudio::ActiveState) { return; }
        m_buffer.close();
        m_buffer.setBuffer(&m_bufferData);
        m_buffer.open(QIODevice::ReadOnly);
        m_audioOutput->start(&m_buffer);
    }

    QByteArray CThreadedTonePairPlayer::getAudioByTonePairs(const QList<CTonePair> &tonePairs)
    {
        Q_ASSERT(!tonePairs.isEmpty());
        QByteArray finalBufferData;

        for (const auto &tonePair : std::as_const(tonePairs))
        {
            if (m_tonePairCache.contains(tonePair))
            {
                QByteArray bufferData;
                bufferData = m_tonePairCache.value(tonePair);
                finalBufferData.append(bufferData);
            }
            else
            {
                QByteArray bufferData;
                bufferData = generateAudioFromTonePairs(tonePair);
                m_tonePairCache.insert(tonePair, bufferData);
                finalBufferData.append(bufferData);
            }
        }
        return finalBufferData;
    }

    QByteArray CThreadedTonePairPlayer::generateAudioFromTonePairs(const CTonePair &tonePair)
    {
        const int bytesPerSample = m_audioFormat.bytesPerSample();
        const int bytesForAllChannels = m_audioFormat.bytesPerFrame();

        QByteArray bufferData;
        qint64 bytesPerTonePair = m_audioFormat.sampleRate() * bytesForAllChannels * tonePair.getDurationMs() / 1000;
        bufferData.resize(static_cast<int>(bytesPerTonePair));
        unsigned char *bufferPointer = reinterpret_cast<unsigned char *>(bufferData.data());

        qint64 last0AmplitudeSample = bytesPerTonePair; // last sample when amplitude was 0
        int sampleIndexPerTonePair = 0;
        while (bytesPerTonePair)
        {
            // http://hyperphysics.phy-astr.gsu.edu/hbase/audio/sumdif.html
            // http://math.stackexchange.com/questions/164369/how-do-you-calculate-the-frequency-perceived-by-humans-of-two-sinusoidal-waves-a
            const double pseudoTime = static_cast<double>(sampleIndexPerTonePair % this->m_audioFormat.sampleRate()) / this->m_audioFormat.sampleRate();
            double amplitude = 0.0; // amplitude -1 -> +1 , 0 is silence
            if (tonePair.getFirstFrequencyHz() > 10)
            {
                // the combination of two frequencies actually would have 2*amplitude,
                // but I have to normalize with amplitude -1 -> +1
                amplitude = tonePair.getSecondFrequencyHz() == 0 ?
                                qSin(2 * M_PI * tonePair.getFirstFrequencyHz() * pseudoTime) :
                                qSin(M_PI * (tonePair.getFirstFrequencyHz() + tonePair.getSecondFrequencyHz()) * pseudoTime) *
                                    qCos(M_PI * (tonePair.getFirstFrequencyHz() - tonePair.getSecondFrequencyHz()) * pseudoTime);
            }

            // avoid overflow
            Q_ASSERT(amplitude <= 1.0 && amplitude >= -1.0);
            if (amplitude < -1.0) { amplitude = -1.0; }
            else if (amplitude > 1.0) { amplitude = 1.0; }
            else if (qAbs(amplitude) < 1.0 / 65535)
            {
                amplitude = 0;
                last0AmplitudeSample = bytesPerTonePair;
            }

            // generate this for all channels, usually 1 channel
            for (int i = 0; i < this->m_audioFormat.channelCount(); ++i)
            {
                this->writeAmplitudeToBuffer(amplitude, bufferPointer);
                bufferPointer += bytesPerSample;
                bytesPerTonePair -= bytesPerSample;
            }
            ++sampleIndexPerTonePair;
        }

        // fixes the range from the last 0 pass through
        if (last0AmplitudeSample > 0)
        {
            bufferPointer -= last0AmplitudeSample;
            while (last0AmplitudeSample)
            {
                const double amplitude = 0.0; // amplitude -1 -> +1 , 0 is silence

                // generate this for all channels, usually 1 channel
                for (int i = 0; i < this->m_audioFormat.channelCount(); ++i)
                {
                    this->writeAmplitudeToBuffer(amplitude, bufferPointer);
                    bufferPointer += bytesPerSample;
                    last0AmplitudeSample -= bytesPerSample;
                }
            }
        }
        return bufferData;
    }

    void CThreadedTonePairPlayer::writeAmplitudeToBuffer(double amplitude, unsigned char *bufferPointer)
    {
        Q_ASSERT(this->m_audioFormat.sampleFormat() == QAudioFormat::Int16);
        static_assert(Q_BYTE_ORDER == Q_LITTLE_ENDIAN);
        const qint16 value = static_cast<qint16>(amplitude * 32767);

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        qToBigEndian<qint16>(value, bufferPointer);
#else
        qToLittleEndian<qint16>(value, bufferPointer);
#endif
    }
}
