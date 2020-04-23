/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "mixingsampleprovider.h"
#include "blackmisc/metadatautils.h"

using namespace BlackMisc;

namespace BlackSound
{
    namespace SampleProvider
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
} // ns
