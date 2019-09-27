#include "resourcesoundsampleprovider.h"
#include "resourcesoundsampleprovider.h"
#include <QDebug>

namespace BlackSound
{
    namespace SampleProvider
    {
        CResourceSoundSampleProvider::CResourceSoundSampleProvider(const CResourceSound &resourceSound, QObject *parent) :
            ISampleProvider(parent),
            m_resourceSound(resourceSound)
        {
            m_tempBuffer.resize(m_tempBufferSize);
        }

        int CResourceSoundSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
        {
            if (count > m_tempBufferSize)
            {
                qDebug() << "Count too large for temp buffer" << count;
                return 0;
            }
            qint64 availableSamples = m_resourceSound.audioData().size() - m_position;

            const qint64 samplesToCopy = qMin(availableSamples, count);
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
                    m_tempBuffer[i] = static_cast<qint16>(qRound(m_gain * m_tempBuffer[i]));
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
