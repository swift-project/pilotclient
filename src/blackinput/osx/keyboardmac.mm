/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "keyboardmac.h"
#include <QHash>
#include <QtWidgets/QMessageBox>
#include <CoreFoundation/CoreFoundation.h>
#include <AppKit/NSEvent.h>
#include <AppKit/NSAlert.h>
#include <Foundation/NSString.h>
#include <Carbon/Carbon.h>

using namespace BlackMisc::Input;

namespace BlackInput
{

    static QHash<int, KeyCode> keyMapping
    {
        { kVK_ANSI_0, Key_0 },
        { kVK_ANSI_1, Key_1 },
        { kVK_ANSI_2, Key_2 },
        { kVK_ANSI_3, Key_3 },
        { kVK_ANSI_4, Key_4 },
        { kVK_ANSI_5, Key_5 },
        { kVK_ANSI_6, Key_6 },
        { kVK_ANSI_7, Key_7 },
        { kVK_ANSI_8, Key_8 },
        { kVK_ANSI_9, Key_9 },
        { kVK_ANSI_A, Key_A },
        { kVK_ANSI_B, Key_B },
        { kVK_ANSI_C, Key_C },
        { kVK_ANSI_D, Key_D },
        { kVK_ANSI_E, Key_E },
        { kVK_ANSI_F, Key_F },
        { kVK_ANSI_G, Key_G },
        { kVK_ANSI_H, Key_H },
        { kVK_ANSI_I, Key_I },
        { kVK_ANSI_J, Key_J },
        { kVK_ANSI_K, Key_K },
        { kVK_ANSI_L, Key_L },
        { kVK_ANSI_M, Key_M },
        { kVK_ANSI_N, Key_N },
        { kVK_ANSI_O, Key_O },
        { kVK_ANSI_P, Key_P },
        { kVK_ANSI_Q, Key_Q },
        { kVK_ANSI_R, Key_R },
        { kVK_ANSI_S, Key_S },
        { kVK_ANSI_T, Key_T },
        { kVK_ANSI_U, Key_U },
        { kVK_ANSI_V, Key_V },
        { kVK_ANSI_W, Key_W },
        { kVK_ANSI_X, Key_X },
        { kVK_ANSI_Y, Key_Y },
        { kVK_ANSI_Z, Key_Z },
    };

    CKeyboardMac::CKeyboardMac(QObject *parent) :
        IKeyboard(parent)
    {
    }

    CKeyboardMac::~CKeyboardMac()
    {
    }

    bool CKeyboardMac::init()
    {
        bool accessibilityEnabled = false;
        if (AXIsProcessTrustedWithOptions != NULL)
        {
            // 10.9 and later
            const void *keys[] = { kAXTrustedCheckOptionPrompt };
            const void *values[] = { kCFBooleanTrue };

            CFDictionaryRef options = CFDictionaryCreate(
                                          kCFAllocatorDefault,
                                          keys,
                                          values,
                                          sizeof(keys) / sizeof(*keys),
                                          &kCFCopyStringDictionaryKeyCallBacks,
                                          &kCFTypeDictionaryValueCallBacks);

            accessibilityEnabled = AXIsProcessTrustedWithOptions(options);
        }
        else
        {
            // 10.8 and older
            accessibilityEnabled = AXAPIEnabled();
        }

        if (!accessibilityEnabled)
        {
            QMessageBox msgBox;
            msgBox.setText("In order to enable hotkeys first add Swift to the list of apps allowed to "
                           "control your computer in System Preferences / Security & Privacy / Privacy / Accessiblity and then restart Swift.");
            msgBox.exec();

            return false;
        }


        CGEventMask eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp) | (1 << kCGEventFlagsChanged));

        // try creating an event tap just for keypresses. if it fails, we need Universal Access.
        CFMachPortRef eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                 eventMask, myCGEventCallback, this);

        CFRunLoopSourceRef source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault,
                                    eventTap, 0);

        CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
        CGEventTapEnable(eventTap, true);

        return true;
    }

    void CKeyboardMac::processKeyEvent(CGEventType type,
                                       CGEventRef event)
    {
        BlackMisc::Input::CHotkeyCombination oldCombination(m_keyCombination);

        unsigned int vkcode = static_cast<unsigned int>(CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode));

        if (type == kCGEventKeyDown)
        {
            auto key = convertToKey(vkcode);
            if (key == Key_Unknown) { return; }
            m_keyCombination.addKeyboardKey(key);
        }
        else if (type == kCGEventKeyUp)
        {
            auto key = convertToKey(vkcode);
            if (key == Key_Unknown) { return; }
            m_keyCombination.removeKeyboardKey(key);
        }
        else if (type == kCGEventFlagsChanged)
        {
            CGEventFlags f = CGEventGetFlags(event);

            if ((f & kCGEventFlagMaskShift) == kCGEventFlagMaskShift)
            {
                if (vkcode == 56)
                {
                    m_keyCombination.addKeyboardKey(Key_ShiftLeft);
                }
                else if (vkcode == 60)
                {
                    m_keyCombination.addKeyboardKey(Key_ShiftRight);
                }
            }
            else
            {
                m_keyCombination.removeKeyboardKey(Key_ShiftLeft);
                m_keyCombination.removeKeyboardKey(Key_ShiftRight);
            }

            if ((f & kCGEventFlagMaskControl) == kCGEventFlagMaskControl)
            {
                // at least on the mac wireless keyboard there is no right ctrl key
                if (vkcode == 59)
                {
                    m_keyCombination.addKeyboardKey(Key_ControlLeft);
                }
            }
            else
            {
                m_keyCombination.removeKeyboardKey(Key_ControlLeft);
            }

            if ((f & kCGEventFlagMaskAlternate) == kCGEventFlagMaskAlternate)
            {
                if (vkcode == 58)
                {
                    m_keyCombination.addKeyboardKey(Key_AltLeft);
                }
                else if (vkcode == 61)
                {
                    m_keyCombination.addKeyboardKey(Key_AltRight);
                }
            }
            else
            {
                m_keyCombination.removeKeyboardKey(Key_AltLeft);
                m_keyCombination.removeKeyboardKey(Key_AltRight);
            }
        }

        if (oldCombination != m_keyCombination)
        {
            emit keyCombinationChanged(m_keyCombination);
        }
    }

    KeyCode CKeyboardMac::convertToKey(int keyCode)
    {
        return keyMapping.value(keyCode, Key_Unknown);
    }

    CGEventRef CKeyboardMac::myCGEventCallback(CGEventTapProxy,
            CGEventType type,
            CGEventRef event,
            void *refcon)
    {

        CKeyboardMac *keyboardMac = static_cast<CKeyboardMac*>(refcon);
        keyboardMac->processKeyEvent(type, event);

        // send event to next application
        return event;
    }
}
