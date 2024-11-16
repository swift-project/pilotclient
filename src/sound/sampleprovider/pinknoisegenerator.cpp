// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "pinknoisegenerator.h"

namespace swift::sound::sample_provider
{
    int CPinkNoiseGenerator::readSamples(QVector<float> &samples, qint64 count)
    {
        const int c = static_cast<int>(count);
        samples.clear();
        samples.fill(0, c);

        for (int sampleCount = 0; sampleCount < count; sampleCount++)
        {
            double white = 2 * m_random.generateDouble() - 1;

            m_pinkNoiseBuffer[0] = 0.99886 * m_pinkNoiseBuffer[0] + white * 0.0555179;
            m_pinkNoiseBuffer[1] = 0.99332 * m_pinkNoiseBuffer[1] + white * 0.0750759;
            m_pinkNoiseBuffer[2] = 0.96900 * m_pinkNoiseBuffer[2] + white * 0.1538520;
            m_pinkNoiseBuffer[3] = 0.86650 * m_pinkNoiseBuffer[3] + white * 0.3104856;
            m_pinkNoiseBuffer[4] = 0.55000 * m_pinkNoiseBuffer[4] + white * 0.5329522;
            m_pinkNoiseBuffer[5] = -0.7616 * m_pinkNoiseBuffer[5] - white * 0.0168980;
            double pink = m_pinkNoiseBuffer[0] + m_pinkNoiseBuffer[1] + m_pinkNoiseBuffer[2] + m_pinkNoiseBuffer[3] + m_pinkNoiseBuffer[4] + m_pinkNoiseBuffer[5] + m_pinkNoiseBuffer[6] + white * 0.5362;
            m_pinkNoiseBuffer[6] = white * 0.115926;
            const float sampleValue = static_cast<float>(m_gain * (pink / 5));
            samples[sampleCount] = sampleValue;
        }
        return c;
    }
} // namespace swift::sound::sample_provider
