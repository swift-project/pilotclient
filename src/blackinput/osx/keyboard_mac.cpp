/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard_mac.h"
#include <QDebug>

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
        return true;
    }

    void CKeyboardMac::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    int CKeyboardMac::sizeOfRegisteredFunctions() const
    {
        int size = 0;
        foreach (QList<IKeyboard::RegistrationHandle> functions, m_registeredFunctions)
        {
            size += functions.size();
        }
        return size;
    }

    void CKeyboardMac::triggerKey(const CKeyboardKey key, bool isPressed)
    {
        callFunctionsBy(key, isPressed);
    }

    IKeyboard::RegistrationHandle CKeyboardMac::registerHotkeyImpl(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, std::function<void(bool)> function)
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

    void CKeyboardMac::unregisterHotkeyImpl(const IKeyboard::RegistrationHandle &handle)
    {
        QList<IKeyboard::RegistrationHandle> functions = m_registeredFunctions.value(handle.m_key);
        functions.removeAll(handle);
        m_registeredFunctions.insert(handle.m_key, functions);
    }

    void CKeyboardMac::unregisterAllHotkeysImpl()
    {
        m_registeredFunctions.clear();
    }

    void CKeyboardMac::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }

    void CKeyboardMac::callFunctionsBy(const CKeyboardKey &key, bool isPressed)
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
