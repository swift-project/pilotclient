#ifndef OUTPUT_H
#define OUTPUT_H

#include "blacksound/sampleprovider/sampleprovider.h"

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

struct OutputVolumeStreamArgs
{
    QAudioDeviceInfo DeviceNumber;
    float PeakRaw = 0.0;
    float PeakDB = -1 * std::numeric_limits<float>::infinity();
    float PeakVU = 0.0;
};

class AudioOutputBuffer : public QIODevice
{
    Q_OBJECT

public:
    AudioOutputBuffer(ISampleProvider *sampleProvider, QObject *parent = nullptr);

    ISampleProvider *m_sampleProvider = nullptr;

    void setAudioFormat(const QAudioFormat &format) { m_outputFormat = format; }

signals:
    void outputVolumeStream(const OutputVolumeStreamArgs &args);

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

private:
    QAudioFormat m_outputFormat;

    float m_maxSampleOutput = 0;
    int m_sampleCount = 0;
    const int c_sampleCountPerEvent = 4800;
    const float maxDb = 0;
    const float minDb = -40;
};

class Output : public QObject
{
    Q_OBJECT
public:
    Output(QObject *parent = nullptr);

    void start(const QAudioDeviceInfo &device, ISampleProvider *sampleProvider);
    void stop();

signals:
    void outputVolumeStream(const OutputVolumeStreamArgs &args);

private:
    bool m_started = false;

    QScopedPointer<QAudioOutput> m_audioOutputCom1;
    AudioOutputBuffer *m_audioOutputBuffer;
};

#endif // OUTPUT_H
