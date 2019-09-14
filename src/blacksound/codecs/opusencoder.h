#ifndef OPUSENCODER_H
#define OPUSENCODER_H

#include "blacksound/blacksoundexport.h"
#include "opus/opus.h"

#include <QByteArray>
#include <QVector>

class BLACKSOUND_EXPORT COpusEncoder
{
public:
    COpusEncoder(int sampleRate, int channels, int application = OPUS_APPLICATION_VOIP);
    ~COpusEncoder();

    void setBitRate(int bitRate);

    //! \param frameCount Number of audio samples per frame
    //! \returns the size of an audio frame in bytes
    int frameByteCount(int frameCount);

    int frameCount(const QVector<qint16> pcmSamples);

    QByteArray encode(const QVector<qint16> pcmSamples, int samplesLength, int *encodedLength);

private:
    OpusEncoder *opusEncoder;
    int m_sampleRate;
    int m_channels;

    static constexpr int maxDataBytes = 4000;
};

#endif // OPUSENCODER_H
