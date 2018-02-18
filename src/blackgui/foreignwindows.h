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

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include <QScopedPointer>

class QWidget;
class QWindow;

namespace BlackGui
{
    class IWindowFinder;

    //! Foreign windows
    class BLACKGUI_EXPORT CForeignWindows
    {
    public:
        //! FS9 window
        static QWindow *getFS9Window();

        //! FSX window
        static QWindow *getFSXWindow();

        //! P3D window
        static QWindow *getP3DWindow();

        //! X-Plane window
        static QWindow *getXPlaneWindow();

        //! First simulator window found
        static QWindow *getFirstFoundSimulatorWindow();

        //! Simulator window
        static QWindow *getSimulatorWindow(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Set simulator as transient parent for child widget
        static bool setSimulatorAsParent(QWindow *simulatorWindow, QWidget *child);

        //! Unset the parent
        static bool unsetSimulatorAsParent(QWidget *child);

    private:
        //! Constructor, use static methods only
        CForeignWindows() {}

        static QScopedPointer<IWindowFinder> m_windowFinder;
    };
}

#endif // guard
