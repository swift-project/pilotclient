#ifndef RECEIVERSAMPLEPROVIDER_H
#define RECEIVERSAMPLEPROVIDER_H

#include "callsignsampleprovider.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/sampleprovider/mixingsampleprovider.h"

#include <QtGlobal>

struct TransceiverReceivingCallsignsChangedArgs
{
    quint16 transceiverID;
    QStringList receivingCallsigns;
};

class ReceiverSampleProvider : public ISampleProvider
{
    Q_OBJECT

public:
    ReceiverSampleProvider(const QAudioFormat &audioFormat, quint16 id, int voiceInputNumber, QObject *parent = nullptr);

    void setBypassEffects(bool value);
    void setFrequency(const uint &frequency);
    int activeCallsigns() const;
    float volume() const;

    bool getMute() const;
    void setMute(bool value);

    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

    void addOpusSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);
    void addSilentSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);

    quint16 getId() const;

signals:
    void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

private:
    uint m_frequency = 122800;
    bool m_mute = false;

    const float m_clickGain = 1.0f;
    const double m_blockToneGain = 0.10f;

    quint16 m_id;

    // TODO VolumeSampleProvider volume;
    MixingSampleProvider *m_mixer;
    // TODO SignalGenerator blockTone;
    QVector<CallsignSampleProvider *> m_voiceInputs;

    bool m_doClickWhenAppropriate = false;
    int lastNumberOfInUseInputs = 0;
};

#endif // RECEIVERSAMPLEPROVIDER_H
