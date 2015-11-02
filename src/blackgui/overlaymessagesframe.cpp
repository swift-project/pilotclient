/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/stylesheetutility.h"
#include <QPainter>
#include <QKeyEvent>

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

    void COverlayMessagesFrame::showMessage(const BlackMisc::CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showMessage(message, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showMessages(messages, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs)
    {
        if (textMessage.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_overlayMessages->showTextMessage(textMessage, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::showVariant(const BlackMisc::CVariant &variant, int timeOutMs)
    {
        this->initInnerFrame();
        this->m_overlayMessages->showVariant(variant, timeOutMs);
        this->repaint();
    }

    void COverlayMessagesFrame::paintEvent(QPaintEvent *event)
    {
        bool s = CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_Widget);
        if (s) { return; }
        QFrame::paintEvent(event);
    }

    void COverlayMessagesFrame::keyPressEvent(QKeyEvent *event)
    {
        if (m_overlayMessages && event->key() == Qt::Key_Escape)
        {
            m_overlayMessages->close();
            event->setAccepted(true);
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
        int wInner = 0.7 * w;
        int hInner = 0.7 * h;
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
