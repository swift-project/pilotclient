#include "mixingsampleprovider.h"

int MixingSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
{
    samples.clear();
    samples.fill(0, count);
    int outputLen = 0;

    QVector<ISampleProvider*> finishedProviders;
    for (int i = 0; i < m_sources.size(); i++)
    {
        ISampleProvider *sampleProvider = m_sources.at(i);
        QVector<qint16> sourceBuffer;
        int len = sampleProvider->readSamples(sourceBuffer, count);

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
