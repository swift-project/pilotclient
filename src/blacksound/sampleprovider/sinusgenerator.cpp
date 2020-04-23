/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "sinusgenerator.h"
#include "blackmisc/metadatautils.h"
#include <cmath>

using namespace BlackMisc;

namespace BlackSound
{
    namespace SampleProvider
    {
        CSinusGenerator::CSinusGenerator(double frequencyHz, QObject *parent) :
            ISampleProvider(parent),
            m_frequencyHz(frequencyHz)
        {
            const QString on = QStringLiteral("%1 frequency: %2Hz").arg(classNameShort(this)).arg(frequencyHz);
            this->setObjectName(on);
        }

        int CSinusGenerator::readSamples(QVector<float> &samples, qint64 count)
        {
            samples.clear();
            samples.fill(0, static_cast<int>(count));

            for (int sampleCount = 0; sampleCount < count; sampleCount++)
            {
                const double multiple    = s_twoPi * m_frequencyHz / m_sampleRate;
                const double sampleValue = m_gain * qSin(m_nSample * multiple);
                samples[sampleCount]     = static_cast<float>(sampleValue);
                m_nSample++;
            }
            return static_cast<int>(count);
        }

        void CSinusGenerator::setFrequency(double frequencyHz)
        {
            m_frequencyHz = frequencyHz;
        }
    } // ns
} // ns
