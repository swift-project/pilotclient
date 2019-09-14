#include "bufferedwaveprovider.h"

#include <QDebug>

BufferedWaveProvider::BufferedWaveProvider(const QAudioFormat &format, QObject *parent) :
    ISampleProvider(parent)
{
    // Set buffer size to 10 secs
    m_maxBufferSize = format.bytesForDuration(10 * 1000 * 1000);
}

void BufferedWaveProvider::addSamples(const QVector<qint16> &samples)
{
    int delta = m_audioBuffer.size() + samples.size() - m_maxBufferSize;
    if(delta > 0)
    {
        m_audioBuffer.remove(0, delta);
    }
    m_audioBuffer.append(samples);
}

int BufferedWaveProvider::readSamples(QVector<qint16> &samples, qint64 count)
{
    qint64 len = qMin(count, static_cast<qint64>(m_audioBuffer.size()));
    samples = m_audioBuffer.mid(0, len);
    // if (len != 0) qDebug() << "Reading" << count << "samples." << m_audioBuffer.size() << "currently in the buffer.";
    m_audioBuffer.remove(0, len);
    return len;
}

void BufferedWaveProvider::clearBuffer()
{
    m_audioBuffer.clear();
}
