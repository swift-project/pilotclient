// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "resourcesoundsampleprovider.h"

#include <QDebug>

#include "misc/metadatautils.h"

using namespace swift::misc;

namespace swift::sound::sample_provider
{
    CResourceSoundSampleProvider::CResourceSoundSampleProvider(const CResourceSound &resourceSound, QObject *parent)
        : ISampleProvider(parent), m_resourceSound(resourceSound)
    {
        const QString on = QStringLiteral("%1 %2").arg(classNameShort(this), resourceSound.getFileName());
        this->setObjectName(on);
        m_tempBuffer.resize(m_tempBufferSize);
    }

    int CResourceSoundSampleProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        if (!m_resourceSound.isLoaded()) { return 0; }
        if (count > m_tempBufferSize)
        {
            qDebug() << "Count too large for temp buffer" << count;
            return 0;
        }
        const qint64 availableSamples = m_resourceSound.audioData().size() - m_position;
        const qint64 samplesToCopy = qMin(availableSamples, count);
        samples.clear();
        samples.fill(0, static_cast<int>(samplesToCopy));

        for (int i = 0; i < samplesToCopy; i++)
        {
            m_tempBuffer[i] = m_resourceSound.audioData().at(static_cast<int>(m_position) + i);
        }

        if (!qFuzzyCompare(m_gain, 1.0))
        {
            for (int i = 0; i < samplesToCopy; i++) { m_tempBuffer[i] = static_cast<float>(m_gain * m_tempBuffer[i]); }
        }

        for (int i = 0; i < samplesToCopy; i++) { samples[i] = m_tempBuffer.at(i); }

        m_position += samplesToCopy;

        if (m_position > availableSamples - 1)
        {
            if (m_looping) { m_position = 0; }
            else { m_isFinished = true; }
        }

        return static_cast<int>(samplesToCopy);
    }
} // namespace swift::sound::sample_provider
