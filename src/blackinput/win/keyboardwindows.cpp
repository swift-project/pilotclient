/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "keyboardwindows.h"

using namespace BlackMisc::Input;

namespace BlackInput
{
    static const auto &keyMapping()
    {
        static const QHash<int, KeyCode> hash
        {
            { '0', Key_0 },
            { '1', Key_1 },
            { '2', Key_2 },
            { '3', Key_3 },
            { '4', Key_4 },
            { '5', Key_5 },
            { '6', Key_6 },
            { '7', Key_7 },
            { '8', Key_8 },
            { '9', Key_9 },
            { 'A', Key_A },
            { 'B', Key_B },
            { 'C', Key_C },
            { 'D', Key_D },
            { 'E', Key_E },
            { 'F', Key_F },
            { 'G', Key_G },
            { 'H', Key_H },
            { 'I', Key_I },
            { 'J', Key_J },
            { 'K', Key_K },
            { 'L', Key_L },
            { 'M', Key_M },
            { 'N', Key_N },
            { 'O', Key_O },
            { 'P', Key_P },
            { 'Q', Key_Q },
            { 'R', Key_R },
            { 'S', Key_S },
            { 'T', Key_T },
            { 'U', Key_U },
            { 'V', Key_V },
            { 'W', Key_W },
            { 'X', Key_X },
            { 'Y', Key_Y },
            { 'Z', Key_Z },
            { VK_LSHIFT, Key_ShiftLeft },
            { VK_RSHIFT, Key_ShiftRight },
            { VK_LCONTROL, Key_ControlLeft },
            { VK_RCONTROL, Key_ControlRight },
            { VK_LMENU, Key_AltLeft },
            { VK_RMENU, Key_AltRight },
        };
        return hash;
    }

    static CKeyboardWindows *g_keyboardWindows = nullptr;

    CKeyboardWindows::CKeyboardWindows(QObject *parent) :
        IKeyboard(parent),
        m_keyboardHook(nullptr)
    {
        connect(&m_pollTimer, &QTimer::timeout, this, &CKeyboardWindows::pollKeyboardState);
    }

    CKeyboardWindows::~CKeyboardWindows()
    {
        if (m_keyboardHook) { UnhookWindowsHookEx(m_keyboardHook); }
    }

    bool CKeyboardWindows::init()
    {
        if (useWindowsHook)
        {
            Q_ASSERT_X(g_keyboardWindows == nullptr, "CKeyboardWindows::init", "Windows supports only one keyboard instance. Cannot initialize a second one!");
            g_keyboardWindows = this;
            HMODULE module;
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCTSTR>(&CKeyboardWindows::keyboardProc), &module);
            m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, CKeyboardWindows::keyboardProc, module, 0);
        }
        {
            m_pollTimer.start(50);
        }
        return true;
    }

    void CKeyboardWindows::processKeyEvent(DWORD vkcode, WPARAM event)
    {
        BlackMisc::Input::CHotkeyCombination oldCombination(m_keyCombination);
        if ((event == WM_KEYDOWN) || (event == WM_SYSKEYDOWN))
        {
            auto key = keyMapping().value(vkcode);
            if (key == Key_Unknown) { return; }
            m_keyCombination.addKeyboardKey(CKeyboardKey(key));
        }
        else if ((event == WM_KEYUP) || (event == WM_SYSKEYUP))
        {
            auto key = keyMapping().value(vkcode);
            if (key == Key_Unknown) { return; }
            m_keyCombination.removeKeyboardKey(CKeyboardKey(key));
        }

        if (oldCombination != m_keyCombination)
        {
            emit keyCombinationChanged(m_keyCombination);
        }
    }

    void CKeyboardWindows::pollKeyboardState()
    {
        BlackMisc::Input::CHotkeyCombination oldCombination(m_keyCombination);
        QList<int> vkeys = keyMapping().keys();
        for (int vkey : vkeys)
        {
            if ((GetKeyState(vkey) & 0x8000) && !m_pressedKeys.contains(vkey))
            {
                // key down
                auto key = keyMapping().value(vkey);
                if (key == Key_Unknown) { return; }
                m_pressedKeys.push_back(vkey);
                m_keyCombination.addKeyboardKey(CKeyboardKey(key));
            }
            else if (!(GetKeyState(vkey) & 0x8000) && m_pressedKeys.contains(vkey))
            {
                // key up
                auto key = keyMapping().value(vkey);
                if (key == Key_Unknown) { return; }
                m_pressedKeys.removeAll(vkey);
                m_keyCombination.removeKeyboardKey(CKeyboardKey(key));
            }
        }

        if (oldCombination != m_keyCombination)
        {
            emit keyCombinationChanged(m_keyCombination);
        }
    }

    LRESULT CALLBACK CKeyboardWindows::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode == HC_ACTION)
        {
            KBDLLHOOKSTRUCT *keyboardEvent = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
            DWORD vkCode = keyboardEvent->vkCode;
            g_keyboardWindows->processKeyEvent(vkCode, wParam);
        }
        return CallNextHookEx(g_keyboardWindows->m_keyboardHook, nCode, wParam, lParam);
    }
}
