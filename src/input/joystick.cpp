// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "joystick.h"

#if defined(Q_OS_WIN)
#    include "win/joystickwindows.h"
#elif defined(Q_OS_LINUX)
#    include "linux/joysticklinux.h"
#elif defined(Q_OS_MACOS)
#    include "macos/joystickmacos.h"
#else
#    error "Platform is not supported!"
#endif

namespace swift::input
{

    IJoystick::IJoystick(QObject *parent) : QObject(parent)
    {}

    std::unique_ptr<IJoystick> IJoystick::create(QObject *parent)
    {
#if defined(Q_OS_WIN)
        std::unique_ptr<IJoystick> ptr(new CJoystickWindows(parent));
#elif defined(Q_OS_LINUX)
        std::unique_ptr<IJoystick> ptr(new CJoystickLinux(parent));
#elif defined(Q_OS_MACOS)
        std::unique_ptr<IJoystick> ptr(new CJoystickMacOS(parent));
#endif
        ptr->init();
        return ptr;
    }

} // swift::input
