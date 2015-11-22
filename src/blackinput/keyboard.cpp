/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard.h"

#if defined(Q_OS_WIN)
    #include "win/keyboardwindows.h"
#elif defined(Q_OS_LINUX)
    #include "linux/keyboardlinux.h"
#elif defined(Q_OS_OSX)
    #include "osx/keyboardmac.h"
#else
    #error "Platform is not supported!"
#endif

namespace BlackInput
{

    IKeyboard::IKeyboard(QObject *parent) : QObject(parent) {}

    std::unique_ptr<IKeyboard> IKeyboard::create(QObject *parent)
    {
#if defined(Q_OS_WIN)
        std::unique_ptr<IKeyboard> ptr(new CKeyboardWindows(parent));
#elif defined(Q_OS_LINUX)
        std::unique_ptr<IKeyboard> ptr(new CKeyboardLinux(parent));
#elif defined(Q_OS_OSX)
        std::unique_ptr<IKeyboard> ptr(new CKeyboardMac(parent));
#endif
        ptr->init();
        return ptr;
    }

} // BlackInput
