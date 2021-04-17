/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "output.h"
#include "blacksound/audioutilities.h"
#include "blackmisc/metadatautils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"

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
                Q_ASSERT_X(sampleProvider, Q_FUNC_INFO, "need sample provide");
                const QString on = QStringLiteral("%1 for %2").arg(classNameShort(this), sampleProvider->objectName());
                this->setObjectName(on);
            }

            qint64 CAudioOutputBuffer::readData(char *data, qint64 maxlen)
            {
                const int sampleBytes  = m_outputFormat.sampleSize() / 8;
                const int channelCount = m_outputFormat.channelCount();
                const qint64 count     = maxlen / (sampleBytes * channelCount);
                QVector<float> buffer;
                m_sampleProvider->readSamples(buffer, count);

                for (float sample : std::as_const(buffer))
                {
                    const float absSample = qAbs(sample);
                    if (absSample > m_maxSampleOutput) { m_maxSampleOutput = absSample; }
                }

                m_sampleCount += buffer.size();
                if (m_sampleCount >= SampleCountPerEvent)
                {
                    OutputVolumeStreamArgs outputVolumeStreamArgs;
                    outputVolumeStreamArgs.PeakRaw = m_maxSampleOutput / 1.0;
                    outputVolumeStreamArgs.PeakDb  = static_cast<float>(20 * std::log10(outputVolumeStreamArgs.PeakRaw));
                    const double db = qBound(m_minDb, outputVolumeStreamArgs.PeakDb, m_maxDb);
                    double ratio = (db - m_minDb) / (m_maxDb - m_minDb);
                    if (ratio < 0.30) { ratio = 0.0; }
                    if (ratio > 1.0)  { ratio = 1.0; }
                    outputVolumeStreamArgs.PeakVU = ratio;
                    emit outputVolumeStream(outputVolumeStreamArgs);
                    m_sampleCount     = 0;
                    m_maxSampleOutput = 0;
                }

                if (channelCount == 2)
                {
                    buffer = convertFromMonoToStereo(buffer);
                }

                memcpy(data, buffer.constData(), static_cast<size_t>(maxlen));
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
                this->setObjectName(classNameShort(this));
            }

            void COutput::start(const CAudioDeviceInfo &outputDevice, ISampleProvider *sampleProvider)
            {
                if (m_started) { return; }

                BLACK_VERIFY_X(outputDevice.isValid() && outputDevice.isOutputDevice(), Q_FUNC_INFO, "Wrong output device");

                if (m_audioOutputBuffer) { m_audioOutputBuffer->deleteLater(); }
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

                m_audioOutput.reset(new QAudioOutput(selectedDevice, outputFormat));
                m_audioOutputBuffer->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
                m_audioOutputBuffer->setAudioFormat(outputFormat);
                m_audioOutput->start(m_audioOutputBuffer);

                m_started = true;
            }

            void COutput::stop()
            {
                if (!m_started) { return; }
                m_started = false;
                m_audioOutput->stop();
                m_audioOutput.reset();
                if (m_audioOutputBuffer)
                {
                    m_audioOutputBuffer->deleteLater();
                    m_audioOutputBuffer = nullptr;
                }
            }

            /*
            double COutput::getDeviceOutputVolume() const
            {
                if (m_audioOutput && m_started) { return static_cast<double>(m_audioOutput->volume()); }
                return 0.0;
            }

            bool COutput::setDeviceOutputVolume(double volume)
            {
                if (!m_audioOutput) { return false; }
                const qreal v = normalize0to100qr(volume);
                m_audioOutput->setVolume(v);
                return true;
            }
            */

        } // ns
    } // ns
} // ns
