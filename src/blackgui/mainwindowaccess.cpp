// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

    bool IMainWindowAccess::displayInOverlayWindow(const CStatusMessageList &messages, int timeOutMs)
    {
        if (messages.isEmpty()) { return false; }
        if (!m_mwaOverlayFrame) { return false; }
        m_mwaOverlayFrame->showOverlayMessages(messages, false, timeOutMs);
        return true;
    }

    bool IMainWindowAccess::displayInOverlayWindow(const QString &html, int timeOutMs)
    {
        if (html.isEmpty()) { return false; }
        if (!m_mwaOverlayFrame) { return false; }
        m_mwaOverlayFrame->showOverlayHTMLMessage(html, timeOutMs);
        return true;
    }
} // ns
