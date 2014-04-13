/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard_linux.h"
#include <QDebug>

using namespace BlackMisc::Hardware;

namespace BlackCore
{
    CKeyboardLinux::CKeyboardLinux(QObject *parent) :
        IKeyboard(parent),
        m_mode(Mode_Nominal)
    {
    }

    CKeyboardLinux::~CKeyboardLinux()
    {
    }

    bool CKeyboardLinux::init()
    {
        return true;
    }

    void CKeyboardLinux::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    int CKeyboardLinux::sizeOfRegisteredFunctions() const
    {
        int size = 0;
        foreach (QList<IKeyboard::RegistrationHandle> functions, m_registeredFunctions)
        {
            size += functions.size();
        }
        return size;
    }

    void CKeyboardLinux::triggerKey(const CKeyboardKey key, bool isPressed)
    {
        callFunctionsBy(key, isPressed);
    }

    IKeyboard::RegistrationHandle CKeyboardLinux::registerHotkeyImpl(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, std::function<void(bool)> function)
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

    void CKeyboardLinux::unregisterHotkeyImpl(const IKeyboard::RegistrationHandle &handle)
    {
        QList<IKeyboard::RegistrationHandle> functions = m_registeredFunctions.value(handle.m_key);
        functions.removeAll(handle);
        m_registeredFunctions.insert(handle.m_key, functions);
    }

    void CKeyboardLinux::unregisterAllHotkeysImpl()
    {
        m_registeredFunctions.clear();
    }

    void CKeyboardLinux::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }

    void CKeyboardLinux::callFunctionsBy(const CKeyboardKey &key, bool isPressed)
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
}
