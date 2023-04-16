/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "keyboardwindows.h"

using namespace BlackMisc::Input;

namespace BlackInput
{
    static const auto &keyMapping()
    {
        static const QHash<int, KeyCode> hash {
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
            { VK_ADD, Key_Plus },
            { VK_OEM_PLUS, Key_Plus },
            { VK_SUBTRACT, Key_Minus },
            { VK_OEM_MINUS, Key_Minus },
            { VK_OEM_PERIOD, Key_Period },
            { VK_DIVIDE, Key_Divide },
            { VK_BACK, Key_Back },
            { VK_TAB, Key_Tab },
            { VK_ESCAPE, Key_Esc },
            { VK_SPACE, Key_Space },
            { VK_INSERT, Key_Insert },
            { VK_DELETE, Key_Delete },
            { VK_HOME, Key_Home },
            { VK_END, Key_End },
            { VK_PRIOR, Key_PageUp },
            { VK_NEXT, Key_PageDown },
            { VK_CAPITAL, Key_CapsLock },
            { VK_RETURN, Key_Enter },
            { VK_MULTIPLY, Key_Multiply },
            { VK_SUBTRACT, Key_NumpadMinus },
            { VK_ADD, Key_NumpadPlus },
            { VK_DECIMAL, Key_NumpadDelete },
            { VK_NUMPAD0, Key_Numpad0 },
            { VK_NUMPAD1, Key_Numpad1 },
            { VK_NUMPAD2, Key_Numpad2 },
            { VK_NUMPAD3, Key_Numpad3 },
            { VK_NUMPAD4, Key_Numpad4 },
            { VK_NUMPAD5, Key_Numpad5 },
            { VK_NUMPAD6, Key_Numpad6 },
            { VK_NUMPAD7, Key_Numpad7 },
            { VK_NUMPAD8, Key_Numpad8 },
            { VK_NUMPAD9, Key_Numpad9 },
            { VK_OEM_NEC_EQUAL, Key_NumpadEqual },
            { VK_OEM_COMMA, Key_Comma },
            { VK_OEM_1, Key_OEM1 },
            { VK_OEM_2, Key_OEM2 },
            { VK_OEM_3, Key_OEM3 },
            { VK_OEM_4, Key_OEM4 },
            { VK_OEM_5, Key_OEM5 },
            { VK_OEM_6, Key_OEM6 },
            { VK_OEM_7, Key_OEM7 },
            { VK_OEM_8, Key_OEM8 },
            { VK_OEM_102, Key_OEM102 },
            { VK_F1, Key_Function1 },
            { VK_F2, Key_Function2 },
            { VK_F3, Key_Function3 },
            { VK_F4, Key_Function4 },
            { VK_F5, Key_Function5 },
            { VK_F6, Key_Function6 },
            { VK_F7, Key_Function7 },
            { VK_F8, Key_Function8 },
            { VK_F9, Key_Function9 },
            { VK_F10, Key_Function10 },
            { VK_F11, Key_Function11 },
            { VK_F12, Key_Function12 },
            { VK_F13, Key_Function13 },
            { VK_F14, Key_Function14 },
            { VK_F15, Key_Function15 },
            { VK_F16, Key_Function16 },
            { VK_F17, Key_Function17 },
            { VK_F18, Key_Function18 },
            { VK_F19, Key_Function19 },
            { VK_F20, Key_Function20 },
            { VK_F21, Key_Function21 },
            { VK_F22, Key_Function22 },
            { VK_F23, Key_Function23 },
            { VK_F24, Key_Function24 },
        };
        return hash;
    }

    static CKeyboardWindows *g_keyboardWindows = nullptr;

    CKeyboardWindows::CKeyboardWindows(QObject *parent) : IKeyboard(parent),
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
        if (useWindowsHook) // cppcheck-suppress knownConditionTrueFalse
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
        CHotkeyCombination oldCombination(m_keyCombination);
        if ((event == WM_KEYDOWN) || (event == WM_SYSKEYDOWN))
        {
            auto key = keyMapping().value(static_cast<int>(vkcode));
            if (key == Key_Unknown) { return; }
            m_keyCombination.addKeyboardKey(CKeyboardKey(key));
        }
        else if ((event == WM_KEYUP) || (event == WM_SYSKEYUP))
        {
            auto key = keyMapping().value(static_cast<int>(vkcode));
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
        CHotkeyCombination oldCombination(m_keyCombination);
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
