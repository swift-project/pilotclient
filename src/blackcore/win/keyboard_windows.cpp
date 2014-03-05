/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard_windows.h"
#include "keymapping_windows.h"
#include <QDebug>

using namespace BlackMisc::Hardware;

namespace BlackCore
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

    void CKeyboardWindows::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    int CKeyboardWindows::sizeOfRegisteredFunctions() const
    {
        int size = 0;
        foreach (QList<IKeyboard::RegistrationHandle> functions, m_registeredFunctions)
        {
            size += functions.size();
        }
        return size;
    }

    void CKeyboardWindows::triggerKey(const CKeyboardKey key, bool isPressed)
    {
        callFunctionsBy(key, isPressed);
    }

    void CKeyboardWindows::keyEvent(WPARAM vkcode, uint event)
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
                m_pressedKey.setNativeVirtualKey(vkcode);
            }
        }
        else if ((event == WM_KEYUP) || (event == WM_SYSKEYUP) )
        {
            if (CKeyMappingWindows::isModifier(vkcode))
                m_pressedKey.removeModifier(CKeyMappingWindows::convertToModifier(vkcode));
            else
            {
                m_pressedKey.setKey(Qt::Key_unknown);
                m_pressedKey.setNativeVirtualKey(0);
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
            callFunctionsBy(lastPressedKey, false);
            callFunctionsBy(m_pressedKey, true);
        }
    }

    IKeyboard::RegistrationHandle CKeyboardWindows::registerHotkeyImpl(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, std::function<void(bool)> function)
    {
        IKeyboard::RegistrationHandle handle;

        // Workaround: Remove key function. Otherwise operator== will not
        // work when we create the key value object by pressed keys
        key.setFunction(BlackMisc::Hardware::CKeyboardKey::HotkeyNone);

        if (!key.hasModifier() && !key.hasKey())
        {
            return handle;
        }

        if (receiver == nullptr)
            return handle;

        handle.m_key = key;
        handle.m_receiver = receiver;
        handle.function = function;

        QList<IKeyboard::RegistrationHandle> functions = m_registeredFunctions.value(key);

        functions.append(handle);
        m_registeredFunctions.insert(key, functions);

        return handle;
    }

    void CKeyboardWindows::unregisterHotkeyImpl(const IKeyboard::RegistrationHandle &handle)
    {
        QList<IKeyboard::RegistrationHandle> functions = m_registeredFunctions.value(handle.m_key);
        functions.removeAll(handle);
        m_registeredFunctions.insert(handle.m_key, functions);
    }

    void CKeyboardWindows::unregisterAllHotkeysImpl()
    {
        m_registeredFunctions.clear();
    }

    void CKeyboardWindows::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }

    void CKeyboardWindows::callFunctionsBy(const CKeyboardKey &key, bool isPressed)
    {
        QList<IKeyboard::RegistrationHandle> functionHandles = m_registeredFunctions.value(key);
        foreach (IKeyboard::RegistrationHandle functionHandle, functionHandles)
        {
            if (functionHandle.m_receiver.isNull())
            {
                continue;
            }
            functionHandle.function(isPressed);
        }
    }

    LRESULT CALLBACK CKeyboardWindows::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        CKeyboardWindows *keyboardWindows = qobject_cast<CKeyboardWindows*>(IKeyboard::getInstance());
        if (nCode == HC_ACTION)
        {
            KBDLLHOOKSTRUCT *keyboardEvent =reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            WPARAM vkCode = keyboardEvent->vkCode;
            keyboardWindows->keyEvent(vkCode, wParam);
        }
        return CallNextHookEx(keyboardWindows->keyboardHook(), nCode, wParam, lParam);
    }
}
