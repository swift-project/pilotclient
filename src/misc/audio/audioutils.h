// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AUDIO_UTILS_H
#define SWIFT_MISC_AUDIO_UTILS_H

#include "misc/swiftmiscexport.h"

namespace swift::misc::audio
{
    //! Start the Windows mixer
    SWIFT_MISC_EXPORT bool startWindowsMixer();

    //! On windows init the audio devices
    //! \remarks workaround for WASAPI driver crashes, hoping that upfront init already sets global vars. etc.
    SWIFT_MISC_EXPORT void initWindowsAudioDevices();
} // namespace swift::misc::audio

#endif // guard
