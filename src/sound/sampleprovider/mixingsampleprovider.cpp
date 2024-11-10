// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "mixingsampleprovider.h"
#include "misc/metadatautils.h"

using namespace swift::misc;

namespace swift::sound::sample_provider
{
    CMixingSampleProvider::CMixingSampleProvider(QObject *parent) : ISampleProvider(parent)
    {
        const QString on = QStringLiteral("%1").arg(classNameShort(this));
        this->setObjectName(on);
    }

    void CMixingSampleProvider::addMixerInput(ISampleProvider *provider)
    {
        Q_ASSERT(provider);
        m_sources.append(provider);

        const QString on = QStringLiteral("%1 sources: %2").arg(classNameShort(this)).arg(m_sources.size());
        this->setObjectName(on);
    }

    int CMixingSampleProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        samples.clear();
        samples.fill(0, static_cast<int>(count));
        int outputLen = 0;

        QVector<ISampleProvider *> finishedProviders;
        for (int i = 0; i < m_sources.size(); i++)
        {
            ISampleProvider *sampleProvider = m_sources.at(i);
            QVector<float> sourceBuffer;

            const int len = sampleProvider->readSamples(sourceBuffer, count);
            for (int n = 0; n < len; n++)
            {
                samples[n] += sourceBuffer[n];
            }

            outputLen = qMax(len, outputLen);
            if (sampleProvider->isFinished())
            {
                finishedProviders.push_back(sampleProvider);
            }
        }

        for (ISampleProvider *sampleProvider : finishedProviders)
        {
            sampleProvider->deleteLater();
            m_sources.removeAll(sampleProvider);
        }

        return outputLen;
    }
} // ns
