/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/foreignwindows.h"
#include "blackgui/windowfinder.h"

#include <QWidget>
#include <QWindow>
#include <QtGlobal>

namespace BlackGui
{
    QScopedPointer<IWindowFinder> CForeignWindows::m_windowFinder(IWindowFinder::create());

    QWindow *CForeignWindows::getFS9Window()
    {
        QWindow *simulatorWindow = nullptr;
        simulatorWindow = m_windowFinder->findForeignWindow("", "FS98MAIN");

        return simulatorWindow;
    }

    QWindow *CForeignWindows::getFSXWindow()
    {
        QWindow *simulatorWindow = nullptr;
        simulatorWindow = m_windowFinder->findForeignWindow("", "FS98MAIN");

        return simulatorWindow;
    }

    QWindow *CForeignWindows::getXPlaneWindow()
    {
        QWindow *simulatorWindow = nullptr;
        // FIXME:
        // Use datarefs Sim/operation/windows/system window via xswiftbus to grab the OS's native window handle
        // http://www.xsquawkbox.net/xpsdk/mediawiki/sim%252Foperation%252Fwindows%252Fsystem_window
        // For the time being, use IWindowFinder.
        // The if condition is to prevent a crash on Linux/MacOS.
        if (!m_windowFinder.isNull()) simulatorWindow = m_windowFinder->findForeignWindow("X-System", "");

        return simulatorWindow;
    }

    void CForeignWindows::setSimulatorAsParent(QWindow *simulatorWindow, QWidget *child)
    {
        if (!simulatorWindow) return;
        if (!child) return;

        bool isVisible = child->isVisible();

        // If visible, hide it during the reparent. Otherwise setting the parent will have no effect.
        if (isVisible) child->hide();

        QWindow *childWindow = child->windowHandle();
        Q_ASSERT_X(childWindow, "CForeignWindows::setSimulatorAsParent", "Native resources for child widget have not yet been allocated. Did you call QWidget::show() before?");

        childWindow->setTransientParent(simulatorWindow);

        // If it was visible before, make it visible again
        if (isVisible) child->show();
    }
}
