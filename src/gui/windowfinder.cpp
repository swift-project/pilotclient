// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/windowfinder.h"

#include <QtGlobal>

#if defined(Q_OS_WIN)
#    include "win/windowfinderwindows.h"
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MACOS)
#else
#    error "Platform is not supported!"
#endif

namespace swift::gui
{
    IWindowFinder *IWindowFinder::create()
    {

#if defined(Q_OS_WIN)
        IWindowFinder *finder = new CWindowFinderWindows();
#elif defined(Q_OS_LINUX)
        IWindowFinder *finder = nullptr;
#elif defined(Q_OS_MACOS)
        IWindowFinder *finder = nullptr;
#endif

        return finder;
    }
} // namespace swift::gui
