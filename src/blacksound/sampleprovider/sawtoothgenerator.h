#ifndef SAWTOOTHGENERATOR_H
#define SAWTOOTHGENERATOR_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QRandomGenerator>
#include <QVector>

#include <array>

class BLACKSOUND_EXPORT SawToothGenerator : public ISampleProvider
{
    Q_OBJECT

public:
    SawToothGenerator(double frequency, QObject *parent = nullptr);

    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

    void setGain(double gain) { m_gain = gain; }

private:
    double m_gain = 0.0;
    double m_frequency = 0.0;
    double m_sampleRate = 48000;
    int m_nSample = 0;
};

#endif // SAWTOOTHGENERATOR_H
