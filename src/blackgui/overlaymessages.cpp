/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/overlaymessages.h"
#include "blackgui/stylesheetutility.h"
#include "ui_overlaymessages.h"
#include <QKeyEvent>

using namespace BlackGui::Models;

namespace BlackGui
{
    COverlayMessages::COverlayMessages(int w, int h, QWidget *parent) :
        QFrame(parent),
        ui(new Ui::COverlayMessages)
    {
        this->init(w, h);
    }

    COverlayMessages::COverlayMessages(const QString &headerText, int w, int h, QWidget *parent) :
        QFrame(parent),
        ui(new Ui::COverlayMessages),
        m_header(headerText)
    {
        this->init(w, h);
    }

    void COverlayMessages::init(int w, int h)
    {
        ui->setupUi(this);
        resize(w, h);
        this->setAutoFillBackground(true);
        this->ui->tvp_StatusMessages->setMode(CStatusMessageListModel::Simplified);
        connect(this->ui->tb_Close, &QToolButton::released, this, &COverlayMessages::close);
        m_autoCloseTimer.setObjectName(objectName() + ":autoCloseTimer");
        connect(&m_autoCloseTimer, &QTimer::timeout, this, &COverlayMessages::close);
    }

    void COverlayMessages::setHeader(const QString &header)
    {
        if (m_header.isEmpty())
        {
            ui->lbl_Header->setText(header);
        }
        else
        {
            ui->lbl_Header->setText(m_header);
        }
    }

    COverlayMessages::~COverlayMessages()
    {}

    void COverlayMessages::showMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->setModeToMessages();
        this->ui->tvp_StatusMessages->updateContainer(messages);
        this->display(timeOutMs);
    }

    void COverlayMessages::showMessage(const BlackMisc::CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        this->setModeToMessage();
        this->ui->form_StatusMessage->setValue(message);
        this->display(timeOutMs);
    }

    void COverlayMessages::setModeToMessages()
    {
        this->ui->sw_StatusMessagesComponent->setCurrentWidget(this->ui->pg_StatusMessages);
        this->setHeader("Messages");
    }

    void COverlayMessages::setModeToMessage()
    {
        this->ui->sw_StatusMessagesComponent->setCurrentWidget(this->ui->pg_StatusMessage);
        this->setHeader("Message");
    }

    void COverlayMessages::keyPressEvent(QKeyEvent *event)
    {
        if (event->key() ==  Qt::Key_Escape)
        {
            this->close();
            event->setAccepted(true);
        }
    }

    void COverlayMessages::close()
    {
        this->hide();
        this->setEnabled(false);
    }

    void COverlayMessages::display(int timeOutMs)
    {
        this->show();
        this->setEnabled(true);
        if (timeOutMs > 250)
        {
            m_autoCloseTimer.start(timeOutMs);
        }
        else
        {
            m_autoCloseTimer.stop();
        }
    }

    void BlackGui::COverlayMessages::paintEvent(QPaintEvent *event)
    {
        bool s = CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_Widget);
        if (s) { return; }
        QFrame::paintEvent(event);
    }

} // ns
