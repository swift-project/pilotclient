/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "output.h"

#include <QDebug>
#include <cmath>

using namespace BlackSound::SampleProvider;

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            CAudioOutputBuffer::CAudioOutputBuffer(ISampleProvider *sampleProvider, QObject *parent) :
                QIODevice(parent),
                m_sampleProvider(sampleProvider)
            { }

            qint64 CAudioOutputBuffer::readData(char *data, qint64 maxlen)
            {
                int sampleBytes  = m_outputFormat.sampleSize() / 8;
                int channelCount = m_outputFormat.channelCount();
                qint64 count = maxlen / (sampleBytes * channelCount);
                QVector<qint16> buffer;
                m_sampleProvider->readSamples(buffer, count);

                for (const qint16 sample : buffer)
                {
                    qint16 sampleInput = sample;
                    sampleInput = qAbs(sampleInput);
                    if (sampleInput > m_maxSampleOutput)
                        m_maxSampleOutput = sampleInput;
                }

                m_sampleCount += buffer.size();
                if (m_sampleCount >= c_sampleCountPerEvent)
                {
                    OutputVolumeStreamArgs outputVolumeStreamArgs;
                    qint16 maxInt = std::numeric_limits<qint16>::max();
                    outputVolumeStreamArgs.PeakRaw = m_maxSampleOutput / maxInt;
                    outputVolumeStreamArgs.PeakDB = (float)(20 * std::log10(outputVolumeStreamArgs.PeakRaw));
                    float db = qBound(minDb, outputVolumeStreamArgs.PeakDB, maxDb);
                    float ratio = (db - minDb) / (maxDb - minDb);
                    if (ratio < 0.30)
                        ratio = 0;
                    if (ratio > 1.0)
                        ratio = 1;
                    outputVolumeStreamArgs.PeakVU = ratio;
                    emit outputVolumeStream(outputVolumeStreamArgs);
                    m_sampleCount = 0;
                    m_maxSampleOutput = 0;
                }

                // if (maxlen > buffer.size()) { maxlen = buffer.size(); }
                // memcpy(data, buffer.constData(), maxlen > buffer.size() ? buffer.size() : maxlen);
                qint16 *p = reinterpret_cast<qint16 *>(data);
                int index = 0;
                for (int n = 0; n < count; n++)
                {
                    p[index++] = buffer[n];
                    if (channelCount == 2) p[index++] = buffer[n];
                }

                return maxlen;
            }

            qint64 CAudioOutputBuffer::writeData(const char *data, qint64 len)
            {
                Q_UNUSED(data)
                Q_UNUSED(len)
                return -1;
            }

            Output::Output(QObject *parent) : QObject(parent)
            { }

            void Output::start(const QAudioDeviceInfo &device, ISampleProvider *sampleProvider)
            {
                m_audioOutputBuffer = new CAudioOutputBuffer(sampleProvider, this);
                connect(m_audioOutputBuffer, &CAudioOutputBuffer::outputVolumeStream, this, &Output::outputVolumeStream);

                QAudioFormat outputFormat;
                outputFormat.setSampleRate(48000);
                outputFormat.setChannelCount(1);
                outputFormat.setSampleSize(16);
                outputFormat.setSampleType(QAudioFormat::SignedInt);
                outputFormat.setByteOrder(QAudioFormat::LittleEndian);
                outputFormat.setCodec("audio/pcm");

                if (!device.isFormatSupported(outputFormat))
                {
                    qWarning() << "Default OUTPUT format not supported - trying to use nearest";
                    outputFormat = device.nearestFormat(outputFormat);
                    qWarning() << "Default format not supported - trying to use nearest:";
                    qWarning() << "Sample rate: " << outputFormat.sampleRate();
                    qWarning() << "Sample size: " << outputFormat.sampleSize();
                    qWarning() << "Sample type: " << outputFormat.sampleType();
                    qWarning() << "Byte order: " << outputFormat.byteOrder();
                    qWarning() << "Codec: " << outputFormat.codec();
                    qWarning() << "Channel count: " << outputFormat.channelCount();
                }

                m_audioOutputCom1.reset(new QAudioOutput(device, outputFormat));
                // m_audioOutput->setBufferSize(bufferSize);
                m_audioOutputBuffer->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
                m_audioOutputBuffer->setAudioFormat(outputFormat);
                m_audioOutputCom1->start(m_audioOutputBuffer);

                m_started = true;
            }

            void Output::stop()
            {
                if (!m_started) { return; }
                m_started = false;
            }
        } // ns
    } // ns
} // ns
