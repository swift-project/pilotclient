// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_AUDIOUTILITIES_H
#define SWIFT_SOUND_AUDIOUTILITIES_H

#include "sound/swiftsoundexport.h"
#include "misc/audio/audiodeviceinfo.h"

#include <QAudioDevice>
#include <QByteArray>
#include <QVector>

namespace swift::sound
{
    //! @{
    //! Conversion functions
    SWIFT_SOUND_EXPORT QVector<float> convertBytesTo32BitFloatPCM(const QByteArray &input);
    SWIFT_SOUND_EXPORT QVector<qint16> convertBytesTo16BitPCM(const QByteArray &input);
    SWIFT_SOUND_EXPORT QVector<float> convertFromMonoToStereo(const QVector<float> &mono);
    SWIFT_SOUND_EXPORT QVector<qint16> convertFromStereoToMono(const QVector<qint16> &stereo);
    SWIFT_SOUND_EXPORT QVector<float> convertFromShortToFloat(const QVector<qint16> &input);

    SWIFT_SOUND_EXPORT QAudioDevice getLowestLatencyDevice(const swift::misc::audio::CAudioDeviceInfo &device, QAudioFormat &format);
    SWIFT_SOUND_EXPORT QAudioDevice getHighestCompatibleOutputDevice(const swift::misc::audio::CAudioDeviceInfo &device, QAudioFormat &format);

    SWIFT_SOUND_EXPORT QString toQString(const QAudioFormat &format);
    SWIFT_SOUND_EXPORT const QString &toQString(QSysInfo::Endian e);
    SWIFT_SOUND_EXPORT const QString &toQString(QAudioFormat::SampleFormat s);
    //! @}

    //! @{
    //! Normalize audio volume to 0..100
    SWIFT_SOUND_EXPORT double normalize0to100(double in);
    SWIFT_SOUND_EXPORT qreal normalize0to100qr(double in);
    //! @}

    //! Open a input device
    SWIFT_SOUND_EXPORT void occupyAudioInputDevice();
} // ns

#endif
