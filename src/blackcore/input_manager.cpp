/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "input_manager.h"

using namespace BlackInput;
using namespace BlackMisc;
using namespace BlackMisc::Hardware;

namespace BlackCore
{
    CInputManager *CInputManager::m_instance = nullptr;

    CInputManager::CInputManager(QObject *parent) :
        QObject(parent),
        m_keyboard(IKeyboard::getInstance()),
        m_joystick(IJoystick::getInstance())
    {
        connect(m_keyboard, &IKeyboard::keyUp, this, &CInputManager::ps_processKeyboardKeyUp);
        connect(m_keyboard, &IKeyboard::keyDown, this, &CInputManager::ps_processKeyboardKeyDown);
        connect(m_joystick, &IJoystick::buttonUp, this, &CInputManager::ps_processJoystickButtonUp);
        connect(m_joystick, &IJoystick::buttonDown, this, &CInputManager::ps_processJoystickButtonDown);
    }

    CInputManager *CInputManager::getInstance()
    {
        if (!m_instance)
        {
            m_instance = new CInputManager();
        }
        return m_instance;
    }

    void CInputManager::changeHotkeySettings(Settings::CSettingKeyboardHotkeyList hotkeys)
    {
        CKeyboardKeyList keyList;
        for (Settings::CSettingKeyboardHotkey settingHotkey : hotkeys)
        {
            CKeyboardKey key = settingHotkey.getKey();
            if (key.isEmpty()) continue;

            m_hashKeyboardKeyFunctions.insert(key, settingHotkey.getFunction());
            keyList.push_back(key);
        }
        m_keyboard->setKeysToMonitor(keyList);
    }

    void CInputManager::ps_processKeyboardKeyDown(const CKeyboardKey &key)
    {
        if (!m_hashKeyboardKeyFunctions.contains(key)) return;

        // Get configured hotkey function
        CHotkeyFunction hotkeyFunc = m_hashKeyboardKeyFunctions.value(key);
        callFunctionsBy(hotkeyFunc, true);
    }

    void CInputManager::ps_processKeyboardKeyUp(const CKeyboardKey &key)
    {
        if (!m_hashKeyboardKeyFunctions.contains(key)) return;

        // Get configured hotkey function
        CHotkeyFunction hotkeyFunc = m_hashKeyboardKeyFunctions.value(key);
        callFunctionsBy(hotkeyFunc, false);
    }

    void CInputManager::ps_processJoystickButtonDown(const CJoystickButton &button)
    {
        qDebug() << "Pressed Button" << button.getButtonIndex();
        if (!m_hashJoystickKeyFunctions.contains(button)) return;

        // Get configured hotkey function
        CHotkeyFunction hotkeyFunc = m_hashJoystickKeyFunctions.value(button);
        callFunctionsBy(hotkeyFunc, true);
    }

    void CInputManager::ps_processJoystickButtonUp(const CJoystickButton &button)
    {
        qDebug() << "Released Button" << button.getButtonIndex();
        if (!m_hashJoystickKeyFunctions.contains(button)) return;

        // Get configured hotkey function
        CHotkeyFunction hotkeyFunc = m_hashJoystickKeyFunctions.value(button);
        callFunctionsBy(hotkeyFunc, false);
    }

    void CInputManager::callFunctionsBy(const CHotkeyFunction &hotkeyFunction, bool isKeyDown)
    {
        if (!m_hashRegisteredFunctions.contains(hotkeyFunction)) return;
        auto func = m_hashRegisteredFunctions.value(hotkeyFunction);
        func(isKeyDown);
    }

    CInputManager::RegistrationHandle CInputManager::registerHotkeyFuncImpl(const BlackMisc::CHotkeyFunction &hotkeyFunction,
                                                                            QObject *receiver,
                                                                            std::function<void (bool)> function)
    {
        m_hashRegisteredFunctions.insert(hotkeyFunction, function);

        RegistrationHandle handle;
        handle.function = function;
        handle.hotkeyFunction = hotkeyFunction;
        handle.m_receiver = receiver;
        return handle;
    }
}
