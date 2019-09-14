#include "input.h"
#include "blacksound/audioutilities.h"

#include <QtGlobal>
#include <QDebug>
#include <cmath>

void AudioInputBuffer::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInputBuffer::stop()
{
    close();
}

qint64 AudioInputBuffer::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 AudioInputBuffer::writeData(const char *data, qint64 len)
{
    QByteArray buffer(data, static_cast<int>(len));
    m_buffer.append(buffer);
    // 20 ms = 960 samples * 2 bytes = 1920 Bytes
    if (m_buffer.size() >= 1920)
    {
        emit frameAvailable(m_buffer.left(1920));
        m_buffer.remove(0, 1920);
    }

    return len;
}

Input::Input(int sampleRate, QObject *parent) :
    QObject(parent),
    m_sampleRate(sampleRate),
    m_encoder(sampleRate, 1, OPUS_APPLICATION_VOIP)
{
    m_encoder.setBitRate(16 * 1024);
}

bool Input::started() const
{
    return m_started;
}

int Input::opusBytesEncoded() const
{
    return m_opusBytesEncoded;
}

void Input::setOpusBytesEncoded(int opusBytesEncoded)
{
    m_opusBytesEncoded = opusBytesEncoded;
}

float Input::volume() const
{
    return m_volume;
}

void Input::setVolume(float volume)
{
    m_volume = volume;
}

void Input::start(const QAudioDeviceInfo &inputDevice)
{
    if (m_started) { return; }

    QAudioFormat waveFormat;

    waveFormat.setSampleRate(m_sampleRate);
    waveFormat.setChannelCount(1);
    waveFormat.setSampleSize(16);
    waveFormat.setSampleType(QAudioFormat::SignedInt);
    waveFormat.setByteOrder(QAudioFormat::LittleEndian);
    waveFormat.setCodec("audio/pcm");

    QAudioFormat inputFormat = waveFormat;
    if (!inputDevice.isFormatSupported(inputFormat))
    {
        qWarning() << "Default format not supported - trying to use nearest";
        inputFormat = inputDevice.nearestFormat(inputFormat);
    }

    m_audioInput.reset(new QAudioInput(inputDevice, inputFormat));
    // We want 20 ms of buffer size
    // 20 ms * nSamplesPerSec × nChannels × wBitsPerSample  / 8 x 1000
    int bufferSize = 20 * inputFormat.sampleRate() * inputFormat.channelCount() * inputFormat.sampleSize() / ( 8 * 1000 );
    m_audioInput->setBufferSize(bufferSize);
    m_audioInputBuffer.start();
    m_audioInput->start(&m_audioInputBuffer);
    connect(&m_audioInputBuffer, &AudioInputBuffer::frameAvailable, this, &Input::audioInDataAvailable);

    m_started = true;
}

void Input::stop()
{
    if (! m_started) { return; }

    m_started = false;

    m_audioInput->stop();
    m_audioInput.reset();
}

void Input::audioInDataAvailable(const QByteArray &frame)
{
    const QVector<qint16> samples = convertBytesTo16BitPCM(frame);

    int length;
    QByteArray encodedBuffer = m_encoder.encode(samples, samples.size(), &length);
    m_opusBytesEncoded += length;

    for (const qint16 sample : samples)
    {
        qint16 sampleInput = sample;
        sampleInput = qAbs(sampleInput);
        if (sampleInput > m_maxSampleInput)
            m_maxSampleInput = sampleInput;
    }

    m_sampleCount += samples.size();
    if (m_sampleCount >= c_sampleCountPerEvent)
    {
        InputVolumeStreamArgs inputVolumeStreamArgs;
        qint16 maxInt = std::numeric_limits<qint16>::max();
        inputVolumeStreamArgs.PeakRaw = m_maxSampleInput / maxInt;
        inputVolumeStreamArgs.PeakDB = (float)(20 * std::log10(inputVolumeStreamArgs.PeakRaw));
        float db = qBound(minDb, inputVolumeStreamArgs.PeakDB, maxDb);
        float ratio = (db - minDb) / (maxDb - minDb);
        if (ratio < 0.30)
            ratio = 0;
        if (ratio > 1.0)
            ratio = 1;
        inputVolumeStreamArgs.PeakVU = ratio;
        emit inputVolumeStream(inputVolumeStreamArgs);
        m_sampleCount = 0;
        m_maxSampleInput = 0;
    }

    OpusDataAvailableArgs opusDataAvailableArgs = { m_audioSequenceCounter++, encodedBuffer };
    emit opusDataAvailable(opusDataAvailableArgs);
}
