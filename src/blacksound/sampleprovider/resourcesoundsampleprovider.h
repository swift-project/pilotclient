#ifndef RESOURCESOUNDSAMPLEPROVIDER_H
#define RESOURCESOUNDSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "sampleprovider.h"
#include "resourcesound.h"

class BLACKSOUND_EXPORT ResourceSoundSampleProvider : public ISampleProvider
{
    Q_OBJECT

public:
    ResourceSoundSampleProvider(const ResourceSound &resourceSound, QObject *parent = nullptr);

    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;
    virtual bool isFinished() override;

    bool looping() const;
    void setLooping(bool looping);

    float gain() const;
    void setGain(float gain);

private:
    float m_gain = 1.0f;
    bool m_looping = false;

    ResourceSound m_resourceSound;
    qint64 position = 0;
    const int tempBufferSize = 9600; //9600 = 200ms
    QVector<qint16> tempBuffer;
    bool m_isFinished = false;
};

#endif // RESOURCESOUNDSAMPLEPROVIDER_H
