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
} // namespace swift::misc::audio

#endif // SWIFT_MISC_AUDIO_UTILS_H
