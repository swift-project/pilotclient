/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard_windows.h"
#include "keymapping_windows.h"
#include <QDebug>

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    CKeyboardWindows::CKeyboardWindows(QObject *parent) :
        IKeyboard(parent),
        m_keyboardHook(nullptr),
        m_mode(Mode_Nominal)
    {
    }

    CKeyboardWindows::~CKeyboardWindows()
    {
        if (m_keyboardHook)
            UnhookWindowsHookEx(m_keyboardHook);
    }

    bool CKeyboardWindows::init()
    {
        m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, CKeyboardWindows::keyboardProc, GetModuleHandle(NULL), 0);
        return true;
    }

    void CKeyboardWindows::setKeysToMonitor(const CKeyboardKeyList &keylist)
    {
        m_listMonitoredKeys = keylist;
    }

    void CKeyboardWindows::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    void CKeyboardWindows::triggerKey(const CKeyboardKey key, bool isPressed)
    {
        if(!isPressed) emit keyUp(key);
        else emit keyDown(key);
    }

    void CKeyboardWindows::processKeyEvent(WPARAM vkcode, uint event)
    {
        BlackMisc::Hardware::CKeyboardKey lastPressedKey = m_pressedKey;
        if (m_ignoreNextKey)
        {
            m_ignoreNextKey = false;
            return;
        }

        bool isFinished = false;
        if ((event == WM_KEYDOWN) || (event == WM_SYSKEYDOWN))
        {
            if (CKeyMappingWindows::isModifier(vkcode))
                m_pressedKey.addModifier(CKeyMappingWindows::convertToModifier(vkcode));
            else
            {
                m_pressedKey.setKey(CKeyMappingWindows::convertToKey(vkcode));
            }
        }
        else if ((event == WM_KEYUP) || (event == WM_SYSKEYUP) )
        {
            if (CKeyMappingWindows::isModifier(vkcode))
                m_pressedKey.removeModifier(CKeyMappingWindows::convertToModifier(vkcode));
            else
            {
                m_pressedKey.setKey(Qt::Key_unknown);
            }

            isFinished = true;
        }

        if (lastPressedKey == m_pressedKey)
            return;

#ifdef DEBUG_KEYBOARD_WINDOWS
        qDebug() << "Virtual key: " << vkcode;
#endif
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

    void CKeyboardWindows::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }

    LRESULT CALLBACK CKeyboardWindows::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        // This is the reason why we have to use singleton pattern. We cannot pass a object pointer to
        // keyboardProc.
        CKeyboardWindows *keyboardWindows = qobject_cast<CKeyboardWindows*>(IKeyboard::getInstance());
        if (nCode == HC_ACTION)
        {
            KBDLLHOOKSTRUCT *keyboardEvent =reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            WPARAM vkCode = keyboardEvent->vkCode;
            keyboardWindows->processKeyEvent(vkCode, wParam);
        }
        return CallNextHookEx(keyboardWindows->keyboardHook(), nCode, wParam, lParam);
    }
}
