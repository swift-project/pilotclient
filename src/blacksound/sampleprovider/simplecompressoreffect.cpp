#include "simplecompressoreffect.h"
#include <QDebug>

SimpleCompressorEffect::SimpleCompressorEffect(ISampleProvider *source, QObject *parent) :
    ISampleProvider(parent),
    m_sourceStream(source)
{
    m_simpleCompressor.setAttack(5.0);
    m_simpleCompressor.setRelease(10.0);
    m_simpleCompressor.setSampleRate(48000.0);
    m_simpleCompressor.setThresh(16.0);
    m_simpleCompressor.setRatio(6.0);
    m_simpleCompressor.setMakeUpGain(16.0);

    m_timer.start(3000);
}

int SimpleCompressorEffect::readSamples(QVector<qint16> &samples, qint64 count)
{
    int samplesRead = m_sourceStream->readSamples(samples, count);

    if (m_enabled)
    {
        for (int sample = 0; sample < samplesRead; sample+=channels)
        {
            double in1 = samples.at(sample) / 32768.0;
            double in2 = (channels == 1) ? 0 : samples.at(sample+1);
            m_simpleCompressor.process(in1, in2);
            samples[sample] = in1 * 32768.0;
            if (channels > 1)
                samples[sample + 1] = in2 * 32768.0f;
        }
    }
    return samplesRead;
}

void SimpleCompressorEffect::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void SimpleCompressorEffect::setMakeUpGain(double gain)
{
    m_simpleCompressor.setMakeUpGain(gain);
}
