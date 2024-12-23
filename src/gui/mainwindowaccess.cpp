// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/mainwindowaccess.h"

#include "gui/components/logcomponent.h"
#include "gui/managedstatusbar.h"
#include "gui/overlaymessagesframe.h"

using namespace swift::misc;

namespace swift::gui
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

    bool IMainWindowAccess::displayInOverlayWindow(const CStatusMessage &message, std::chrono::milliseconds timeout)
    {
        if (message.isEmpty()) { return false; }
        if (!m_mwaOverlayFrame) { return false; }
        m_mwaOverlayFrame->showOverlayMessage(message, timeout);
        return true;
    }

    bool IMainWindowAccess::displayInOverlayWindow(const CStatusMessageList &messages,
                                                   std::chrono::milliseconds timeout)
    {
        if (messages.isEmpty()) { return false; }
        if (!m_mwaOverlayFrame) { return false; }
        m_mwaOverlayFrame->showOverlayMessages(messages, false, timeout);
        return true;
    }

    bool IMainWindowAccess::displayInOverlayWindow(const QString &html, std::chrono::milliseconds timeout)
    {
        if (html.isEmpty()) { return false; }
        if (!m_mwaOverlayFrame) { return false; }
        m_mwaOverlayFrame->showOverlayHTMLMessage(html, timeout);
        return true;
    }
} // namespace swift::gui
