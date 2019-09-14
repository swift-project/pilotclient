#ifndef RESOURCESOUND_H
#define RESOURCESOUND_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/wav/wavfile.h"

#include <QString>
#include <QVector>

class ResourceSound
{
public:
    ResourceSound(const QString &audioFileName);

    const QVector<qint16> &audioData();

private:
    WavFile *m_wavFile;
    QVector<qint16> m_samples;
};

#endif // RESOURCESOUND_H
