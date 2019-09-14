#ifndef SAMPLEPROVIDER_H
#define SAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include <QObject>
#include <QVector>

class BLACKSOUND_EXPORT ISampleProvider : public QObject
{
    Q_OBJECT

public:
    ISampleProvider(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ISampleProvider() {}

    virtual int readSamples(QVector<qint16> &samples, qint64 count) = 0;

    virtual bool isFinished() { return false; }
};

#endif // SAMPLEPROVIDER_H
