/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "messages.h"
#include <XPLMGraphics.h>
#include <XPUIGraphics.h>

namespace XBus
{

    const int c_screenWidth = 1024;
    const int c_screenHeight = 768;
    const int c_boxLeft = 128;
    const int c_boxTop = c_screenHeight - 16;
    const int c_boxRight = c_screenWidth - 128;

    void CMessageBox::draw()
    {
        static int lineHeight = 0;
        if (! lineHeight)
        {
            XPLMGetFontDimensions(xplmFont_Basic, nullptr, &lineHeight, nullptr);
        }
        static const int lineSpace = lineHeight / 3;
        const int boxBottom = c_boxTop - lineSpace * 2 - (lineHeight + lineSpace) * m_messages.size();
        XPLMDrawTranslucentDarkBox(c_boxLeft, c_boxTop, c_boxRight, boxBottom);

        static int arrowWidth = 0, arrowHeight = 0;
        if (! arrowHeight)
        {
            XPGetElementDefaultDimensions(xpElement_LittleUpArrow, &arrowWidth, &arrowHeight, nullptr);
        }

        static const int x = c_boxLeft + lineSpace;
        if (m_upArrow)
        {
            const int y = c_boxTop - lineSpace - arrowHeight;
            XPDrawElement(x, y, x + arrowWidth, y + arrowHeight, xpElement_LittleUpArrow, 0);
        }
        if (m_downArrow)
        {
            const int y = c_boxTop - (lineHeight + lineSpace) * m_messages.size();
            XPDrawElement(x, y, x + arrowWidth, y + arrowHeight, xpElement_LittleDownArrow, 0);
        }
        for (size_t i = 0; i < m_messages.size(); ++i)
        {
            const int y = c_boxTop - (lineHeight + lineSpace) * (i + 1);
            XPLMDrawString(m_messages[i].m_rgb.data(), x + arrowWidth + arrowWidth / 2, y, const_cast<char*>(m_messages[i].m_text.c_str()), nullptr, xplmFont_Basic);
        }
    }

    int CMessageBox::maxLineLength() const
    {
        static int len = 0;
        if (! len)
        {
            int charWidth;
            XPLMGetFontDimensions(xplmFont_Basic, &charWidth, nullptr, nullptr);
            len = (c_boxRight - c_boxLeft - 20) / charWidth;
        }
        return len;
    }

    CMessageBoxControl::CMessageBoxControl() :
        m_showCommand("org/swift-project/xbus/show_messages", "Show XBus text messages", [this] { show(); }),
        m_hideCommand("org/swift-project/xbus/hide_messages", "Hide XBus text messages", [this] { hide(); }),
        m_toggleCommand("org/swift-project/xbus/toggle_messages", "Toggle XBus text messages", [this] { toggle(); }),
        m_scrollUpCommand("org/swift-project/xbus/scroll_up", "Scroll up XBus text messages", [this] { scrollUp(); }),
        m_scrollDownCommand("org/swift-project/xbus/scroll_down", "Scroll down XBus text messages", [this] { scrollDown(); }),
        m_scrollToTopCommand("org/swift-project/xbus/scroll_top", "Scroll to top of XBus text messages", [this] { scrollToTop(); }),
        m_scrollToBottomCommand("org/swift-project/xbus/scroll_bottom", "Scroll to bottom of XBus text messages", [this] { scrollToBottom(); }),
        m_debugCommand("org/swift-project/xbus/debug", "", [this] { static int c = 0; this->addMessage({ "hello " + std::to_string(c++), 0, .75, 0 }); })
    {}

    void CMessageBoxControl::addMessage(const CMessage &message)
    {
        if (m_messages.size() >= c_maxTotalLines) { m_messages.erase(m_messages.begin()); }
        m_messages.push_back(message);
        if (m_position + 1 >= m_messages.size() || ! m_visible)
        {
            show();
            scrollToBottom();
        }
    }

    void CMessageBoxControl::scrollUp()
    {
        if (! m_visible) { return; }

        if (m_position - 1 >= std::min(m_messages.size(), c_maxVisibleLines))
        {
            m_position--;
        }
        updateVisibleLines();
    }

    void CMessageBoxControl::scrollDown()
    {
        if (! m_visible) { return; }

        if (m_position + 1 <= m_messages.size())
        {
            m_position++;
        }
        updateVisibleLines();
    }

    void CMessageBoxControl::scrollToTop()
    {
        if (! m_visible) { return; }

        m_position = std::min(m_messages.size(), c_maxVisibleLines);
        updateVisibleLines();
    }

    void CMessageBoxControl::scrollToBottom()
    {
        if (! m_visible) { return; }

        m_position = m_messages.size();
        updateVisibleLines();
    }

    void CMessageBoxControl::updateVisibleLines()
    {
        const size_t lines = std::min(m_messages.size(), c_maxVisibleLines);
        const auto end = m_messages.cbegin() + m_position;
        m_messageBox.setMessages(end - lines, end);
        m_messageBox.enableArrows(m_position > lines, m_position < m_messages.size());
    }

}
