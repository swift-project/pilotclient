// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_WINDOWFINDER_H
#define SWIFT_GUI_WINDOWFINDER_H

#include "gui/swiftguiexport.h"
#include <QString>

class QWindow;

namespace swift::gui
{
    //! Finder for foreign windows
    class SWIFT_GUI_EXPORT IWindowFinder
    {
    public:
        //! Constructor
        IWindowFinder() {}

        //! Destructor
        virtual ~IWindowFinder() {}

        //! Find and return a foreign window owned by another process
        virtual QWindow *findForeignWindow(const QString &windowName, const QString &className) = 0;

        //! Factory method
        static IWindowFinder *create();
    };
}

#endif // guard
