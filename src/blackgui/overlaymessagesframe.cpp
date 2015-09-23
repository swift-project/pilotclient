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
        if (!m_statusMessages) { return; }
    }

    void COverlayMessagesFrame::showMessages(const BlackMisc::CStatusMessageList &messages)
    {
        if (messages.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_statusMessages->showMessages(messages);
        this->repaint();
    }

    void COverlayMessagesFrame::showMessage(const BlackMisc::CStatusMessage &message)
    {
        if (message.isEmpty()) { return; }
        this->initInnerFrame();
        this->m_statusMessages->showMessage(message);
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
        if (!m_statusMessages) { QFrame::keyPressEvent(event); }
        if (event->key() == Qt::Key_Escape)
        {
            m_statusMessages->close();
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
        if (!this->m_statusMessages)
        {
            this->m_statusMessages = new COverlayMessages(inner.width(), inner.height(), this);
        }

        QPoint middle = this->geometry().center();
        int w = inner.width();
        int h = inner.height();
        int x = middle.x() - w / 2;
        int y = middle.y() - h / 2;
        this->m_statusMessages->setGeometry(x, y, w, h);
    }
} // ns
