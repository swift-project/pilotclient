#include "opusdecoder.h"

COpusDecoder::COpusDecoder(int sampleRate, int channels) :
    m_sampleRate(sampleRate),
    m_channels(channels)
{
    int error;
    opusDecoder = opus_decoder_create(sampleRate, channels, &error);
}

COpusDecoder::~COpusDecoder()
{
    opus_decoder_destroy(opusDecoder);
}

int COpusDecoder::frameCount(int bufferSize)
{
    //  seems like bitrate should be required
    int bitrate = 16;
    int bytesPerSample = (bitrate / 8) * m_channels;
    return bufferSize / bytesPerSample;
}

QVector<qint16> COpusDecoder::decode(const QByteArray opusData, int dataLength, int *decodedLength)
{
    QVector<qint16> decoded(maxDataBytes, 0);
    int count = frameCount(maxDataBytes);

    if (! opusData.isEmpty())
    {
        *decodedLength = opus_decode(opusDecoder, reinterpret_cast<const unsigned char*>(opusData.data()), dataLength, decoded.data(), count, 0);
    }
    decoded.resize(*decodedLength);
    return decoded;
}

void COpusDecoder::resetState()
{
    opus_decoder_ctl(opusDecoder, OPUS_RESET_STATE);
}
