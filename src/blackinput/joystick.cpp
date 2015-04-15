/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "joystick.h"

#if defined(Q_OS_WIN)
    #include "win/joystick_windows.h"
#elif defined(Q_OS_LINUX)
    #include "linux/joystick_linux.h"
#elif defined(Q_OS_OSX)
    #include "osx/joystick_mac.h"
#else
    #error "Platform is not supported!"
#endif

namespace BlackInput
{
    IJoystick *IJoystick::m_instance = nullptr;

    IJoystick::IJoystick(QObject *parent) :
        QObject(parent)
    {
    }

    IJoystick *IJoystick::getInstance()
    {
        if (!m_instance)
        {
#if defined(Q_OS_WIN)
            m_instance = new CJoystickWindows;
#elif defined(Q_OS_LINUX)
            m_instance = new CJoystickLinux;
#elif defined(Q_OS_OSX)
            m_instance = new CJoystickMac;
#endif
            Q_ASSERT_X(m_instance, "IJoystick::getInstance", "Pointer to IJoystick is nullptr!");
        }
        return m_instance;
    }

} // BlackInput
