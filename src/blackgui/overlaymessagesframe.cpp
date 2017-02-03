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
#include "blackmisc/network/textmessage.h"

#include <QKeyEvent>
#include <QPoint>
#include <QRect>
#include <QStyle>
#include <Qt>

namespace BlackGui
{
    COverlayMessagesFrame::COverlayMessagesFrame(QWidget *parent) :
        QFrame(parent)
    { }

    COverlayMessagesFrame::~COverlayMessagesFrame()
    {}

    void COverlayMessagesFrame::showStatusMessagesFrame()
    {
        this->initInnerFrame();
    }

    void COverlayMessagesFrame::hideStatusMessagesFrame()
    {
        if (!m_overlayMessages) { return; }
    }

    void COverlayMessagesFrame::showOverlayMessagesWithConfirmation(const BlackMisc::CStatusMessageList &messages, const QString &confirmationMessage, std::function<void ()> okLambda, int defaultButton, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showOverlayMessagesWithConfirmation(messages, confirmationMessage, okLambda, defaultButton, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showOverlayMessage(message, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showOverlayMessages(messages, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs)
    {
        if (textMessage.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showOverlayTextMessage(textMessage, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs)
    {
        this->initInnerFrame();
        this->m_overlayMessages->showOverlayVariant(variant, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showOverlayImage(const BlackMisc::CPixmap &pixmap, int timeOutMs)
    {
        this->initInnerFrame();
        this->m_overlayMessages->showOverlayImage(pixmap, timeOutMs);
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

    QSize COverlayMessagesFrame::innerFrameSize() const
    {
        int w = this->width();
        int h = this->height();
        int wInner = this->m_widthFactor * w;
        int hInner = this->m_heightFactor * h;
        if (wInner > this->maximumWidth()) wInner = this->maximumWidth();
        if (hInner > this->maximumHeight()) hInner = this->maximumHeight();
        return QSize(wInner, hInner);
    }

    void COverlayMessagesFrame::initInnerFrame()
    {
        QSize inner(innerFrameSize());
        if (!this->m_overlayMessages)
        {
            this->m_overlayMessages = new COverlayMessages(inner.width(), inner.height(), this);
        }

        QPoint middle = this->geometry().center();
        int w = inner.width();
        int h = inner.height();
        int x = middle.x() - w / 2;
        int y = middle.y() - h / 2;
        this->m_overlayMessages->setGeometry(x, y, w, h);
    }
} // ns
