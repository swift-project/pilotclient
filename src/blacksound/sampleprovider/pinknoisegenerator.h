#ifndef PINKNOISEGENERATOR_H
#define PINKNOISEGENERATOR_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QRandomGenerator>
#include <QVector>

#include <array>

class BLACKSOUND_EXPORT PinkNoiseGenerator : public ISampleProvider
{
    Q_OBJECT

public:
    PinkNoiseGenerator(QObject *parent = nullptr) : ISampleProvider(parent) {}

    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

    void setGain(double gain) { m_gain = gain; }

private:
    QRandomGenerator random;
    std::array<double, 7> pinkNoiseBuffer = {0};
    double m_gain = 0.0;
};

#endif // PINKNOISEGENERATOR_H
