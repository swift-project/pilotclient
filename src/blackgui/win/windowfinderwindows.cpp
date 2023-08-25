// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/win/windowfinderwindows.h"
#include <QWindow>
#include <QScopedArrayPointer>

#include <windows.h>

namespace BlackGui
{
    QWindow *CWindowFinderWindows::findForeignWindow(const QString &windowName, const QString &className)
    {
        QScopedArrayPointer<TCHAR> lpWindowName;
        QScopedArrayPointer<TCHAR> lpClassName;

        if (!windowName.isEmpty())
        {
            lpWindowName.reset(new TCHAR[windowName.size() + 1]);
            windowName.toWCharArray(lpWindowName.data());
            lpWindowName[windowName.size()] = 0;
        }
        if (!className.isEmpty())
        {
            lpClassName.reset(new TCHAR[className.size() + 1]);
            className.toWCharArray(lpClassName.data());
            lpClassName[className.size()] = 0;
        }

        HWND hWindow = FindWindow(lpClassName.data(), lpWindowName.data());
        if (hWindow) { return QWindow::fromWinId(reinterpret_cast<WId>(hWindow)); }

        return nullptr;
    }
}
