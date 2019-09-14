#ifndef SOUNDCARDSAMPLEPROVIDER_H
#define SOUNDCARDSAMPLEPROVIDER_H

#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/sampleprovider/mixingsampleprovider.h"
#include "receiversampleprovider.h"

#include <QAudioFormat>

class SoundcardSampleProvider : public ISampleProvider
{
    Q_OBJECT

public:
    SoundcardSampleProvider(int sampleRate, const QVector<quint16> &transceiverIDs, QObject *parent = nullptr);

    QAudioFormat waveFormat() const;

    void setBypassEffects(bool value);
    void pttUpdate(bool active, const QVector<TxTransceiverDto> &txTransceivers);
    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;
    void addOpusSamples(const IAudioDto &audioDto, const QVector<RxTransceiverDto> &rxTransceivers);
    void updateRadioTransceivers(const QVector<TransceiverDto> &radioTransceivers);

signals:
    void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

private:
    QAudioFormat m_waveFormat;
    MixingSampleProvider *m_mixer;
    QVector<ReceiverSampleProvider *> m_receiverInputs;
    QVector<quint16> m_receiverIDs;
};

#endif // SOUNDCARDSAMPLEPROVIDER_H
