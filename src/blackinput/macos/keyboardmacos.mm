/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "keyboardmacos.h"
#include "macosinpututils.h"
#include "blackmisc/logmessage.h"

#include <CoreFoundation/CoreFoundation.h>
#include <array>

using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace BlackInput
{
    // https://developer.apple.com/documentation/iokit/1592915-anonymous?language=objc
    static QHash<quint32, KeyCode> keyMapping
    {
        { kHIDUsage_Keyboard0, Key_0 },
        { kHIDUsage_Keyboard1, Key_1 },
        { kHIDUsage_Keyboard2, Key_2 },
        { kHIDUsage_Keyboard3, Key_3 },
        { kHIDUsage_Keyboard4, Key_4 },
        { kHIDUsage_Keyboard5, Key_5 },
        { kHIDUsage_Keyboard6, Key_6 },
        { kHIDUsage_Keyboard7, Key_7 },
        { kHIDUsage_Keyboard8, Key_8 },
        { kHIDUsage_Keyboard9, Key_9 },
        { kHIDUsage_KeyboardA, Key_A },
        { kHIDUsage_KeyboardB, Key_B },
        { kHIDUsage_KeyboardC, Key_C },
        { kHIDUsage_KeyboardD, Key_D },
        { kHIDUsage_KeyboardE, Key_E },
        { kHIDUsage_KeyboardF, Key_F },
        { kHIDUsage_KeyboardG, Key_G },
        { kHIDUsage_KeyboardH, Key_H },
        { kHIDUsage_KeyboardI, Key_I },
        { kHIDUsage_KeyboardJ, Key_J },
        { kHIDUsage_KeyboardK, Key_K },
        { kHIDUsage_KeyboardL, Key_L },
        { kHIDUsage_KeyboardM, Key_M },
        { kHIDUsage_KeyboardN, Key_N },
        { kHIDUsage_KeyboardO, Key_O },
        { kHIDUsage_KeyboardP, Key_P },
        { kHIDUsage_KeyboardQ, Key_Q },
        { kHIDUsage_KeyboardR, Key_R },
        { kHIDUsage_KeyboardS, Key_S },
        { kHIDUsage_KeyboardT, Key_T },
        { kHIDUsage_KeyboardU, Key_U },
        { kHIDUsage_KeyboardV, Key_V },
        { kHIDUsage_KeyboardW, Key_W },
        { kHIDUsage_KeyboardX, Key_X },
        { kHIDUsage_KeyboardY, Key_Y },
        { kHIDUsage_KeyboardZ, Key_Z },
        { kHIDUsage_KeypadPlus, Key_NumpadPlus },
        { kHIDUsage_KeypadHyphen, Key_NumpadMinus },
        { kHIDUsage_KeyboardHyphen, Key_Minus },
        { kHIDUsage_KeyboardPeriod, Key_Period },
        { kHIDUsage_KeypadAsterisk, Key_Multiply },
        { kHIDUsage_KeypadSlash, Key_Divide },
        { kHIDUsage_KeyboardDeleteOrBackspace, Key_Back },
        { kHIDUsage_KeyboardTab, Key_Tab },
        { kHIDUsage_KeyboardEscape, Key_Esc },
        { kHIDUsage_KeyboardSpacebar, Key_Space },
        { kHIDUsage_KeyboardGraveAccentAndTilde, Key_DeadGrave },
        { kHIDUsage_KeyboardComma, Key_Comma },
        { kHIDUsage_Keypad0, Key_Numpad0 },
        { kHIDUsage_Keypad1, Key_Numpad1 },
        { kHIDUsage_Keypad2, Key_Numpad2 },
        { kHIDUsage_Keypad3, Key_Numpad3 },
        { kHIDUsage_Keypad4, Key_Numpad4 },
        { kHIDUsage_Keypad5, Key_Numpad5 },
        { kHIDUsage_Keypad6, Key_Numpad6 },
        { kHIDUsage_Keypad7, Key_Numpad7 },
        { kHIDUsage_Keypad8, Key_Numpad8 },
        { kHIDUsage_Keypad9, Key_Numpad9 },
        { kHIDUsage_KeyboardRightControl, Key_ControlRight },
        { kHIDUsage_KeyboardLeftControl, Key_ControlLeft },
        { kHIDUsage_KeyboardRightAlt, Key_AltRight },
        { kHIDUsage_KeyboardLeftAlt, Key_AltLeft }

        /** fixme Missing ones
        Key_Insert,
        Key_Delete,
        CKeyboardKey(Key_NumpadEqual),
        CKeyboardKey(Key_OEM1),
        CKeyboardKey(Key_OEM2),
        CKeyboardKey(Key_OEM3),
        CKeyboardKey(Key_OEM4),
        CKeyboardKey(Key_OEM5),
        CKeyboardKey(Key_OEM6),
        CKeyboardKey(Key_OEM7),
        CKeyboardKey(Key_OEM8),
        CKeyboardKey(Key_OEM102)
        **/
    };

    CKeyboardMacOS::CKeyboardMacOS(QObject *parent) : IKeyboard(parent)
    { }

    void CKeyboardMacOS::processKeyEvent(IOHIDValueRef value)
    {
        IOHIDElementRef element = IOHIDValueGetElement(value);
        UInt32 usagePage = IOHIDElementGetUsagePage(element);
        if (usagePage != kHIDPage_KeyboardOrKeypad) { return; }

        quint32 scancode = IOHIDElementGetUsage(element);

        KeyCode code = convertToKey(scancode);
        if (code != Key_Unknown)
        {
            CHotkeyCombination oldCombination(m_keyCombination);
            bool pressed = IOHIDValueGetIntegerValue(value) == 1;
            if (pressed)
            {
                m_keyCombination.addKeyboardKey(code);
            }
            else
            {
                m_keyCombination.removeKeyboardKey(code);
            }

            if (oldCombination != m_keyCombination)
            {
                emit keyCombinationChanged(m_keyCombination);
            }
        }
    }

    CKeyboardMacOS::~CKeyboardMacOS()
    {
        if (m_hidManager)
        {
            IOHIDManagerClose(m_hidManager, kIOHIDOptionsTypeNone);
            CFRelease(m_hidManager);
        }
    }

    bool CKeyboardMacOS::init()
    {
        if (!CMacOSInputUtils::requestAccess())
        {
            CLogMessage(this).error(u"Access denied for keyboard input monitoring");
        }
        m_hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

        CFMutableArrayRef matchingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        if (!matchingArray)
        {
            CLogMessage(this).warning(u"Cocoa: Failed to create array");
            return false;
        }

        CFDictionaryRef matchingDict = CMacOSInputUtils::createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
        if (matchingDict)
        {
            CFArrayAppendValue(matchingArray, matchingDict);
            CFRelease(matchingDict);
        }

        matchingDict = CMacOSInputUtils::createDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keypad);
        if (matchingDict)
        {
            CFArrayAppendValue(matchingArray, matchingDict);
            CFRelease(matchingDict);
        }

        IOHIDManagerSetDeviceMatchingMultiple(m_hidManager, matchingArray);
        CFRelease(matchingArray);

        IOHIDManagerRegisterInputValueCallback(m_hidManager, valueCallback, this);
        IOHIDManagerScheduleWithRunLoop(m_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        const auto result = IOHIDManagerOpen(m_hidManager, kIOHIDOptionsTypeNone);

        if (result == kIOReturnSuccess)
        {
            CLogMessage(this).debug(u"Initialized");
        }
        else
        {
            CLogMessage(this).error(u"Failed to open HID manager for keyboard monitoring");
        }
        return true;
    }

    void CKeyboardMacOS::valueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value)
    {
        Q_UNUSED(result);
        Q_UNUSED(sender);
        CKeyboardMacOS *obj = static_cast<CKeyboardMacOS *>(context);
        obj->processKeyEvent(value);
    }

    KeyCode CKeyboardMacOS::convertToKey(quint32 keyCode)
    {
        return keyMapping.value(keyCode, Key_Unknown);
    }
}
