#include "equalizersampleprovider.h"

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

        int CEqualizerSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
        {
            int samplesRead = m_sourceProvider->readSamples(samples, count);
            if (m_bypass) return samplesRead;

            for (int n = 0; n < samplesRead; n++)
            {
                // TODO stereo implementation

                for (int band = 0; band < m_filters.size(); band++)
                {
                    float s = samples[n] / 32768.0f;
                    s = m_filters[band].process(s);
                    samples[n] = s * 32768;
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
                m_filters.push_back(BiQuadFilter(BiQuadFilterType::HighPass, 44100, 310, 0.25f));
                m_filters.push_back(BiQuadFilter(BiQuadFilterType::Peak, 44100, 450, 0.75f, 17.0f));
                m_filters.push_back(BiQuadFilter(BiQuadFilterType::Peak, 44100, 1450, 1.0f, 25.0f));
                m_filters.push_back(BiQuadFilter(BiQuadFilterType::Peak, 44100, 2000, 1.0f, 25.0f));
                m_filters.push_back(BiQuadFilter(BiQuadFilterType::LowPass, 44100, 2500, 0.25f));
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
