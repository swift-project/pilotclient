/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "sinusgenerator.h"
#include <cmath>

namespace BlackSound
{
    namespace SampleProvider
    {
        CSinusGenerator::CSinusGenerator(double frequency, QObject *parent) :
            ISampleProvider(parent),
            m_frequency(frequency)
        {}

        int CSinusGenerator::readSamples(QVector<float> &samples, qint64 count)
        {
            samples.clear();
            samples.fill(0, static_cast<int>(count));

            for (int sampleCount = 0; sampleCount < count; sampleCount++)
            {
                double multiple = m_twoPi * m_frequency / m_sampleRate;
                double sampleValue = m_gain * qSin(m_nSample * multiple);
                samples[sampleCount] = static_cast<float>(sampleValue);
                m_nSample++;
            }
            return static_cast<int>(count);
        }

        void CSinusGenerator::setFrequency(double frequency)
        {
            m_frequency = frequency;
        }
    } // ns
} // ns
