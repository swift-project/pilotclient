/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "keyboard_mac.h"
#include "keymapping_mac.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/NSEvent.h>
#include <AppKit/NSAlert.h>
#include <Foundation/NSString.h>

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    CKeyboardMac::CKeyboardMac(QObject *parent) :
        IKeyboard(parent),
        m_mode(Mode_Nominal)
    {
    }

    CKeyboardMac::~CKeyboardMac()
    {
    }

    bool CKeyboardMac::init()
    {
        bool accessibilityEnabled = false;
        if (AXIsProcessTrustedWithOptions != NULL) {
            // 10.9 and later
            const void * keys[] = { kAXTrustedCheckOptionPrompt };
            const void * values[] = { kCFBooleanTrue };

            CFDictionaryRef options = CFDictionaryCreate(
                    kCFAllocatorDefault,
                    keys,
                    values,
                    sizeof(keys) / sizeof(*keys),
                    &kCFCopyStringDictionaryKeyCallBacks,
                    &kCFTypeDictionaryValueCallBacks);

            accessibilityEnabled = AXIsProcessTrustedWithOptions(options);
        }
        else {
            // 10.8 and older
            accessibilityEnabled = AXAPIEnabled();
        }

        if (!accessibilityEnabled) {
            QMessageBox msgBox;
            msgBox.setText("In order to enable hotkeys first add Swift to the list of apps allowed to "
               "control your computer in System Preferences / Security & Privacy / Privacy / Accessiblity and then restart Swift.");
            msgBox.exec();

            return false;
        }


        CGEventMask eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp) | (1 <<kCGEventFlagsChanged));

        // try creating an event tap just for keypresses. if it fails, we need Universal Access.
        CFMachPortRef eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, 0,
                                                  eventMask, myCGEventCallback, NULL);

        CFRunLoopSourceRef source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault,
                                                                  eventTap, 0);

        CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
        CGEventTapEnable(eventTap, true);

        return true;
    }

    void CKeyboardMac::setKeysToMonitor(const CKeyboardKeyList &keylist)
    {
        m_listMonitoredKeys = keylist;
    }

    void CKeyboardMac::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    void CKeyboardMac::triggerKey(const CKeyboardKey &key, bool isPressed)
    {
        if(!isPressed) emit keyUp(key);
        else emit keyDown(key);
    }

    void CKeyboardMac::processKeyEvent(CGEventType type,
                                           CGEventRef event)
    {
        BlackMisc::Hardware::CKeyboardKey lastPressedKey = m_pressedKey;
        if (m_ignoreNextKey)
        {
            m_ignoreNextKey = false;
            return;
        }

        unsigned int vkcode = static_cast<unsigned int>(CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode));

        bool isFinished = false;
        if (type == kCGEventKeyDown) {
            m_pressedKey.setKey(CKeyMappingMac::convertToKey(vkcode));
        } else if (type == kCGEventKeyUp) {
            m_pressedKey.setKey(Qt::Key_unknown);
            isFinished = true;
        } else if (type == kCGEventFlagsChanged) {

            // m_pressedKey.removeAllModifiers();

            CGEventFlags f = CGEventGetFlags(event);
            qDebug() << "hier";

            if ((f & kCGEventFlagMaskShift) == kCGEventFlagMaskShift) {
                qDebug() << "shift";
                if (vkcode == 56) {
                    m_pressedKey.addModifier(CKeyboardKey::ModifierShiftLeft);
                } else if (vkcode == 60) {
                    m_pressedKey.addModifier(CKeyboardKey::ModifierShiftRight);
                }
            } else {
                m_pressedKey.removeModifier(CKeyboardKey::ModifierShiftLeft);
                m_pressedKey.removeModifier(CKeyboardKey::ModifierShiftRight);
            }

            if ((f & kCGEventFlagMaskControl) == kCGEventFlagMaskControl) {
                qDebug() << "ctrl";
                // at least on the mac wireless keyboard there is no right ctrl key
                if (vkcode == 59) {
                    m_pressedKey.addModifier(CKeyboardKey::ModifierCtrlLeft);
                }
            } else {
                m_pressedKey.removeModifier(CKeyboardKey::ModifierCtrlLeft);
            }

            if ((f & kCGEventFlagMaskAlternate) == kCGEventFlagMaskAlternate) {
                qDebug() << "alt";
                if (vkcode == 58) {
                    m_pressedKey.addModifier(CKeyboardKey::ModifierAltLeft);
                } else if (vkcode == 61) {
                    m_pressedKey.addModifier(CKeyboardKey::ModifierAltRight);
                }
            } else {
                m_pressedKey.removeModifier(CKeyboardKey::ModifierAltLeft);
                m_pressedKey.removeModifier(CKeyboardKey::ModifierAltRight);
            }
        }

        if (lastPressedKey == m_pressedKey)
            return;

        if (m_mode == Mode_Capture)
        {
            if (isFinished)
            {
                sendCaptureNotification(lastPressedKey, true);
                m_mode = Mode_Nominal;
            }
            else
            {
                sendCaptureNotification(m_pressedKey, false);
            }
        }
        else
        {
            if (m_listMonitoredKeys.contains(lastPressedKey)) emit keyUp(lastPressedKey);
            if (m_listMonitoredKeys.contains(m_pressedKey)) emit keyDown(m_pressedKey);
        }
    }

    void CKeyboardMac::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }

    CGEventRef CKeyboardMac::myCGEventCallback(CGEventTapProxy,
                                 CGEventType type,
                                 CGEventRef event,
                                 void *)
    {

        CKeyboardMac *keyboardMac = qobject_cast<CKeyboardMac*>(IKeyboard::getInstance());
        keyboardMac->processKeyEvent(type, event);

       // send event to next application
       return event;
    }
}
