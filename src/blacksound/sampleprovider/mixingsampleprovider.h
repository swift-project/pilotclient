#ifndef MIXINGSAMPLEPROVIDER_H
#define MIXINGSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include <QSharedPointer>
#include <QVector>

class BLACKSOUND_EXPORT MixingSampleProvider : public ISampleProvider
{
public:
    MixingSampleProvider(QObject * parent = nullptr) : ISampleProvider(parent) {}

    void addMixerInput(ISampleProvider *provider) { m_sources.append(provider); }
    virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

private:
    QVector<ISampleProvider*> m_sources;
};

#endif // MIXINGSAMPLEPROVIDER_H
