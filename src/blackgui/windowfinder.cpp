/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "windowfinder.h"

#if defined(Q_OS_WIN)
    #include "win/windowfinder_win.h"
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_OSX)
#else
    #error "Platform is not supported!"
#endif

namespace BlackGui
{
    IWindowFinder *IWindowFinder::create()
    {
        IWindowFinder *finder = nullptr;

#if defined(Q_OS_WIN)
        finder = new CWindowFinderWindows();
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_OSX)
#endif

        return finder;
    }
}
