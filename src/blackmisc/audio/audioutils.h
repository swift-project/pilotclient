/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
