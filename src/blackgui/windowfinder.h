// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_WINDOWFINDER_H
#define BLACKGUI_WINDOWFINDER_H

#include "blackgui/blackguiexport.h"
#include <QString>

class QWindow;

namespace BlackGui
{
    //! Finder for foreign windows
    class BLACKGUI_EXPORT IWindowFinder
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
