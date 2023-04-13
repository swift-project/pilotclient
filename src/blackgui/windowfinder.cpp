/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/windowfinder.h"

#include <QtGlobal>

#if defined(Q_OS_WIN)
#    include "win/windowfinderwindows.h"
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MACOS)
#else
#    error "Platform is not supported!"
#endif

namespace BlackGui
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
}
