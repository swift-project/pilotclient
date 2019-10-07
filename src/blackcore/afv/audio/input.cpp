/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "input.h"
#include "blackmisc/logmessage.h"
#include "blacksound/audioutilities.h"

#include <QtGlobal>
#include <QStringBuilder>
#include <QDebug>
#include <QAudioDeviceInfo>
#include <cmath>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackSound;

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            CAudioInputBuffer::CAudioInputBuffer(QObject *parent) :
                QIODevice(parent)
            {
                this->setObjectName("CAudioInputBuffer");
            }

            void CAudioInputBuffer::start()
            {
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
                while (m_buffer.size() > 1920)
                {
                    // qDebug() << QDateTime::currentMSecsSinceEpoch() << "CAudioInputBuffer::writeData " << m_buffer.size();
                    emit frameAvailable(m_buffer.left(1920));
                    m_buffer.remove(0, 1920);
                }
                return len;
            }

            /**
            void CAudioInputBuffer::timerEvent(QTimerEvent *event)
            {
                Q_UNUSED(event)
                // 20 ms = 960 samples * 2 bytes = 1920 Bytes
                if (m_buffer.size() < 1920) { return; }

                const qint64 now   = QDateTime::currentMSecsSinceEpoch();
                const qint64 delta = now - m_lastFrameSent;
                if (delta >= 19)
                {
                    // qDebug() << now << "[signal] frameAvailable - buffer size" << m_buffer.size();
                    m_buffer.remove(0, 1920);
                    m_lastFrameSent = now;
                    emit frameAvailable(m_buffer.left(1920));
                }
            }
            **/

            CInput::CInput(int sampleRate, QObject *parent) :
                QObject(parent),
                m_sampleRate(sampleRate),
                m_encoder(sampleRate, 1, OPUS_APPLICATION_VOIP)
            {
                this->setObjectName("CInput");
                m_encoder.setBitRate(16 * 1024);
            }

            void CInput::start(const CAudioDeviceInfo &inputDevice)
            {
                if (m_started) { return; }

                m_device = inputDevice;

                m_inputFormat.setSampleRate(m_sampleRate);
                m_inputFormat.setChannelCount(1);
                m_inputFormat.setSampleSize(16);
                m_inputFormat.setSampleType(QAudioFormat::SignedInt);
                m_inputFormat.setByteOrder(QAudioFormat::LittleEndian);
                m_inputFormat.setCodec("audio/pcm");

                QAudioDeviceInfo selectedDevice = getLowestLatencyDevice(inputDevice, m_inputFormat);
                m_audioInput.reset(new QAudioInput(selectedDevice, m_inputFormat));
                m_audioInputBuffer.start();

                m_audioInput->start(&m_audioInputBuffer);
                connect(&m_audioInputBuffer, &CAudioInputBuffer::frameAvailable, this, &CInput::audioInDataAvailable);

                m_started = true;
            }

            void CInput::stop()
            {
                if (!m_started) { return; }
                m_started = false;
                if (m_audioInput) { m_audioInput->stop(); }
                m_audioInput.reset();
                m_audioInputBuffer.stop();
            }

            void CInput::audioInDataAvailable(const QByteArray &frame)
            {
                QVector<qint16> samples = convertBytesTo16BitPCM(frame);

                if (m_inputFormat.channelCount() == 2)
                {
                    samples = convertFromStereoToMono(samples);
                }

                int value = 0;
                for (qint16 &sample : samples)
                {
                    value = qRound(sample * m_volume);
                    if (value > std::numeric_limits<qint16>::max())
                        value = std::numeric_limits<qint16>::max();
                    if (value < std::numeric_limits<qint16>::min())
                        value = std::numeric_limits<qint16>::min();
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
                    inputVolumeStreamArgs.PeakDB  = static_cast<float>(20 * std::log10(inputVolumeStreamArgs.PeakRaw));
                    double db = qBound(minDb, inputVolumeStreamArgs.PeakDB, maxDb);
                    double ratio = (db - minDb) / (maxDb - minDb);
                    if (ratio < 0.30) { ratio = 0.0; }
                    if (ratio > 1.0)  { ratio = 1.0; }
                    inputVolumeStreamArgs.PeakVU = ratio;
                    emit inputVolumeStream(inputVolumeStreamArgs);
                    m_sampleCount = 0;
                    m_maxSampleInput = 0;
                }

                OpusDataAvailableArgs opusDataAvailableArgs = { m_audioSequenceCounter++, encodedBuffer };
                emit opusDataAvailable(opusDataAvailableArgs);
            }

        } // ns
    } // ns
} // ns
