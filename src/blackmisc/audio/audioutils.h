// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AUDIO_UTILS_H
#define BLACKMISC_AUDIO_UTILS_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Audio
{
    //! Start the Windows mixer
    BLACKMISC_EXPORT bool startWindowsMixer();

    //! On windows init the audio devices
    //! \remarks workaround for WSAPI driver crashes, hoping that upfront init already sets global vars. etc.
    BLACKMISC_EXPORT void initWindowsAudioDevices();
}

#endif // guard
