// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_INPUT_MACOSINPUTUTILS_H
#define SWIFT_INPUT_MACOSINPUTUTILS_H

//! \file

#include <CoreFoundation/CoreFoundation.h>

namespace swift::input
{
    //! Common MacOS input utilities
    class CMacOSInputUtils
    {
    public:
        CMacOSInputUtils() = delete;

        //! Check OS permission for input monitoring access
        static bool hasAccess();

        //! Request OS permission for input monitoring access
        static bool requestAccess();

        //! Creates a new device matching dict using usagePage and usage
        static CFMutableDictionaryRef createDeviceMatchingDictionary(UInt32 usagePage, UInt32 usage);
    };
} // namespace swift::input

#endif
