#ifndef BLACKSOUND_BUFFEREDWAVEPROVIDER_H
#define BLACKSOUND_BUFFEREDWAVEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QAudioFormat>
#include <QByteArray>
#include <QVector>

class BLACKSOUND_EXPORT BufferedWaveProvider : public ISampleProvider
{
    Q_OBJECT

public:
    BufferedWaveProvider(const QAudioFormat &format, QObject *parent = nullptr);

    void addSamples(const QVector<qint16> &samples);
    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

    int getBufferedBytes() const { return m_audioBuffer.size(); }

    void clearBuffer();

private:
    QVector<qint16> m_audioBuffer;
    qint32 m_maxBufferSize;
};

#endif // BUFFEREDWAVEPROVIDER_H
