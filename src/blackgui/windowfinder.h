/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
