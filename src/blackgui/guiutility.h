/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIUTILITY_H
#define BLACKGUI_GUIUTILITY_H

#include <QWidgetList>
#include <QMainWindow>

namespace BlackGui
{
    //! GUI utilities
    class CGuiUtility
    {

    public:

        //! Main application window
        static QMainWindow *mainWindow();

        //! Top level widgets with names
        static QWidgetList topLevelApplicationWidgetsWithName();

        //! Position of main window
        static QPoint mainWindowPosition();

        //! Position of intro window
        static QPoint introWindowPosition();

        //! During initialization, when main window position might not be set set
        static QPoint assumedMainWindowPosition();

    private:
        //! Constructor, use static methods only
        CGuiUtility() {}
    };

}

#endif // guard
