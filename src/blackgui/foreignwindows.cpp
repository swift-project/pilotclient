// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/foreignwindows.h"
#include "blackgui/windowfinder.h"

#include <QWidget>
#include <QWindow>
#include <QtGlobal>

using namespace swift::misc::simulation;

namespace BlackGui
{
    QScopedPointer<IWindowFinder> CForeignWindows::m_windowFinder(IWindowFinder::create());

    QWindow *CForeignWindows::getFS9Window()
    {
        if (!m_windowFinder) { return nullptr; }
        QWindow *simulatorWindow = m_windowFinder->findForeignWindow("", "FS98MAIN");
        return simulatorWindow;
    }

    QWindow *CForeignWindows::getFSXWindow()
    {
        if (!m_windowFinder) { return nullptr; }
        QWindow *simulatorWindow = m_windowFinder->findForeignWindow("", "FS98MAIN");
        return simulatorWindow;
    }

    QWindow *CForeignWindows::getP3DWindow()
    {
        if (!m_windowFinder) { return nullptr; }
        QWindow *simulatorWindow = m_windowFinder->findForeignWindow("", "FS98MAIN");
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

    QWindow *CForeignWindows::getFirstFoundSimulatorWindow()
    {
        QWindow *w = CForeignWindows::getP3DWindow();
        if (w) { return w; }
        w = CForeignWindows::getXPlaneWindow();
        if (w) { return w; }
        w = CForeignWindows::getFSXWindow();
        if (w) { return w; }
        w = CForeignWindows::getFS9Window();
        return w;
    }

    QWindow *CForeignWindows::getSimulatorWindow(const CSimulatorInfo &simulator)
    {
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FS9: return CForeignWindows::getFS9Window();
        case CSimulatorInfo::FSX: return CForeignWindows::getFSXWindow();
        case CSimulatorInfo::P3D: return CForeignWindows::getP3DWindow();
        case CSimulatorInfo::XPLANE: return CForeignWindows::getXPlaneWindow();
        default: break;
        }
        return nullptr;
    }

    bool CForeignWindows::setSimulatorAsParent(QWindow *simulatorWindow, QWidget *child)
    {
        if (!simulatorWindow) { return false; }
        if (!child) { return false; }

        // If visible, hide it during the reparent. Otherwise setting the parent will have no effect.
        const bool isVisible = child->isVisible();
        if (isVisible) { child->hide(); }

        QWindow *childWindow = child->windowHandle();
        Q_ASSERT_X(childWindow, Q_FUNC_INFO, "Native resources for child widget have not yet been allocated. Did you call QWidget::show() before?");

        childWindow->setTransientParent(simulatorWindow);

        // If it was visible before, make it visible again
        if (isVisible) { child->show(); }
        return true;
    }

    bool CForeignWindows::unsetSimulatorAsParent(QWidget *child)
    {
        if (!child) { return false; }
        if (!child->windowHandle()) { return false; }
        if (!child->windowHandle()->transientParent()) { return false; }
        child->windowHandle()->setTransientParent(nullptr);
        return true;
    }
}
