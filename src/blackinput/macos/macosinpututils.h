/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
