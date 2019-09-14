#ifndef SIMPLECOMPRESSOREFFECT_H
#define SIMPLECOMPRESSOREFFECT_H

#include "blacksound/blacksoundexport.h"
#include "sampleprovider.h"
#include "blacksound/dsp/SimpleComp.h"

#include <QObject>
#include <QTimer>

class BLACKSOUND_EXPORT SimpleCompressorEffect : public ISampleProvider
{
    Q_OBJECT

public:
    SimpleCompressorEffect(ISampleProvider *source, QObject *parent = nullptr);

    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

    void setEnabled(bool enabled);
    void setMakeUpGain(double gain);

private:

    QTimer m_timer;
    ISampleProvider *m_sourceStream;
    bool m_enabled = true;
    chunkware_simple::SimpleComp m_simpleCompressor;
    const int channels = 1;
};

#endif // SIMPLECOMPRESSOREFFECT_H
