#include "macosinpututils.h"

#include <IOKit/hid/IOHIDKeys.h>

namespace BlackInput
{
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
