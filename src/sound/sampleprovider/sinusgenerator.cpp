// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "sinusgenerator.h"

#include <cmath>

#include "misc/metadatautils.h"

using namespace swift::misc;

namespace swift::sound::sample_provider
{
    CSinusGenerator::CSinusGenerator(double frequencyHz, QObject *parent)
        : ISampleProvider(parent), m_frequencyHz(frequencyHz)
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
            const double multiple = s_twoPi * m_frequencyHz / m_sampleRate;
            const double sampleValue = m_gain * qSin(m_nSample * multiple);
            samples[sampleCount] = static_cast<float>(sampleValue);
            m_nSample++;
        }
        return static_cast<int>(count);
    }

    void CSinusGenerator::setFrequency(double frequencyHz) { m_frequencyHz = frequencyHz; }
} // namespace swift::sound::sample_provider
