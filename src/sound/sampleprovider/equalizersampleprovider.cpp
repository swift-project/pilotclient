// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "equalizersampleprovider.h"

#include <QDebug>

#include "sound/audioutilities.h"

using namespace swift::sound::dsp;

namespace swift::sound::sample_provider
{
    CEqualizerSampleProvider::CEqualizerSampleProvider(ISampleProvider *sourceProvider, EqualizerPresets preset,
                                                       QObject *parent)
        : ISampleProvider(parent)
    {
        Q_ASSERT_X(sourceProvider, Q_FUNC_INFO, "Need provider");
        const QString on =
            QStringLiteral("%1 of %2").arg(this->metaObject()->className(), sourceProvider->objectName());
        this->setObjectName(on);

        m_sourceProvider = sourceProvider;
        setupPreset(preset);
    }

    int CEqualizerSampleProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        const int samplesRead = m_sourceProvider->readSamples(samples, count);
        if (m_bypass) return samplesRead;

        for (int n = 0; n < samplesRead; n++)
        {
            for (int band = 0; band < m_filters.size(); band++) { samples[n] = m_filters[band].transform(samples[n]); }
            samples[n] *= static_cast<float>(m_outputGain);
        }
        return samplesRead;
    }

    void CEqualizerSampleProvider::setupPreset(EqualizerPresets preset)
    {
        switch (preset)
        {
        case VHFEmulation:
            m_filters.push_back(BiQuadFilter::highPassFilter(44100, 310, 0.25));
            m_filters.push_back(BiQuadFilter::peakingEQ(44100, 450, 0.75, 17.0));
            m_filters.push_back(BiQuadFilter::peakingEQ(44100, 1450, 1.0, 25.0));
            m_filters.push_back(BiQuadFilter::peakingEQ(44100, 2000, 1.0, 25.0));
            m_filters.push_back(BiQuadFilter::lowPassFilter(44100, 2500, 0.25));
            break;
        }
    }

    double CEqualizerSampleProvider::outputGain() const { return m_outputGain; }

    void CEqualizerSampleProvider::setOutputGain(double outputGain) { m_outputGain = outputGain; }
} // namespace swift::sound::sample_provider
