#ifndef OPUSDECODER_H
#define OPUSDECODER_H

#include "blacksound/blacksoundexport.h"

#include "opus/opus.h"

#include <QVector>

class BLACKSOUND_EXPORT COpusDecoder
{
public:
    COpusDecoder(int sampleRate, int channels);
    ~COpusDecoder();

    int frameCount(int bufferSize);

    QVector<qint16> decode(const QByteArray opusData, int dataLength, int *decodedLength);

    void resetState();

private:
    OpusDecoder *opusDecoder;
    int m_sampleRate;
    int m_channels;

    static constexpr int maxDataBytes = 4000;
};

#endif // OPUSDECODER_H
