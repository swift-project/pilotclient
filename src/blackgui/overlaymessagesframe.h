/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_OVERLAYMESSAGES_FRAME_H
#define BLACKGUI_OVERLAYMESSAGES_FRAME_H

#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessages.h"
#include "blackgui/guiutility.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QTabWidget>
#include <QMessageBox>
#include <QObject>
#include <QSize>
#include <QString>
#include <functional>

class QKeyEvent;
class QPaintEvent;
class QWidget;

namespace BlackGui
{
    /*!
     * Base class to display overlay messages in different widgets
     * (nested in this widget).
     * \fixme KB 2017-12 all header version, if someone manages to create a cpp version go ahead, I failed on gcc with "undefined reference to `BlackGui::COverlayMessagesBase<QFrame>::showOverlayMessages`"
     */
    template <class WIDGET> class COverlayMessagesBase : public WIDGET
    {
    public:
        //! Destructor
        virtual ~COverlayMessagesBase()
        { }

        //! Show the inner frame
        void showStatusMessagesFrame()
        {
            this->initInnerFrame();
        }

        //! Set the size factors
        void setOverlaySizeFactors(double widthFactor, double heightFactor, double middleFactor = 2)
        {
            m_widthFactor = widthFactor;
            m_heightFactor = heightFactor;
            if (middleFactor >=0) { m_middleFactor = middleFactor; }
        }

        //! \copydoc BlackGui::COverlayMessages::showKillButton
        void showKillButton(bool killButton)
        {
            m_showKillButton = killButton;
            if (m_overlayMessages)
            {
                m_overlayMessages->showKillButton(killButton);
            }
        }

        //! \copydoc BlackGui::COverlayMessages::setForceSmall
        void setForceSmall(bool force)
        {
            m_forceSmallMsgs = force;
            if (m_overlayMessages)
            {
                m_overlayMessages->setForceSmall(force);
            }
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessagesWithConfirmation
        void showOverlayMessagesWithConfirmation(
            const BlackMisc::CStatusMessageList &messages,
            bool                                appendOldMessages,
            const QString                       &confirmationMessage,
            std::function<void()>               okLambda,
            QMessageBox::StandardButton         defaultButton = QMessageBox::Cancel,
            int                                 timeOutMs = -1
        )
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessagesWithConfirmation(messages, appendOldMessages, confirmationMessage, okLambda, defaultButton, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::clearOverlayMessages
        void clearOverlayMessages()
        {
            if (!m_overlayMessages) { return; }
            m_overlayMessages->clearOverlayMessages();
        }

        //! \copydoc BlackGui::COverlayMessages::close
        void closeOverlay()
        {
            if (!m_overlayMessages) { return; }
            m_overlayMessages->close();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessages
        void showOverlayMessages(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessages(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessage
        void showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1)
        {
            if (message.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessage(message, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayTextMessage
        void showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs = -1)
        {
            if (textMessage.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayTextMessage(textMessage, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayVariant
        void showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs = -1)
        {
            this->initInnerFrame();
            m_overlayMessages->showOverlayVariant(variant, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayImage
        void showOverlayImage(const BlackMisc::CPixmap &pixmap, int timeOutMs = -1)
        {
            this->initInnerFrame();
            m_overlayMessages->showOverlayImage(pixmap, timeOutMs);
            WIDGET::repaint();
        }

    protected:
        COverlayMessages *m_overlayMessages = nullptr; //!< embedded QFrame with status messages

        //! Constructor
        COverlayMessagesBase(QWidget *parent) : WIDGET(parent)
        {
            const bool isFrameless = CGuiUtility::isMainWindowFrameless();
            m_middleFactor = isFrameless ? 1.25 : 1.5; // 2 is middle in normal window
        }

        //! Init the inner frame (if not yet initialized)
        void initInnerFrame()
        {
            const QSize inner(innerFrameSize());
            if (!m_overlayMessages)
            {
                // lazy init
                m_overlayMessages = new COverlayMessages(inner.width(), inner.height(), this);
                m_overlayMessages->addShadow();
                m_overlayMessages->showKillButton(m_showKillButton);
                m_overlayMessages->setForceSmall(m_forceSmallMsgs);
            }

            Q_ASSERT(m_overlayMessages);

            const QPoint middle = WIDGET::geometry().center();
            const int w = inner.width();
            const int h = inner.height();
            const int x = middle.x() - w / 2;
            const int y = middle.y() - h / m_middleFactor;
            m_overlayMessages->setGeometry(x, y, w, h);
        }

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override
        {
            if (m_overlayMessages && event->key() == Qt::Key_Escape)
            {
                m_overlayMessages->close();
                event->accept();
            }
            else
            {
                WIDGET::keyPressEvent(event);
            }
        }

        //! \copydoc QFrame::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override
        {
            WIDGET::resizeEvent(event);
            if (m_overlayMessages && m_overlayMessages->isVisible())
            {
                this->initInnerFrame();
            }
        }

    private:
        //! Calculate inner frame size
        QSize innerFrameSize() const
        {
            // check against minimum if widget is initialized, but not yet resized
            const int w = std::max(WIDGET::width(),  WIDGET::minimumWidth());
            const int h = std::max(WIDGET::height(), WIDGET::minimumHeight());

            int wInner = m_widthFactor * w;
            int hInner = m_heightFactor * h;
            if (wInner > WIDGET::maximumWidth())  { wInner = WIDGET::maximumWidth();  }
            if (hInner > WIDGET::maximumHeight()) { hInner = WIDGET::maximumHeight(); }
            return QSize(wInner, hInner);
        }

        bool m_showKillButton = false;     //!< show kill button
        bool m_forceSmallMsgs = false;     //!< force small messages
        double m_widthFactor  = 0.7;       //!< inner frame x factor
        double m_heightFactor = 0.6;       //!< inner frame x factor
        double m_middleFactor = 2;         //!< 2 means middle, 1 means on top
    };

    /*!
     * Using this class provides a QFrame with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesFrame : public COverlayMessagesBase<QFrame>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesFrame(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QTabWidget with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesTabWidget : public COverlayMessagesBase<QTabWidget>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTabWidget(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
