#include "sawtoothgenerator.h"
#include <cmath>

SawToothGenerator::SawToothGenerator(double frequency, QObject *parent) :
    ISampleProvider(parent),
    m_frequency(frequency)
{}

int SawToothGenerator::readSamples(QVector<qint16> &samples, qint64 count)
{
    samples.clear();
    samples.fill(0, count);

    for (int sampleCount = 0; sampleCount < count; sampleCount++)
    {
        double multiple = 2 * m_frequency / m_sampleRate;
        double sampleSaw = std::fmod((m_nSample * multiple), 2) - 1;
        double sampleValue = m_gain * sampleSaw;
        samples[sampleCount] = sampleValue * 32768;
        m_nSample++;
    }
    return count;
}
