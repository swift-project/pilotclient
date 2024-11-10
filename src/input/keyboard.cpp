// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "keyboard.h"

#if defined(Q_OS_WIN)
#    include "win/keyboardwindows.h"
#elif defined(Q_OS_LINUX)
#    include "linux/keyboardlinux.h"
#elif defined(Q_OS_MACOS)
#    include "macos/keyboardmacos.h"
#else
#    error "Platform is not supported!"
#endif

namespace swift::input
{
    IKeyboard::IKeyboard(QObject *parent) : QObject(parent) {}

    std::unique_ptr<IKeyboard> IKeyboard::create(QObject *parent)
    {
#if defined(Q_OS_WIN)
        std::unique_ptr<IKeyboard> ptr(new CKeyboardWindows(parent));
#elif defined(Q_OS_LINUX)
        std::unique_ptr<IKeyboard> ptr(new CKeyboardLinux(parent));
#elif defined(Q_OS_MACOS)
        std::unique_ptr<IKeyboard> ptr(new CKeyboardMacOS(parent));
#endif
        ptr->init();
        return ptr;
    }
} // ns
