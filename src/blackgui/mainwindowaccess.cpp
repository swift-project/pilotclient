/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/logcomponent.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/overlaymessagesframe.h"

using namespace BlackMisc;

namespace BlackGui
{
    IMainWindowAccess::~IMainWindowAccess()
    {
        // void
    }

    bool IMainWindowAccess::displayTextInConsole(const QString &message)
    {
        if (message.isEmpty()) { return false; }
        if (!m_mwaLogComponent) { return false; }
        m_mwaLogComponent->appendPlainTextToConsole(message);
        m_mwaLogComponent->displayConsole(true);
        return true;
    }

    bool IMainWindowAccess::displayInStatusBar(const CStatusMessage &message)
    {
        if (message.isEmpty()) { return false; }
        if (!m_mwaStatusBar) { return false; }
        m_mwaStatusBar->displayStatusMessage(message);
        return true;
    }

    bool IMainWindowAccess::displayInOverlayWindow(const CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return false; }
        if (!m_mwaOverlayFrame) { return false; }
        m_mwaOverlayFrame->showOverlayMessage(message, timeOutMs);
        return true;
    }
} // ns
