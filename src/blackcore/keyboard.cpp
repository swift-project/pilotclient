/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard.h"

#if defined(Q_OS_WIN)
    #include "blackcore/win/keyboard_windows.h"
#endif

namespace BlackCore
{
    IKeyboard *IKeyboard::m_instance = nullptr;

    IKeyboard::IKeyboard(QObject *parent) :
        QObject(parent)
    {
    }

    IKeyboard *IKeyboard::getInstance()
    {
        if (!m_instance)
        {
#if defined(Q_OS_WIN)
            m_instance = new CKeyboardWindows;
#elif defined(Q_OS_OSX)
#elif defined(Q_OS_LINUX)
#endif
            Q_ASSERT_X(m_instance, "IKeyboard::getInstance", "Pointer to IKeyboard is NULL!");
            m_instance->init();
        }
        return m_instance;
    }

    bool operator==(IKeyboard::RegistrationHandle const &lhs, IKeyboard::RegistrationHandle const &rhs)
    {
        if ((lhs.m_key == rhs.m_key) && (lhs.m_receiver == rhs.m_receiver))
            return true;
        else
            return false;
    }

} // BlackCore
