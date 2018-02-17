/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_WINDOWFINDERWINDOWS_H
#define BLACKGUI_WINDOWFINDERWINDOWS_H

#include "blackgui/blackguiexport.h"
#include "blackgui/windowfinder.h"

namespace BlackGui
{
    //! Foreign windows
    class BLACKGUI_EXPORT CWindowFinderWindows : public IWindowFinder
    {
    public:
        //! Constructor
        CWindowFinderWindows() {}

        //! Destructor
        virtual ~CWindowFinderWindows() {}

        //! \copydoc IWindowFinder::findForeignWindow
        virtual QWindow *findForeignWindow(const QString &windowName, const QString &className) override;
    };
}

#endif // guard
