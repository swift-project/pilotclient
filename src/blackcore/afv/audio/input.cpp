// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackcore/afv/audio/input.h"
#include "sound/audioutilities.h"
#include "misc/logmessage.h"
#include "misc/verify.h"

#include <QtGlobal>
#include <QStringBuilder>
#include <QDebug>
#include <QAudioDevice>
#include <cmath>

using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::sound;

namespace BlackCore::Afv::Audio
{
    CAudioInputBuffer::CAudioInputBuffer(QObject *parent) : QIODevice(parent)
    {
        this->setObjectName("CAudioInputBuffer");
    }

    void CAudioInputBuffer::start(const QAudioFormat &format)
    {
        m_format = format;
        m_buffer.clear();
        if (!this->isOpen())
        {
            open(QIODevice::WriteOnly | QIODevice::Unbuffered);
        }
    }

    void CAudioInputBuffer::stop()
    {
        this->close();
    }

    qint64 CAudioInputBuffer::readData(char *data, qint64 maxlen)
    {
        Q_UNUSED(data)
        Q_UNUSED(maxlen)
        return 0;
    }

    qint64 CAudioInputBuffer::writeData(const char *data, qint64 len)
    {
        m_buffer.append(data, static_cast<int>(len));
        const int byteCount = 1920 * m_format.channelCount();
        while (m_buffer.size() > byteCount)
        {
            // qDebug() << QDateTime::currentMSecsSinceEpoch() << "CAudioInputBuffer::writeData " << m_buffer.size();
            emit frameAvailable(m_buffer.left(byteCount));
            m_buffer.remove(0, byteCount);
        }
        return len;
    }

    CInput::CInput(int sampleRate, QObject *parent) : QObject(parent),
                                                      m_sampleRate(sampleRate),
                                                      m_encoder(sampleRate, 1, OPUS_APPLICATION_VOIP)
    {
        this->setObjectName("CInput");
        m_encoder.setBitRate(16 * 1024);
    }

    bool CInput::setGainRatio(double gainRatio)
    {
        if (qFuzzyCompare(m_gainRatio, gainRatio)) { return false; }
        m_gainRatio = gainRatio;
        return true;
    }

    /*
    double CInput::getDeviceInputVolume() const
    {
        if (m_audioInput) { return static_cast<double>(m_audioInput->volume()); }
        return 0.0;
    }

    bool CInput::setDeviceInputVolume(double volume)
    {
        if (!m_audioInput && m_started) { return false; }
        const qreal v = normalize0to100qr(volume);
        m_audioInput->setVolume(v);
        return true;
    }
    */

    void CInput::start(const CAudioDeviceInfo &inputDevice)
    {
        if (m_started) { return; }

        SWIFT_VERIFY_X(inputDevice.isValid() && inputDevice.isInputDevice(), Q_FUNC_INFO, "Wrong input device");
        m_device = inputDevice;

        QAudioFormat inputFormat;
        inputFormat.setSampleRate(m_sampleRate); // normally 48000
        inputFormat.setChannelCount(1);
        inputFormat.setSampleFormat(QAudioFormat::Int16);

        QAudioDevice selectedDevice = getLowestLatencyDevice(inputDevice, inputFormat);
        m_inputFormat = inputFormat;
        m_audioInput.reset(new QAudioSource(selectedDevice, m_inputFormat));
        if (!m_audioInputBuffer) { m_audioInputBuffer = new CAudioInputBuffer(this); }
        else { m_audioInputBuffer->disconnect(); } // make sure disconnected in any case
        m_audioInputBuffer->start(m_inputFormat);

#ifdef Q_OS_MAC
        CMacOSMicrophoneAccess::AuthorizationStatus status = m_micAccess.getAuthorizationStatus();
        if (status == CMacOSMicrophoneAccess::Authorized)
        {
            m_audioInput->start(m_audioInputBuffer);
            connect(m_audioInputBuffer, &CAudioInputBuffer::frameAvailable, this, &CInput::audioInDataAvailable);
            m_started = true;
            return;
        }
        else if (status == CMacOSMicrophoneAccess::NotDetermined)
        {
            connect(&m_micAccess, &CMacOSMicrophoneAccess::permissionRequestAnswered, this, &CInput::delayedInitMicrophone);
            m_micAccess.requestAccess();
            CLogMessage(this).info(u"MacOS requested input device");
        }
        else
        {
            CLogMessage(this).error(u"Microphone access not granted. Voice input will not work.");
            return;
        }
#else
        m_audioInput->start(m_audioInputBuffer);
        connect(m_audioInputBuffer, &CAudioInputBuffer::frameAvailable, this, &CInput::audioInDataAvailable);
        m_started = true;
#endif
        const QString format = toQString(m_inputFormat);
        CLogMessage(this).info(u"Starting: '%1' with: %2") << selectedDevice.description() << format;
    }

    void CInput::stop()
    {
        if (!m_started) { return; }
        m_started = false;
        if (m_audioInput) { m_audioInput->stop(); }
        m_audioInput.reset();
        if (m_audioInputBuffer)
        {
            m_audioInputBuffer->stop();
            m_audioInputBuffer->deleteLater();
            m_audioInputBuffer = nullptr;
        }
    }

    void CInput::audioInDataAvailable(const QByteArray &frame)
    {
        static_assert(Q_BYTE_ORDER == Q_LITTLE_ENDIAN);
        QVector<qint16> samples = convertBytesTo16BitPCM(frame);

        if (m_inputFormat.channelCount() == 2)
        {
            samples = convertFromStereoToMono(samples);
        }

        const double volume = m_gainRatio;
        for (qint16 &sample : samples)
        {
            int value = qRound(sample * volume);
            if (value > std::numeric_limits<qint16>::max()) value = std::numeric_limits<qint16>::max();
            if (value < std::numeric_limits<qint16>::min()) value = std::numeric_limits<qint16>::min();
            sample = static_cast<qint16>(value);

            qint16 sampleInput = qAbs(sample);
            m_maxSampleInput = qMax(qAbs(sampleInput), m_maxSampleInput);
        }

        int length;
        const QByteArray encodedBuffer = m_encoder.encode(samples, samples.size(), &length);
        m_opusBytesEncoded += length;

        m_sampleCount += samples.size();
        if (m_sampleCount >= SampleCountPerEvent)
        {
            InputVolumeStreamArgs inputVolumeStreamArgs;
            qint16 maxInt = std::numeric_limits<qint16>::max();
            inputVolumeStreamArgs.PeakRaw = static_cast<float>(m_maxSampleInput) / maxInt;
            inputVolumeStreamArgs.PeakDB = static_cast<float>(20 * std::log10(inputVolumeStreamArgs.PeakRaw));
            double db = qBound(minDb, inputVolumeStreamArgs.PeakDB, maxDb);
            double ratio = (db - minDb) / (maxDb - minDb);
            if (ratio < 0.30) { ratio = 0.0; }
            if (ratio > 1.0) { ratio = 1.0; }
            inputVolumeStreamArgs.PeakVU = ratio;
            emit inputVolumeStream(inputVolumeStreamArgs);
            m_sampleCount = 0;
            m_maxSampleInput = 0;
        }

        OpusDataAvailableArgs opusDataAvailableArgs = { m_audioSequenceCounter++, encodedBuffer };
        emit opusDataAvailable(opusDataAvailableArgs);
    }

#ifdef Q_OS_MAC
    void CInput::delayedInitMicrophone()
    {
        m_audioInput->start(m_audioInputBuffer);
        connect(m_audioInputBuffer, &CAudioInputBuffer::frameAvailable, this, &CInput::audioInDataAvailable);
        m_started = true;
    }
#endif

} // ns
