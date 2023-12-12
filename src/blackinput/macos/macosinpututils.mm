// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "macosinpututils.h"

#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hidsystem/IOHIDLib.h>

namespace BlackInput
{

    bool CMacOSInputUtils::hasAccess()
    {
        if (@available(macOS 10.15, *))
        {
            return IOHIDCheckAccess(kIOHIDRequestTypeListenEvent) == IOHIDAccessType::kIOHIDAccessTypeGranted;
        }
        else
        {
            return true;
        }
    }

    bool CMacOSInputUtils::requestAccess()
    {
        if (@available(macOS 10.15, *))
        {
            return IOHIDRequestAccess(kIOHIDRequestTypeListenEvent);
        }
        else
        {
            return true;
        }
    }

    CFMutableDictionaryRef CMacOSInputUtils::createDeviceMatchingDictionary(UInt32 usagePage, UInt32 usage)
    {
        CFMutableDictionaryRef result = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                  &kCFTypeDictionaryKeyCallBacks,
                                                                  &kCFTypeDictionaryValueCallBacks);

        if (result)
        {
            if (usagePage)
            {
                CFNumberRef pageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);
                if (pageCFNumberRef)
                {
                    CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
                    CFRelease(pageCFNumberRef);

                    if (usage)
                    {
                        CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
                        if (usageCFNumberRef)
                        {
                            CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);
                            CFRelease(usageCFNumberRef);
                        }
                    }
                }
            }
        }
        return result;
    }
}
