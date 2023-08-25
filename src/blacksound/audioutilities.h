// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
    BLACKSOUND_EXPORT QVector<float> convertBytesTo32BitFloatPCM(const QByteArray &input);
    BLACKSOUND_EXPORT QVector<qint16> convertBytesTo16BitPCM(const QByteArray &input);
    BLACKSOUND_EXPORT QVector<qint16> convertFloatBytesTo16BitPCM(const QByteArray &input);
    BLACKSOUND_EXPORT QVector<float> convertFromMonoToStereo(const QVector<float> &mono);
    BLACKSOUND_EXPORT QVector<qint16> convertFromStereoToMono(const QVector<qint16> &stereo);
    BLACKSOUND_EXPORT QVector<float> convertFromShortToFloat(const QVector<qint16> &input);

    BLACKSOUND_EXPORT QAudioDeviceInfo getLowestLatencyDevice(const BlackMisc::Audio::CAudioDeviceInfo &device, QAudioFormat &format);
    BLACKSOUND_EXPORT QAudioDeviceInfo getHighestCompatibleOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device, QAudioFormat &format);

    BLACKSOUND_EXPORT QString toQString(const QAudioFormat &format);
    BLACKSOUND_EXPORT const QString &toQString(QAudioFormat::Endian e);
    BLACKSOUND_EXPORT const QString &toQString(QAudioFormat::SampleType s);
    //! @}

    //! @{
    //! Normalize audio volume to 0..100
    BLACKSOUND_EXPORT double normalize0to100(double in);
    BLACKSOUND_EXPORT qreal normalize0to100qr(double in);
    //! @}

    //! Open a input device
    BLACKSOUND_EXPORT void occupyAudioInputDevice();
} // ns

#endif // guard
