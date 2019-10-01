#include "equalizersampleprovider.h"
#include "blacksound/audioutilities.h"
#include <QDebug>

namespace BlackSound
{
    namespace SampleProvider
    {
        CEqualizerSampleProvider::CEqualizerSampleProvider(ISampleProvider *sourceProvider, EqualizerPresets preset, QObject *parent) :
            ISampleProvider(parent)
        {
            m_sourceProvider = sourceProvider;
            setupPreset(preset);
        }

        int CEqualizerSampleProvider::readSamples(QVector<float> &samples, qint64 count)
        {
            int samplesRead = m_sourceProvider->readSamples(samples, count);
            if (m_bypass) return samplesRead;

            for (int n = 0; n < samplesRead; n++)
            {
                for (int band = 0; band < m_filters.size(); band++)
                {
                    samples[n] = m_filters[band].transform(samples[n]);
                }
                samples[n] *= m_outputGain;
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

        double CEqualizerSampleProvider::outputGain() const
        {
            return m_outputGain;
        }

        void CEqualizerSampleProvider::setOutputGain(double outputGain)
        {
            m_outputGain = outputGain;
        }
    }
}
