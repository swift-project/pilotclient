/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "sawtoothgenerator.h"
#include <cmath>

namespace BlackSound::SampleProvider
{
    CSawToothGenerator::CSawToothGenerator(double frequency, QObject *parent) :
        ISampleProvider(parent),
        m_frequency(frequency)
    {
        this->setObjectName("CSawToothGenerator");
    }

    int CSawToothGenerator::readSamples(QVector<float> &samples, qint64 count)
    {
        samples.clear();
        samples.fill(0, static_cast<int>(count));

        for (int sampleCount = 0; sampleCount < count; sampleCount++)
        {
            double multiple = 2 * m_frequency / m_sampleRate;
            double sampleSaw = std::fmod((m_nSample * multiple), 2) - 1;
            double sampleValue = m_gain * sampleSaw;
            samples[sampleCount] = static_cast<float>(sampleValue);
            m_nSample++;
        }
        return static_cast<int>(count);
    }
} // ns
