// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "sawtoothgenerator.h"

#include <cmath>

namespace swift::sound::sample_provider
{
    CSawToothGenerator::CSawToothGenerator(double frequency, QObject *parent) : ISampleProvider(parent),
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
} // namespace swift::sound::sample_provider
