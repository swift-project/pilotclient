// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
