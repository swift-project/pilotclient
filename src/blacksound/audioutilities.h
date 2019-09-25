#ifndef AUDIOUTILITIES_H
#define AUDIOUTILITIES_H

#include "blacksound/blacksoundexport.h"
#include <QByteArray>
#include <QVector>

BLACKSOUND_EXPORT QVector<qint16> convertBytesTo16BitPCM(const QByteArray input);
BLACKSOUND_EXPORT QVector<qint16> convertFloatBytesTo16BitPCM(const QByteArray input);
BLACKSOUND_EXPORT QVector<qint16> convertFromMonoToStereo(const QVector<qint16> &mono);
BLACKSOUND_EXPORT QVector<qint16> convertFromStereoToMono(const QVector<qint16> &stereo);

#endif // guard
