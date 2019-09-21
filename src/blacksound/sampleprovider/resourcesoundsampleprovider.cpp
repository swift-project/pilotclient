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
            tempBuffer.resize(tempBufferSize);
        }

        int CResourceSoundSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
        {
            if (count > tempBufferSize)
            {
                qDebug() << "Count too large for temp buffer";
                return 0;
            }
            qint64 availableSamples = m_resourceSound.audioData().size() - position;

            qint64 samplesToCopy = qMin(availableSamples, count);
            samples.clear();
            samples.fill(0, samplesToCopy);

            for (qint64 i = 0; i < samplesToCopy; i++)
            {
                tempBuffer[i] = m_resourceSound.audioData().at(position + i);
            }

            if (m_gain != 1.0f)
            {
                for (int i = 0; i < samplesToCopy; i++)
                {
                    tempBuffer[i] *= m_gain;
                }
            }

            for (qint64 i = 0; i < samplesToCopy; i++)
            {
                samples[i] = tempBuffer.at(i);
            }

            position += samplesToCopy;

            if (position > availableSamples - 1)
            {
                if (m_looping) { position = 0; }
                else { m_isFinished = true; }
            }

            return (int)samplesToCopy;
        }

        bool CResourceSoundSampleProvider::isFinished()
        {
            return m_isFinished;
        }

        bool CResourceSoundSampleProvider::looping() const
        {
            return m_looping;
        }

        void CResourceSoundSampleProvider::setLooping(bool looping)
        {
            m_looping = looping;
        }

        float CResourceSoundSampleProvider::gain() const
        {
            return m_gain;
        }

        void CResourceSoundSampleProvider::setGain(float gain)
        {
            m_gain = gain;
        }
    }
}
