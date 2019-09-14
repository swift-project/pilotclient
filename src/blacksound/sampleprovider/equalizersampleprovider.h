#ifndef EQUALIZERSAMPLEPROVIDER_H
#define EQUALIZERSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/dsp/biquadfilter.h"

#include <QSharedPointer>
#include <QVector>

enum EqualizerPresets
{
    VHFEmulation = 1
};

class BLACKSOUND_EXPORT EqualizerSampleProvider : public ISampleProvider
{
    Q_OBJECT

public:
    EqualizerSampleProvider(ISampleProvider *sourceProvider, EqualizerPresets preset, QObject *parent = nullptr);

    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

    void setBypassEffects(bool value) { m_bypass = value; }

    double outputGain() const;
    void setOutputGain(double outputGain);

private:
    void setupPreset(EqualizerPresets preset);

    ISampleProvider *m_sourceProvider;

    int m_channels = 1;
    bool m_bypass = false;
    double m_outputGain = 1.0;
    QVector<BiQuadFilter> m_filters;
};

#endif // EQUALIZERSAMPLEPROVIDER_H
