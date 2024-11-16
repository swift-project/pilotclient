// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simplecompressoreffect.h"

#include <QDebug>

namespace swift::sound::sample_provider
{
    CSimpleCompressorEffect::CSimpleCompressorEffect(ISampleProvider *source, QObject *parent)
        : ISampleProvider(parent), m_sourceStream(source)
    {

        this->setObjectName("CSimpleCompressorEffect");
        m_timer = new QTimer(this);
        m_timer->setObjectName(this->objectName() + ":m_timer");

        m_simpleCompressor.setAttack(5.0);
        m_simpleCompressor.setRelease(10.0);
        m_simpleCompressor.setSampleRate(48000.0);
        m_simpleCompressor.setThresh(16.0);
        m_simpleCompressor.setRatio(6.0);
        m_simpleCompressor.setMakeUpGain(16.0);
        m_timer->start(3000);
    }

    int CSimpleCompressorEffect::readSamples(QVector<float> &samples, qint64 count)
    {
        int samplesRead = m_sourceStream->readSamples(samples, count);

        if (m_enabled)
        {
            for (int sample = 0; sample < samplesRead; sample += m_channels)
            {
                double in1 = samples.at(sample);
                double in2 = (m_channels == 1) ? 0 : samples.at(sample + 1);
                m_simpleCompressor.process(in1, in2);
                samples[sample] = static_cast<float>(in1);
                if (m_channels > 1) { samples[sample + 1] = static_cast<float>(in2); }
            }
        }
        return samplesRead;
    }

    void CSimpleCompressorEffect::setEnabled(bool enabled) { m_enabled = enabled; }

    void CSimpleCompressorEffect::setMakeUpGain(double gain) { m_simpleCompressor.setMakeUpGain(gain); }

    void CSimpleCompressorEffect::setChannels(int channels)
    {
        if (channels < 1) { channels = 1; }
        else if (channels > 2) { channels = 2; }
        m_channels = channels;
    }
} // namespace swift::sound::sample_provider
