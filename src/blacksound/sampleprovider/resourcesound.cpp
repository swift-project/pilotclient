#include "resourcesound.h"
#include "audioutilities.h"

ResourceSound::ResourceSound(const QString &audioFileName)
{
    m_wavFile = new WavFile;
    m_wavFile->open(audioFileName);
    if (m_wavFile->fileFormat().sampleType() == QAudioFormat::Float)
    {
        m_samples = convertFloatBytesTo16BitPCM(m_wavFile->audioData());
    }
    else
    {
        m_samples = convertBytesTo16BitPCM(m_wavFile->audioData());
    }

}

const QVector<qint16>& ResourceSound::audioData()
{
    return m_samples;
}
