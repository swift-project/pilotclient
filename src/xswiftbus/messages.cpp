/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "messages.h"
#include <XPLMGraphics.h>
#include <XPUIGraphics.h>

namespace XSwiftBus
{

    void CMessageBox::draw()
    {
        const int messageCount = static_cast<int>(m_messages.size());

        static int lineHeight = 0;
        if (! lineHeight)
        {
            XPLMGetFontDimensions(xplmFont_Proportional, nullptr, &lineHeight, nullptr);
        }
        static const int lineSpace = lineHeight / 3;
        const int boxTop = m_screenHeight.get() - m_boxTop;
        const int boxBottom = boxTop - lineSpace * 2 - (lineHeight + lineSpace) * messageCount;
        const int boxRight = m_screenWidth.get() - m_boxRight;
        const int boxLeft = m_boxLeft;
        XPLMDrawTranslucentDarkBox(boxLeft, boxTop, boxRight, boxBottom);

        static int arrowWidth = 0, arrowHeight = 0;
        if (! arrowHeight)
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
            XPLMDrawString(m_messages[i].m_rgb.data(), x + arrowWidth + arrowWidth / 2, y, const_cast<char*>(m_messages[i].m_text.c_str()), nullptr, xplmFont_Proportional);
        }
    }

    int CMessageBox::maxLineLength() const
    {
        static int len = 0;
        if (! len)
        {
            int charWidth;
            XPLMGetFontDimensions(xplmFont_Proportional, &charWidth, nullptr, nullptr);
            const int boxRight = m_screenWidth.get() - m_boxRight;
            const int boxLeft = m_boxLeft;
            len = (boxRight - boxLeft - 20) / charWidth;
        }
        return len;
    }

    CMessageBoxControl::CMessageBoxControl(int left, int right, int top) :
        m_messageBox(left, right, top),
        m_showCommand("org/swift-project/xswiftbus/show_messages", "Show XSwiftBus text messages", [this] { show(); }),
        m_hideCommand("org/swift-project/xswiftbus/hide_messages", "Hide XSwiftBus text messages", [this] { hide(); }),
        m_toggleCommand("org/swift-project/xswiftbus/toggle_messages", "Toggle XSwiftBus text messages", [this] { toggle(); }),
        m_scrollUpCommand("org/swift-project/xswiftbus/scroll_up", "Scroll up XSwiftBus text messages", [this] { scrollUp(); }),
        m_scrollDownCommand("org/swift-project/xswiftbus/scroll_down", "Scroll down XSwiftBus text messages", [this] { scrollDown(); }),
        m_scrollToTopCommand("org/swift-project/xswiftbus/scroll_top", "Scroll to top of XSwiftBus text messages", [this] { scrollToTop(); }),
        m_scrollToBottomCommand("org/swift-project/xswiftbus/scroll_bottom", "Scroll to bottom of XSwiftBus text messages", [this] { scrollToBottom(); }),
        m_debugCommand("org/swift-project/xswiftbus/debug", "", [this] { static int c = 0; this->addMessage({ "hello " + std::to_string(c++), 0, .75, 0 }); })
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

//! \endcond
