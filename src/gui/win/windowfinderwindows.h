// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_WINDOWFINDERWINDOWS_H
#define SWIFT_GUI_WINDOWFINDERWINDOWS_H

#include "gui/swiftguiexport.h"
#include "gui/windowfinder.h"

namespace swift::gui
{
    //! Foreign windows
    class SWIFT_GUI_EXPORT CWindowFinderWindows : public IWindowFinder
    {
    public:
        //! Constructor
        CWindowFinderWindows() {}

        //! Destructor
        virtual ~CWindowFinderWindows() {}

        //! \copydoc IWindowFinder::findForeignWindow
        virtual QWindow *findForeignWindow(const QString &windowName, const QString &className) override;
    };
} // namespace swift::gui

#endif // guard
