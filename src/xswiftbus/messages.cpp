// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#ifndef NOMINMAX
#    define NOMINMAX
#endif
#include "messages.h"
#include <XPLMGraphics.h>
#include <XPUIGraphics.h>

namespace XSwiftBus
{
    int CMessageBox::lineHeight()
    {
        static const int lh = [] {
            int lineHeight = 0;
            XPLMGetFontDimensions(xplmFont_Basic, nullptr, &lineHeight, nullptr);
            return lineHeight;
        }();
        return lh;
    }

    void CMessageBox::draw()
    {
        static const int lineHeight = CMessageBox::lineHeight();
        static const int lineSpace = lineHeight / 3;

        const int messageCount = static_cast<int>(m_messages.size());

        const int screenHeight = m_screenHeight.get();
        const int boxRight = m_screenWidth.get() - m_boxRight;
        const int boxLeft = m_boxLeft;
        int boxTop;
        int boxBottom;

        if (m_boxTop >= 0)
        {
            boxTop = screenHeight - m_boxTop;
            boxBottom = boxTop - lineSpace * 2 - (lineHeight + lineSpace) * messageCount;
        }
        else
        {
            boxBottom = m_boxBottom >= 0 ? m_boxBottom : 20;
            boxTop = boxBottom + lineSpace * 2 + (lineHeight + lineSpace) * messageCount;
        }

        if (boxTop > screenHeight) { boxTop = screenHeight; }
        if (boxBottom <= 0) { boxBottom = 0; }

        XPLMDrawTranslucentDarkBox(boxLeft, boxTop, boxRight, boxBottom);

        static int arrowWidth = 0, arrowHeight = 0;
        if (!arrowHeight)
        {
            XPGetElementDefaultDimensions(xpElement_LittleUpArrow, &arrowWidth, &arrowHeight, nullptr);
        }

        static const int x = boxLeft + lineSpace;
        if (m_upArrow)
        {
            const int y = boxTop - lineSpace - arrowHeight;
            XPDrawElement(x, y, x + arrowWidth, y + arrowHeight, xpElement_LittleUpArrow, 0);
        }
        if (m_downArrow)
        {
            const int y = boxTop - (lineHeight + lineSpace) * messageCount;
            XPDrawElement(x, y, x + arrowWidth, y + arrowHeight, xpElement_LittleDownArrow, 0);
        }
        for (int i = 0; i < messageCount; ++i)
        {
            const int y = boxTop - (lineHeight + lineSpace) * (i + 1);
            const size_t ii = static_cast<size_t>(i);
            XPLMDrawString(m_messages[ii].m_rgb.data(), x + arrowWidth + arrowWidth / 2, y,
                           const_cast<char *>(reinterpret_cast<const char *>(m_messages[ii].m_text.c_str())), nullptr, xplmFont_Basic);
        }
    }

    void CMessageBox::setValues(int leftPx, int topPx, int rightPx, int bottomPx, int lines, int durationMs)
    {
        m_boxBottom = bottomPx;
        m_boxLeft = leftPx;
        m_boxRight = rightPx;
        m_boxTop = topPx;
        m_lines = lines;
        m_durationMs = durationMs;
    }

    int CMessageBox::maxLineLength() const
    {
        static int len = 0;
        if (!len)
        {
            int charWidth;
            XPLMGetFontDimensions(xplmFont_Basic, &charWidth, nullptr, nullptr);
            const int boxRight = m_screenWidth.get() - m_boxRight;
            const int boxLeft = m_boxLeft;
            len = (boxRight - boxLeft - 20) / charWidth;
        }
        return len;
    }

    CMessageBoxControl::CMessageBoxControl(int left, int right, int top) : m_messageBox(left, right, top),
                                                                           m_showCommand("org/swift-project/xswiftbus/show_messages", "Show XSwiftBus text messages", [this] { show(); }),
                                                                           m_hideCommand("org/swift-project/xswiftbus/hide_messages", "Hide XSwiftBus text messages", [this] { hide(); }),
                                                                           m_toggleCommand("org/swift-project/xswiftbus/toggle_messages", "Toggle XSwiftBus text messages", [this] { toggle(); }),
                                                                           m_scrollUpCommand("org/swift-project/xswiftbus/scroll_up", "Scroll up XSwiftBus text messages", [this] { scrollUp(); }),
                                                                           m_scrollDownCommand("org/swift-project/xswiftbus/scroll_down", "Scroll down XSwiftBus text messages", [this] { scrollDown(); }),
                                                                           m_scrollToTopCommand("org/swift-project/xswiftbus/scroll_top", "Scroll to top of XSwiftBus text messages", [this] { scrollToTop(); }),
                                                                           m_scrollToBottomCommand("org/swift-project/xswiftbus/scroll_bottom", "Scroll to bottom of XSwiftBus text messages", [this] { scrollToBottom(); })
    {
        show();
    }

    void CMessageBoxControl::addMessage(const CMessage &message)
    {
        if (m_messages.size() >= c_maxTotalLines) { m_messages.erase(m_messages.begin()); }
        m_messages.push_back(message);
        if (m_position + 1 >= m_messages.size() || !m_visible)
        {
            scrollToBottom();
        }
    }

    void CMessageBoxControl::scrollUp()
    {
        if (!m_visible) { return; }

        if (m_position - 1 >= std::min(m_messages.size(), m_maxVisibleLines))
        {
            m_position--;
        }
        updateVisibleLines();
    }

    void CMessageBoxControl::scrollDown()
    {
        if (!m_visible) { return; }

        if (m_position + 1 <= m_messages.size())
        {
            m_position++;
        }
        updateVisibleLines();
    }

    void CMessageBoxControl::scrollToTop()
    {
        if (!m_visible) { return; }

        m_position = std::min(m_messages.size(), m_maxVisibleLines);
        updateVisibleLines();
    }

    void CMessageBoxControl::scrollToBottom()
    {
        m_position = m_messages.size();
        updateVisibleLines();
    }

    void CMessageBoxControl::updateVisibleLines()
    {
        const size_t lines = std::min(m_messages.size(), m_maxVisibleLines);
        // const auto end = m_messages.cbegin() +  m_position;
        // m_messageBox.setMessages(end - lines, end);
        auto end = m_messages.begin();
        std::advance(end, m_position);
        auto start = end;
        std::advance(start, -1 * static_cast<int>(lines));

        m_messageBox.setMessages(start, end);
        m_messageBox.enableArrows(m_position > lines, m_position < m_messages.size());
    }
}

//! \endcond
