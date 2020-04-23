/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "resourcesoundsampleprovider.h"
#include "blackmisc/metadatautils.h"

#include <QDebug>

using namespace BlackMisc;

namespace BlackSound
{
    namespace SampleProvider
    {
        CResourceSoundSampleProvider::CResourceSoundSampleProvider(const CResourceSound &resourceSound, QObject *parent) :
            ISampleProvider(parent),
            m_resourceSound(resourceSound)
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
            const qint64 samplesToCopy    = qMin(availableSamples, count);
            samples.clear();
            samples.fill(0, static_cast<int>(samplesToCopy));

            for (int i = 0; i < samplesToCopy; i++)
            {
                m_tempBuffer[i] = m_resourceSound.audioData().at(static_cast<int>(m_position) + i);
            }

            if (!qFuzzyCompare(m_gain, 1.0))
            {
                for (int i = 0; i < samplesToCopy; i++)
                {
                    m_tempBuffer[i] = static_cast<float>(m_gain * m_tempBuffer[i]);
                }
            }

            for (int i = 0; i < samplesToCopy; i++)
            {
                samples[i] = m_tempBuffer.at(i);
            }

            m_position += samplesToCopy;

            if (m_position > availableSamples - 1)
            {
                if (m_looping) { m_position = 0; }
                else { m_isFinished = true; }
            }

            return static_cast<int>(samplesToCopy);
        }
    } // ns
} // ns
