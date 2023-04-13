#include "bufferedwaveprovider.h"
#include "blacksound/audioutilities.h"

#include <QDebug>

namespace BlackSound::SampleProvider
{
    CBufferedWaveProvider::CBufferedWaveProvider(const QAudioFormat &format, QObject *parent) : ISampleProvider(parent)
    {
        const QString on = QStringLiteral("%1 format: '%2'").arg(this->metaObject()->className(), BlackSound::toQString(format));
        this->setObjectName(on);

        // Set buffer size to 10 secs
        m_maxBufferSize = format.bytesForDuration(10 * 1000 * 1000);
    }

    void CBufferedWaveProvider::addSamples(const QVector<float> &samples)
    {
        int delta = m_audioBuffer.size() + samples.size() - m_maxBufferSize;
        if (delta > 0)
        {
            m_audioBuffer.remove(0, delta);
        }
        m_audioBuffer.append(samples);
    }

    int CBufferedWaveProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        const int len = static_cast<int>(qMin(count, static_cast<qint64>(m_audioBuffer.size())));
        samples = m_audioBuffer.mid(0, len);
        // if (len != 0) qDebug() << "Reading" << count << "samples." << m_audioBuffer.size() << "currently in the buffer.";
        m_audioBuffer.remove(0, len);
        return len;
    }

    void CBufferedWaveProvider::clearBuffer()
    {
        m_audioBuffer.clear();
    }
} // ns
