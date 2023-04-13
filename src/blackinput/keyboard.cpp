/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

namespace BlackInput
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
