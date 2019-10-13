/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "output.h"
#include "blackmisc/logmessage.h"
#include "blacksound/audioutilities.h"

#include <QDebug>
#include <QStringBuilder>
#include <cmath>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackSound;
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
            {
                this->setObjectName("CAudioOutputBuffer");
            }

            qint64 CAudioOutputBuffer::readData(char *data, qint64 maxlen)
            {
                const int sampleBytes  = m_outputFormat.sampleSize() / 8;
                const int channelCount = m_outputFormat.channelCount();
                const qint64 count = maxlen / (sampleBytes * channelCount);
                QVector<float> buffer;
                m_sampleProvider->readSamples(buffer, count);

                for (float sample : buffer)
                {
                    const float absSample = qAbs(sample);
                    if (absSample > m_maxSampleOutput) { m_maxSampleOutput = absSample; }
                }

                m_sampleCount += buffer.size();
                if (m_sampleCount >= SampleCountPerEvent)
                {
                    OutputVolumeStreamArgs outputVolumeStreamArgs;
                    outputVolumeStreamArgs.PeakRaw = m_maxSampleOutput / 1.0;
                    outputVolumeStreamArgs.PeakDb = static_cast<float>(20 * std::log10(outputVolumeStreamArgs.PeakRaw));
                    const double db = qBound(m_minDb, outputVolumeStreamArgs.PeakDb, m_maxDb);
                    double ratio = (db - m_minDb) / (m_maxDb - m_minDb);
                    if (ratio < 0.30) { ratio = 0.0; }
                    if (ratio > 1.0)  { ratio = 1.0; }
                    outputVolumeStreamArgs.PeakVU = ratio;
                    emit outputVolumeStream(outputVolumeStreamArgs);
                    m_sampleCount = 0;
                    m_maxSampleOutput = 0;
                }

                if (channelCount == 2)
                {
                    buffer = convertFromMonoToStereo(buffer);
                }

                memcpy(data, buffer.constData(), maxlen);

                return maxlen;
            }

            qint64 CAudioOutputBuffer::writeData(const char *data, qint64 len)
            {
                Q_UNUSED(data)
                Q_UNUSED(len)
                return -1;
            }

            COutput::COutput(QObject *parent) : QObject(parent)
            {
                this->setObjectName("COutput");
            }

            void COutput::start(const CAudioDeviceInfo &outputDevice, ISampleProvider *sampleProvider)
            {
                if (m_started) { return; }

                m_audioOutputBuffer = new CAudioOutputBuffer(sampleProvider, this);
                connect(m_audioOutputBuffer, &CAudioOutputBuffer::outputVolumeStream, this, &COutput::outputVolumeStream);

                m_device = outputDevice;

                QAudioFormat outputFormat;
                outputFormat.setSampleRate(48000);
                outputFormat.setChannelCount(1);
                outputFormat.setSampleSize(32);
                outputFormat.setSampleType(QAudioFormat::Float);
                outputFormat.setByteOrder(QAudioFormat::LittleEndian);
                outputFormat.setCodec("audio/pcm");

                const QString format = toQString(outputFormat);
                const QAudioDeviceInfo selectedDevice = getLowestLatencyDevice(outputDevice, outputFormat);
                CLogMessage(this).info(u"Starting: '%1' with: %2") << selectedDevice.deviceName() << format;

                m_audioOutputCom.reset(new QAudioOutput(selectedDevice, outputFormat));
                m_audioOutputBuffer->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
                m_audioOutputBuffer->setAudioFormat(outputFormat);
                m_audioOutputCom->start(m_audioOutputBuffer);

                m_started = true;
            }

            void COutput::stop()
            {
                if (!m_started) { return; }
                m_started = false;
                m_audioOutputCom->stop();
                m_audioOutputCom.reset();
                if (m_audioOutputBuffer)
                {
                    m_audioOutputBuffer->deleteLater();
                    m_audioOutputBuffer = nullptr;
                }
            }
        } // ns
    } // ns
} // ns
