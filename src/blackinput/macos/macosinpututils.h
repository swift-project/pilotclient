// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKINPUT_MACOSINPUTUTILS_H
#define BLACKINPUT_MACOSINPUTUTILS_H

//! \file

#include <CoreFoundation/CoreFoundation.h>

namespace BlackInput
{
    //! Common MacOS input utilities
    class CMacOSInputUtils
    {
    public:
        CMacOSInputUtils() = delete;

        //! Request OS permission for input monitoring access
        static bool requestAccess();

        //! Creates a new device matching dict using usagePage and usage
        static CFMutableDictionaryRef createDeviceMatchingDictionary(UInt32 usagePage, UInt32 usage);
    };
}

#endif
