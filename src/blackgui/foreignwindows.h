/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FOREIGNWINDOWS_H
#define BLACKGUI_FOREIGNWINDOWS_H

#include "blackmisc/simulation/simulatorplugininfo.h"
#include "windowfinder.h"
#include <QWindow>
#include <QWidget>
#include <QScopedPointer>

namespace BlackGui
{
    //! Foreign windows
    class CForeignWindows
    {

    public:

        //! FS9 window
        static QWindow *getFS9Window();

        //! FSX window
        static QWindow *getFSXWindow();

        //! X-Plane window
        static QWindow *getXPlaneWindow();

        //! Set simulator as transient parent for child widget
        static void setSimulatorAsParent(QWindow *simulatorWindow, QWidget *child);

    private:

        //! Constructor, use static methods only
        CForeignWindows() {}

        static QScopedPointer<IWindowFinder> m_windowFinder;
    };
}

#endif // guard
