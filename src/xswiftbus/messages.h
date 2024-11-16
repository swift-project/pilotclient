// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_MESSAGES_H
#define SWIFT_SIM_XSWIFTBUS_MESSAGES_H

//! \file

#include <algorithm>
#include <array>
#include <iterator>
#include <string>
#include <vector>

#include "command.h"
#include "datarefs.h"
#include "drawable.h"

namespace XSwiftBus
{
    //! \cond
    namespace Private
    {
        inline auto empty_u8string()
        {
            using namespace std::literals;
            return u8""s;
        }
    } // namespace Private
    //! \endcond

    /*!
     * Class representing a single line of text to be drawn in a message box.
     */
    struct CMessage
    {
        //! String type.
        using string = decltype(Private::empty_u8string());

        //! Constructor.
        CMessage(const string &text, float r = 1, float g = 1, float b = 1) : m_text(text), m_rgb { { r, g, b } } {}

        //! Text.
        string m_text;

        //! Color.
        std::array<float, 3> m_rgb;
    };

    /*!
     * Class for drawing a gray box with text messages.
     */
    class CMessageBox : public CDrawable
    {
    public:
        //! Constructor.
        //! \param left  Number of "virtual pixels" between screen left edge and box left edge.
        //! \param right Number of "virtual pixels" between screen right edge and box right edge.
        //! \param top   Number of "virtual pixels" between screen top edge and box top edge.
        CMessageBox(int left, int right, int top)
            : CDrawable(xplm_Phase_Window, true), m_boxLeft(left), m_boxRight(right), m_boxTop(top)
        {}

        //! Set messages to draw in message box, from a pair of iterators.
        template <typename Iterator>
        void setMessages(Iterator begin, Iterator end)
        {
            m_messages.clear();
            std::copy(begin, end, std::back_inserter(m_messages));
        }

        //! Set margin values
        void setValues(int leftPx, int topPx, int rightPx, int bottomPx, int lines, int durationMs);

        //! Set whether to draw a small arrow at the bottom of the box.
        void enableArrows(bool up, bool down)
        {
            m_upArrow = up;
            m_downArrow = down;
        }

        //! Returns the maximum number of characters per line.
        int maxLineLength() const;

        //! Line height based on font
        static int lineHeight();

    protected:
        virtual void draw() override;

    private:
        std::vector<CMessage> m_messages;
        bool m_upArrow = false;
        bool m_downArrow = false;
        int m_boxLeft = 0;
        int m_boxRight = 0;
        int m_boxTop = 0;
        int m_boxBottom = 0;
        int m_lines = 10;
        int m_durationMs = 2500;

        // Screen
        DataRef<xplane::data::sim::graphics::view::window_width> m_screenWidth;
        DataRef<xplane::data::sim::graphics::view::window_height> m_screenHeight;
    };

    /*!
     * Class which builds upon CMessageBox with a scrollback buffer and commands for user control.
     */
    class CMessageBoxControl
    {
    public:
        //! \copydoc CMessageBox::CMessageBox
        CMessageBoxControl(int left, int right, int top);

        //! Add a new message to the bottom of the list.
        void addMessage(const CMessage &message);

        //! \copydoc XSwiftBus::CMessageBox::maxLineLength
        int maxLineLength() const { return m_messageBox.maxLineLength(); }

        //! \copydoc XSwiftBus::CMessageBox::setValues
        void setValues(int leftPx, int topPx, int rightPx, int bottomPx, int lines, int durationMs)
        {
            m_messageBox.setValues(leftPx, topPx, rightPx, bottomPx, lines, durationMs);
            this->setMaxVisibleLines(static_cast<size_t>(lines));
        }

        //! Set max. visible lines
        void setMaxVisibleLines(size_t lines) { m_maxVisibleLines = lines; }

        //! Toggles the visibility of the message box
        void toggle()
        {
            if (m_visible) { hide(); }
            else { show(); }
        }

        //! Is message box currently visible?
        bool isVisible() const { return m_visible; }

    private:
        void show()
        {
            m_messageBox.show();
            m_visible = true;
        }
        void hide()
        {
            m_messageBox.hide();
            m_visible = false;
        }

        void scrollUp();
        void scrollDown();
        void scrollToTop();
        void scrollToBottom();
        void updateVisibleLines();

        bool m_visible = false;
        std::vector<CMessage> m_messages;
        size_t m_position = 0;
        size_t m_maxVisibleLines = 5;
        const size_t c_maxTotalLines = 1024;
        CMessageBox m_messageBox;

        CCommand m_showCommand;
        CCommand m_hideCommand;
        CCommand m_toggleCommand;
        CCommand m_scrollUpCommand;
        CCommand m_scrollDownCommand;
        CCommand m_scrollToTopCommand;
        CCommand m_scrollToBottomCommand;
    };
} // namespace XSwiftBus

#endif
