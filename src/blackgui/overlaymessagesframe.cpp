/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/overlaymessages.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/verify.h"

#include <QKeyEvent>
#include <QPoint>
#include <QRect>
#include <QStyle>
#include <Qt>
#include <algorithm>

namespace BlackGui
{
    COverlayMessagesFrame::COverlayMessagesFrame(QWidget *parent) :
        QFrame(parent)
    {
        // void
    }

    COverlayMessagesFrame::~COverlayMessagesFrame()
    { }

    void COverlayMessagesFrame::showStatusMessagesFrame()
    {
        this->initInnerFrame();
    }

    void COverlayMessagesFrame::showKillButton(bool killButton)
    {
        m_showKillButton = killButton;
        if (m_overlayMessages)
        {
            m_overlayMessages->showKillButton(killButton);
        }
    }

    void COverlayMessagesFrame::showOverlayMessagesWithConfirmation(
        const BlackMisc::CStatusMessageList &messages, bool appendOldMessages,
        const QString &confirmationMessage, std::function<void ()> okLambda,
        QMessageBox::StandardButton defaultButton, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->initInnerFrame();
        m_overlayMessages->showOverlayMessagesWithConfirmation(messages, appendOldMessages, confirmationMessage, okLambda, defaultButton, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::clearOverlayMessages()
    {
        m_overlayMessages->clearOverlayMessages();
    }

    void COverlayMessagesFrame::showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        this->initInnerFrame();
        m_overlayMessages->showOverlayMessage(message, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayMessages(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->initInnerFrame();
        m_overlayMessages->showOverlayMessages(messages, appendOldMessages, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs)
    {
        if (textMessage.isEmpty()) { return; }
        this->initInnerFrame();
        m_overlayMessages->showOverlayTextMessage(textMessage, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs)
    {
        this->initInnerFrame();
        m_overlayMessages->showOverlayVariant(variant, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayImage(const BlackMisc::CPixmap &pixmap, int timeOutMs)
    {
        this->initInnerFrame();
        m_overlayMessages->showOverlayImage(pixmap, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::keyPressEvent(QKeyEvent *event)
    {
        if (m_overlayMessages && event->key() == Qt::Key_Escape)
        {
            m_overlayMessages->close();
            event->accept();
        }
        else
        {
            QFrame::keyPressEvent(event);
        }
    }

    void COverlayMessagesFrame::resizeEvent(QResizeEvent *event)
    {
        QFrame::resizeEvent(event);
        if (m_overlayMessages && m_overlayMessages->isVisible())
        {
            this->initInnerFrame();
        }
    }

    QSize COverlayMessagesFrame::innerFrameSize() const
    {
        // check against minimum if widget is initialized, but not yet resized
        const int w = std::max(this->width(), this->minimumWidth());
        const int h = std::max(this->height(), this->minimumHeight());

        int wInner = m_widthFactor * w;
        int hInner = m_heightFactor * h;
        if (wInner > this->maximumWidth()) wInner = this->maximumWidth();
        if (hInner > this->maximumHeight()) hInner = this->maximumHeight();
        return QSize(wInner, hInner);
    }

    void COverlayMessagesFrame::initInnerFrame()
    {
        const QSize inner(innerFrameSize());
        if (!m_overlayMessages)
        {
            // lazy init
            m_overlayMessages = new COverlayMessages(inner.width(), inner.height(), this);
            m_overlayMessages->addShadow();
            m_overlayMessages->showKillButton(m_showKillButton);
        }

        Q_ASSERT(m_overlayMessages);

        const bool isFrameless = CGuiUtility::isMainWindowFrameless();
        const QPoint middle = this->geometry().center();
        const double yFactor = isFrameless ? 1.25 : 1.5; // 2 is middle in normal window
        const int w = inner.width();
        const int h = inner.height();
        const int x = middle.x() - w / 2;
        const int y = middle.y() - h / yFactor;
        m_overlayMessages->setGeometry(x, y, w, h);
    }
} // ns
