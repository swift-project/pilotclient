/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_AUDIOUTILITIES_H
#define BLACKSOUND_AUDIOUTILITIES_H

#include "blacksound/blacksoundexport.h"
#include "blackmisc/audio/audiodeviceinfo.h"

#include <QAudioDeviceInfo>
#include <QByteArray>
#include <QVector>

namespace BlackSound
{
    //! @{
    //! Conversion functions
    BLACKSOUND_EXPORT QVector<float>  convertBytesTo32BitFloatPCM(const QByteArray &input);
    BLACKSOUND_EXPORT QVector<qint16> convertBytesTo16BitPCM(const QByteArray &input);
    BLACKSOUND_EXPORT QVector<qint16> convertFloatBytesTo16BitPCM(const QByteArray &input);
    BLACKSOUND_EXPORT QVector<float>  convertFromMonoToStereo(const QVector<float> &mono);
    BLACKSOUND_EXPORT QVector<qint16> convertFromStereoToMono(const QVector<qint16> &stereo);
    BLACKSOUND_EXPORT QVector<float>  convertFromShortToFloat(const QVector<qint16> &input);

    BLACKSOUND_EXPORT QAudioDeviceInfo getLowestLatencyDevice(const BlackMisc::Audio::CAudioDeviceInfo &device, QAudioFormat &format);
    BLACKSOUND_EXPORT QAudioDeviceInfo getHighestCompatibleOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device, QAudioFormat &format);

    BLACKSOUND_EXPORT QString toQString(const QAudioFormat &format);
    BLACKSOUND_EXPORT const QString &toQString(QAudioFormat::Endian e);
    BLACKSOUND_EXPORT const QString &toQString(QAudioFormat::SampleType s);
    //! @}

    //! @{
    //! Normalize audio volume to 0..100
    BLACKSOUND_EXPORT double normalize0to100(double in);
    BLACKSOUND_EXPORT qreal  normalize0to100qr(double in);
    //! @}

    //! Open a input device
    BLACKSOUND_EXPORT void occupyAudioInputDevice();
} // ns

#endif // guard
