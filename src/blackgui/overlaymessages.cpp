/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/overlaymessages.h"
#include "blackgui/guiapplication.h"
#include "ui_overlaymessages.h"
#include <QKeyEvent>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackGui::Models;
using namespace BlackGui::Views;

namespace BlackGui
{
    COverlayMessages::COverlayMessages(int w, int h, QWidget *parent) :
        QFrame(parent),
        ui(new Ui::COverlayMessages)
    {
        this->init(w, h);
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &COverlayMessages::ps_onStyleSheetsChanged);
        connect(this->ui->pb_Ok, &QPushButton::clicked, this, &COverlayMessages::ps_okClicked);
        connect(this->ui->pb_Cancel, &QPushButton::clicked, this, &COverlayMessages::ps_cancelClicked);

        this->ui->tvp_StatusMessages->setResizeMode(CStatusMessageView::ResizingAuto);
        this->ui->fr_Confirmation->setVisible(false);
        this->setDefaultConfirmationButton(QMessageBox::Cancel);
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

    void COverlayMessages::ps_onStyleSheetsChanged()
    {
        // stlye sheet
    }

    void COverlayMessages::ps_okClicked()
    {
        this->m_lastConfirmation = QMessageBox::Ok;
        if (this->m_okLambda)
        {
            this->m_okLambda();
        }
        this->close();
    }

    void COverlayMessages::ps_cancelClicked()
    {
        this->m_lastConfirmation = QMessageBox::Cancel;
        this->close();
    }

    bool COverlayMessages::useSmall() const
    {
        return (this->width() < 400);
    }

    COverlayMessages::~COverlayMessages()
    {}

    void COverlayMessages::showOverlayMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        this->setModeToMessages();
        this->ui->tvp_StatusMessages->updateContainer(messages);
        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        if (this->useSmall())
        {
            this->setModeToMessageSmall();
            this->ui->form_StatusMessageSmall->setValue(message);
        }
        else
        {
            this->setModeToMessage();
            this->ui->form_StatusMessage->setValue(message);
        }
        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayTextMessage(const CTextMessage &textMessage, int timeOutMs)
    {
        if (textMessage.isEmpty()) { return; }
        this->setModeToTextMessage();

        // message and display
        this->ui->le_TmFrom->setText(textMessage.getSenderCallsign().asString());
        this->ui->le_TmTo->setText(textMessage.getRecipientCallsign().asString());
        this->ui->le_TmReceived->setText(textMessage.getFormattedUtcTimestampHms());
        this->ui->te_TmText->setText(textMessage.getMessage());

        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayImage(const CPixmap &image, int timeOutMs)
    {
        this->showOverlayImage(image.toPixmap(), timeOutMs);
    }

    void COverlayMessages::showOverlayImage(const QPixmap &image, int timeOutMs)
    {
        this->setModeToImage();
        QSize sizeAvailable = this->ui->fr_StatusMessagesComponentsInner->size();
        if (sizeAvailable.width() < 300)
        {
            // first time inner frame is not giving correct size, workaround
            sizeAvailable = this->size() * 0.9;
        }

        this->ui->lbl_Image->setText("");
        this->ui->lbl_Image->setPixmap(
            image.scaled(sizeAvailable, Qt::KeepAspectRatio, Qt::FastTransformation)
        );
        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs)
    {
        if (variant.canConvert<CStatusMessageList>())
        {
            showOverlayMessages(variant.value<CStatusMessageList>(), timeOutMs);
        }
        else if (variant.canConvert<CStatusMessage>())
        {
            showOverlayMessage(variant.value<CStatusMessage>(), timeOutMs);
        }
        else if (variant.canConvert<CTextMessage>())
        {
            showOverlayTextMessage(variant.value<CTextMessage>(), timeOutMs);
        }
        else if (variant.canConvert<QPixmap>())
        {
            showOverlayImage(variant.value<QPixmap>(), timeOutMs);
        }
        else if (variant.canConvert<CPixmap>())
        {
            showOverlayImage(variant.value<CPixmap>(), timeOutMs);
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Unsupported type");
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

    void COverlayMessages::setModeToMessageSmall()
    {
        this->ui->sw_StatusMessagesComponent->setCurrentWidget(this->ui->pg_StatusMessageSmall);
        this->setHeader("Message");
    }

    void COverlayMessages::setModeToTextMessage()
    {
        this->ui->sw_StatusMessagesComponent->setCurrentWidget(this->ui->pg_TextMessage);
        this->setHeader("Text message");
    }

    void COverlayMessages::setModeToImage()
    {
        this->ui->sw_StatusMessagesComponent->setCurrentWidget(this->ui->pg_Image);
        this->setHeader("Image");
    }

    void COverlayMessages::setConfirmationMessage(const QString &message)
    {
        if (message.isEmpty())
        {
            this->ui->fr_Confirmation->setVisible(false);
        }
        else
        {
            this->ui->fr_Confirmation->setVisible(true);
            this->ui->lbl_Confirmation->setText(message);
        }
    }

    void COverlayMessages::showOverlayMessagesWithConfirmation(const CStatusMessageList &messages, const QString &confirmationMessage, std::function<void ()> okLambda, int defaultButton, int timeOutMs)
    {
        this->setConfirmationMessage(confirmationMessage);
        this->showOverlayMessages(messages, timeOutMs);
        this->m_okLambda = okLambda;
        this->setDefaultConfirmationButton(defaultButton);
    }

    void COverlayMessages::setDefaultConfirmationButton(int button)
    {
        switch (button)
        {
        case QMessageBox::Ok:
            this->ui->pb_Cancel->setDefault(false);
            this->ui->pb_Cancel->setAutoDefault(false);
            this->ui->pb_Ok->setDefault(true);
            this->ui->pb_Ok->setAutoDefault(true);
            this->m_lastConfirmation = QMessageBox::Ok;
            this->ui->pb_Ok->setFocus();
            break;
        case QMessageBox::Cancel:
            this->ui->pb_Ok->setDefault(false);
            this->ui->pb_Ok->setAutoDefault(false);
            this->ui->pb_Cancel->setDefault(true);
            this->ui->pb_Cancel->setAutoDefault(true);
            this->m_lastConfirmation = QMessageBox::Cancel;
            this->ui->pb_Cancel->setFocus();
            break;
        default:
            break;
        }
    }

    void COverlayMessages::keyPressEvent(QKeyEvent *event)
    {
        if (!this->isVisible()) { QFrame::keyPressEvent(event); }
        if (event->key() ==  Qt::Key_Escape)
        {
            this->close();
            event->accept();
        }
        else
        {
            QFrame::keyPressEvent(event);
        }
    }

    void COverlayMessages::close()
    {
        this->hide();
        this->setEnabled(false);
        this->ui->fr_Confirmation->setVisible(false);
        this->m_lastConfirmation = QMessageBox::Cancel;
        this->m_okLambda = nullptr;
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
