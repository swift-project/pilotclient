/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard.h"

#if defined(Q_OS_WIN)
    #include "win/keyboard_windows.h"
#elif defined(Q_OS_LINUX)
    #include "linux/keyboard_linux.h"
#elif defined(Q_OS_OSX)
    #include "osx/keyboard_mac.h"
#else
    #error "Platform is not supported!"
#endif

namespace BlackInput
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
#elif defined(Q_OS_LINUX)
            m_instance = new CKeyboardLinux;
#elif defined(Q_OS_OSX)
            m_instance = new CKeyboardMac;
#endif
            Q_ASSERT_X(m_instance, "IKeyboard::getInstance", "Pointer to IKeyboard is NULL!");
            m_instance->init();
        }
        return m_instance;
    }

} // BlackInput
