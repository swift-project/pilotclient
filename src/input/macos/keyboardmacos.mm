// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "keyboardmacos.h"
#include "macosinpututils.h"
#include "misc/logmessage.h"

#include <CoreFoundation/CoreFoundation.h>
#include <array>

using namespace swift::misc;
using namespace swift::misc::input;

namespace swift::input
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
        { kHIDUsage_KeypadHyphen, Key_NumpadMinus },
        { kHIDUsage_KeypadPlus, Key_NumpadPlus },
        { kHIDUsage_KeypadPeriod, Key_NumpadDelete },
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
        { kHIDUsage_KeyboardLeftAlt, Key_AltLeft },
        { kHIDUsage_KeyboardInsert, Key_Insert },
        { kHIDUsage_KeyboardDeleteForward, Key_Delete },
        { kHIDUsage_KeyboardHome, Key_Home },
        { kHIDUsage_KeyboardEnd, Key_End },
        { kHIDUsage_KeyboardPageUp, Key_PageUp },
        { kHIDUsage_KeyboardPageDown, Key_PageDown },
        { kHIDUsage_KeyboardCapsLock, Key_CapsLock },
        { kHIDUsage_KeyboardReturn, Key_Enter },
        { kHIDUsage_KeyboardF1, Key_Function1 },
        { kHIDUsage_KeyboardF2, Key_Function2 },
        { kHIDUsage_KeyboardF3, Key_Function3 },
        { kHIDUsage_KeyboardF4, Key_Function4 },
        { kHIDUsage_KeyboardF5, Key_Function5 },
        { kHIDUsage_KeyboardF6, Key_Function6 },
        { kHIDUsage_KeyboardF7, Key_Function7 },
        { kHIDUsage_KeyboardF8, Key_Function8 },
        { kHIDUsage_KeyboardF9, Key_Function9 },
        { kHIDUsage_KeyboardF10, Key_Function10 },
        { kHIDUsage_KeyboardF11, Key_Function11 },
        { kHIDUsage_KeyboardF12, Key_Function12 },
        { kHIDUsage_KeyboardF13, Key_Function13 },
        { kHIDUsage_KeyboardF14, Key_Function14 },
        { kHIDUsage_KeyboardF15, Key_Function15 },
        { kHIDUsage_KeyboardF16, Key_Function16 },
        { kHIDUsage_KeyboardF17, Key_Function17 },
        { kHIDUsage_KeyboardF18, Key_Function18 },
        { kHIDUsage_KeyboardF19, Key_Function19 },
        { kHIDUsage_KeyboardF20, Key_Function20 },
        { kHIDUsage_KeyboardF21, Key_Function21 },
        { kHIDUsage_KeyboardF22, Key_Function22 },
        { kHIDUsage_KeyboardF23, Key_Function23 },
        { kHIDUsage_KeyboardF24, Key_Function24 },

        /** fixme Missing ones
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
