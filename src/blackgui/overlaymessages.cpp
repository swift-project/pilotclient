/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/statusmessageform.h"
#include "blackgui/components/statusmessageformsmall.h"
#include "blackgui/views/statusmessageview.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackgui/overlaymessages.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/network/textmessage.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/application.h"
#include "ui_overlaymessages.h"

#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QSize>
#include <QStackedWidget>
#include <QStyle>
#include <QGraphicsDropShadowEffect>
#include <QTextEdit>
#include <QToolButton>
#include <Qt>
#include <QtGlobal>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui::Models;
using namespace BlackGui::Views;
using namespace BlackGui::Settings;
using namespace BlackGui::Components;

namespace BlackGui
{
    COverlayMessages::COverlayMessages(int w, int h, QWidget *parent) : QFrame(parent),
                                                                        ui(new Ui::COverlayMessages)
    {
        this->init(w, h);
        this->showKillButton(false);

        if (sGui) { connect(sGui, &CGuiApplication::styleSheetsChanged, this, &COverlayMessages::onStyleSheetsChanged, Qt::QueuedConnection); }
        connect(ui->pb_Ok, &QPushButton::clicked, this, &COverlayMessages::onOkClicked);
        connect(ui->pb_Cancel, &QPushButton::clicked, this, &COverlayMessages::onCancelClicked);
        connect(ui->tb_Kill, &QPushButton::clicked, this, &COverlayMessages::onKillClicked);

        ui->tvp_StatusMessages->setResizeMode(CStatusMessageView::ResizingAlways);
        ui->tvp_StatusMessages->setForceColumnsToMaxSize(false); // problems with multiline entries, with T138 we need multiline messages
        ui->tvp_StatusMessages->menuAddItems(CStatusMessageView::MenuSave);
        ui->fr_Confirmation->setVisible(false);

        ui->comp_OverlayTextMessage->showSettings(false);
        ui->comp_OverlayTextMessage->showTextMessageEntry(true);
        ui->comp_OverlayTextMessage->setAsUsedInOverlayMode();
        ui->comp_OverlayTextMessage->activate(false, false); // per default ignore incoming/outgoing text messages
        ui->comp_OverlayTextMessage->removeAllMessagesTab();
        ui->comp_OverlayTextMessage->setAtcButtonsRowsColumns(2, 3, true);
        ui->comp_OverlayTextMessage->setAtcButtonsBackgroundUpdates(false);

        this->setDefaultConfirmationButton(QMessageBox::Cancel);
    }

    COverlayMessages::~COverlayMessages()
    {}

    void COverlayMessages::init(int w, int h)
    {
        ui->setupUi(this);
        if (this->parent() && !this->parent()->objectName().isEmpty())
        {
            const QString n("Overlay messages for " + this->parent()->objectName());
            this->setObjectName(n);
        }

        this->resize(w, h);
        this->setAutoFillBackground(true);
        m_autoCloseTimer.setObjectName(this->objectName() % ":autoCloseTimer");
        ui->tvp_StatusMessages->setMode(CStatusMessageListModel::Simplified);
        connect(ui->tb_Close, &QToolButton::released, this, &COverlayMessages::close);
        connect(&m_autoCloseTimer, &QTimer::timeout, this, &COverlayMessages::close);
    }

    void COverlayMessages::setHeader(const QString &header)
    {
        ui->lbl_Header->setText(m_header.isEmpty() ? header : m_header);
    }

    void COverlayMessages::onStyleSheetsChanged()
    {
        // stylesheet
    }

    void COverlayMessages::onOkClicked()
    {
        m_lastConfirmation = QMessageBox::Ok;
        if (m_okLambda) { m_okLambda(); }
        this->close();
    }

    void COverlayMessages::onCancelClicked()
    {
        m_lastConfirmation = QMessageBox::Cancel;
        this->close();
    }

    void COverlayMessages::onKillClicked()
    {
        QMessageBox msgBox;
        msgBox.setText("Shutdown the application.");
        msgBox.setInformativeText(u"Do you want to terminate " % sGui->getApplicationNameAndVersion() % u"?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        if (QMessageBox::Ok == msgBox.exec() && sGui)
        {
            sGui->gracefulShutdown();
            sGui->exit();
        }
    }

    bool COverlayMessages::useSmall() const
    {
        return (m_forceSmall || this->width() < 400 || this->height() < 300);
    }

    void COverlayMessages::showKill(bool show)
    {
        ui->tb_Kill->setVisible(m_hasKillButton && show);
        ui->tb_Kill->setEnabled(m_hasKillButton && show);
    }

    bool COverlayMessages::displayTextMessage(const CTextMessage &textMessage) const
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return false; }
        const CTextMessageSettings s = m_messageSettings.getThreadLocal();
        if (s.popup(textMessage)) { return true; } // fast check without needing own aircraft
        const CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
        return s.popup(textMessage, ownAircraft);
    }

    void COverlayMessages::showOverlayMessages(const CStatusMessageList &messages, bool appendOldMessages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (this->hasPendingConfirmation())
        {
            // defer message
            QPointer<COverlayMessages> myself(this);
            m_pendingMessageCalls.push_back([=]() {
                if (!myself) { return; }
                myself->showOverlayMessages(messages, timeOutMs);
            });
            return;
        }

        CStatusMessageList newMsgs(messages);
        if (appendOldMessages && !ui->tvp_StatusMessages->isEmpty())
        {
            newMsgs.push_back(ui->tvp_StatusMessages->container());
        }

        //! \fixme KB 2017-09 a possible alternative maybe is to resize rows always to content -> performance?
        ui->tvp_StatusMessages->adjustOrderColumn(newMsgs);
        ui->tvp_StatusMessages->rowsResizeModeBasedOnThreshold(newMsgs.size());
        ui->tvp_StatusMessages->updateContainerMaybeAsync(newMsgs);

        this->setModeToMessages(false);
        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayMessagesOrSingleMessage(const CStatusMessageList &messages, bool appendOldMessages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (messages.size() > 1)
        {
            this->showOverlayMessages(messages, appendOldMessages, timeOutMs);
        }
        else
        {
            this->showOverlayMessage(messages.front(), timeOutMs);
        }
    }

    void COverlayMessages::showOverlayMessagesOrHTMLMessage(const CStatusMessageList &messages, bool appendOldMessages, int timeOutMs)
    {
        if (messages.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (messages.size() > 1)
        {
            this->showOverlayMessages(messages, appendOldMessages, timeOutMs);
        }
        else
        {
            this->showHTMLMessage(messages.front(), timeOutMs);
        }
    }

    void COverlayMessages::sortOverlayMessages(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        ui->tvp_StatusMessages->sortByPropertyIndex(propertyIndex, order);
    }

    void COverlayMessages::setOverlayMessagesSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        ui->tvp_StatusMessages->setSorting(propertyIndex, order);
    }

    void COverlayMessages::showOverlayMessage(const CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (this->hasPendingConfirmation())
        {
            // defer message
            QPointer<COverlayMessages> myself(this);
            m_pendingMessageCalls.push_back([=]() {
                if (!myself) { return; }
                myself->showOverlayMessage(message, timeOutMs);
            });
            return;
        }

        if (this->useSmall())
        {
            this->setModeToMessageSmall(message.isFailure());
            ui->form_StatusMessageSmall->setValue(message);
        }
        else
        {
            this->setModeToMessage(message.isFailure());
            ui->form_StatusMessage->setValue(message);
        }
        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayTextMessage(const CTextMessage &textMessage, int timeOutMs)
    {
        if (textMessage.isEmpty()) { return; }
        if (!this->displayTextMessage(textMessage)) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }

        if (this->hasPendingConfirmation())
        {
            // defer message
            QPointer<COverlayMessages> myself(this);
            m_pendingMessageCalls.push_back([=]() {
                if (!myself) { return; }
                myself->showOverlayTextMessage(textMessage, timeOutMs);
            });
            return;
        }

        // do we support inline text messages, we use this one
        const bool activatedText = ui->comp_OverlayTextMessage->isActivated();
        if (activatedText)
        {
            if (textMessage.isPrivateMessage())
            {
                this->showOverlayInlineTextMessage(textMessage.getSenderCallsign());
            }
            else
            {
                this->showOverlayInlineTextMessage(TextMessagesAll);
                ui->comp_OverlayTextMessage->showCorrespondingTabForFrequency(textMessage.getFrequency());
            }
        }
        else
        {
            // message and display
            //! @deprecated KB 2019-02 normally using overlay messages is used
            ui->le_TmFrom->setText(textMessage.getSenderCallsign().asString());
            ui->le_TmTo->setText(textMessage.getRecipientCallsign().asString());
            ui->le_TmReceived->setText(textMessage.getFormattedUtcTimestampHms());
            ui->te_TmText->setText(textMessage.getMessage());
            ui->wi_TmSupervisor->setVisible(textMessage.isSupervisorMessage());
            ui->wi_TmSupervisor->setStyleSheet("background-color: red;");

            this->setModeToTextMessage();
            this->display(timeOutMs);
        }
    }

    void COverlayMessages::showOverlayInlineTextMessage(TextMessageTab tab)
    {
        ui->comp_OverlayTextMessage->setTab(tab);
        ui->comp_OverlayTextMessage->updateAtcStationsButtons();
        ui->comp_OverlayTextMessage->showTextMessageEntry(true);
        this->setModeToOverlayTextMessage();
        this->display();
        ui->comp_OverlayTextMessage->focusTextEntry();
    }

    void COverlayMessages::showOverlayInlineTextMessage(const CCallsign &callsign)
    {
        this->showOverlayInlineTextMessage(TextMessagesUnicom);
        ui->comp_OverlayTextMessage->showCorrespondingTab(callsign);
    }

    void COverlayMessages::showOverlayImage(const CPixmap &image, int timeOutMs)
    {
        this->showOverlayImage(image.toPixmap(), timeOutMs);
    }

    void COverlayMessages::showOverlayImage(const QPixmap &image, int timeOutMs)
    {
        if (this->hasPendingConfirmation())
        {
            // defer message
            QPointer<COverlayMessages> myself(this);
            m_pendingMessageCalls.push_back([=]() {
                if (!myself) { return; }
                myself->showOverlayImage(image, timeOutMs);
            });
            return;
        }

        this->setModeToImage();
        QSize sizeAvailable = ui->fr_StatusMessagesComponentsInner->size();
        if (sizeAvailable.width() < 300)
        {
            // first time inner frame is not giving correct size, workaround
            sizeAvailable = this->size() * 0.9;
        }

        ui->lbl_Image->setText("");
        if (image.isNull())
        {
            static const QPixmap e;
            ui->lbl_Image->setPixmap(e);
        }
        else
        {
            ui->lbl_Image->setPixmap(
                image.scaled(sizeAvailable, Qt::KeepAspectRatio, Qt::FastTransformation));
        }
        this->display(timeOutMs);
    }

    void COverlayMessages::showOverlayVariant(const CVariant &variant, int timeOutMs)
    {
        if (variant.canConvert<CStatusMessageList>())
        {
            this->showOverlayMessages(variant.value<CStatusMessageList>(), timeOutMs);
        }
        else if (variant.canConvert<CStatusMessage>())
        {
            this->showOverlayMessage(variant.value<CStatusMessage>(), timeOutMs);
        }
        else if (variant.canConvert<CTextMessage>())
        {
            this->showOverlayTextMessage(variant.value<CTextMessage>(), timeOutMs);
        }
        else if (variant.canConvert<QPixmap>())
        {
            this->showOverlayImage(variant.value<QPixmap>(), timeOutMs);
        }
        else if (variant.canConvert<CPixmap>())
        {
            this->showOverlayImage(variant.value<CPixmap>(), timeOutMs);
        }
        else
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Unsupported type");
        }
    }

    void COverlayMessages::showHTMLMessage(const CStatusMessage &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        this->showHTMLMessage(message.toHtml(true, true), timeOutMs);
    }

    void COverlayMessages::showHTMLMessage(const QString &htmlMessage, int timeOutMs)
    {
        if (htmlMessage.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }

        if (this->hasPendingConfirmation())
        {
            // defer message
            QPointer<COverlayMessages> myself(this);
            m_pendingMessageCalls.push_back([=]() {
                if (!myself) { return; }
                myself->showHTMLMessage(htmlMessage, timeOutMs);
            });
            return;
        }

        this->setModeToHTMLMessage();
        ui->te_HTMLMessage->setTextColor(QColor(Qt::white)); // hardcoded color
        ui->te_HTMLMessage->setReadOnly(true);
        ui->te_HTMLMessage->setText(htmlMessage);
        this->display(timeOutMs);
    }

    void COverlayMessages::showDownloadProgress(int progress, qint64 current, qint64 max, const QUrl &url, int timeOutMs)
    {
        if (progress >= 0 && max >= 0)
        {
            static const QString m("%1 of %2 from '%3'");
            this->showProgressBar(progress, m.arg(current).arg(max).arg(url.toString()), timeOutMs);
        }
        else
        {
            static const QString m("%1 from '%2'");
            this->showHTMLMessage(m.arg(current).arg(url.toString()), timeOutMs);
        }
    }

    void COverlayMessages::showProgressBar(int percentage, const QString &message, int timeOutMs)
    {
        if (message.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (this->hasPendingConfirmation()) { return; } // ignore if something else is pending

        const int p = qMax(qMin(percentage, 100), 0);

        ui->pb_ProgressBar->setValue(p);
        ui->lbl_ProgressBar->setText(message);
        this->setModeToProgressBar();

        if (p >= 100)
        {
            this->close();
        }
        else
        {
            this->display(timeOutMs);
        }
    }

    void COverlayMessages::showKillButton(bool killButton)
    {
        m_hasKillButton = killButton;
        this->showKill(killButton);
    }

    void COverlayMessages::setModeToMessages(bool withKillButton)
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_StatusMessages);
        this->showKill(withKillButton);
        this->setHeader("Messages");
    }

    void COverlayMessages::setModeToMessage(bool withKillButton)
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_StatusMessage);
        this->showKill(withKillButton);
        this->setHeader("Message");
    }

    void COverlayMessages::setModeToMessageSmall(bool withKillButton)
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_StatusMessageSmall);
        this->showKill(withKillButton);
        this->setHeader("Message");
    }

    void COverlayMessages::setModeToHTMLMessage(bool withKillButton)
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_HTMLMessage);
        this->showKill(withKillButton);
        this->setHeader("Message");
    }

    void COverlayMessages::setModeToProgressBar(bool withKillButton)
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_ProgressBar);
        this->showKill(withKillButton);
        this->setHeader("Progress bar");
    }

    void COverlayMessages::setModeToTextMessage()
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_TextMessage);
        this->setHeader("Text message");
        this->showKill(false);
    }

    void COverlayMessages::setModeToOverlayTextMessage()
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_OverlayTextMessage);
        this->setHeader("Text message");
        this->showKill(false);
    }

    void COverlayMessages::activateTextMessages(bool activate)
    {
        ui->comp_OverlayTextMessage->activate(activate, activate);
    }

    bool COverlayMessages::isTextMessagesActivated() const
    {
        return ui->comp_OverlayTextMessage->isActivated();
    }

    void COverlayMessages::setModeToImage()
    {
        ui->sw_StatusMessagesComponent->setCurrentWidget(ui->pg_Image);
        this->setHeader("Image");
        this->showKillButton(false);
    }

    void COverlayMessages::setConfirmationMessage(const QString &message)
    {
        if (message.isEmpty())
        {
            ui->fr_Confirmation->setVisible(false);
        }
        else
        {
            ui->fr_Confirmation->setVisible(true);
            ui->lbl_Confirmation->setText(message);
        }
    }

    void COverlayMessages::showOverlayMessagesWithConfirmation(const CStatusMessageList &messages, bool appendOldMessages, const QString &confirmationMessage, std::function<void()> okLambda, QMessageBox::StandardButton defaultButton, int timeOutMs)
    {
        if (this->hasPendingConfirmation())
        {
            // defer message
            QPointer<COverlayMessages> myself(this);
            m_pendingMessageCalls.push_back([=]() {
                if (!myself) { return; }
                this->showOverlayMessagesWithConfirmation(messages, appendOldMessages, confirmationMessage, okLambda, defaultButton, timeOutMs);
            });
            return;
        }
        this->setConfirmationMessage(confirmationMessage);
        this->setDefaultConfirmationButton(defaultButton);
        this->showOverlayMessages(messages, appendOldMessages, timeOutMs);
        m_awaitingConfirmation = true; // needs to be after showOverlayMessages
        m_okLambda = okLambda;
    }

    void COverlayMessages::clearOverlayMessages()
    {
        ui->tvp_StatusMessages->clear();
    }

    void COverlayMessages::setDefaultConfirmationButton(int button)
    {
        switch (button)
        {
        case QMessageBox::Ok:
            ui->pb_Cancel->setDefault(false);
            ui->pb_Cancel->setAutoDefault(false);
            ui->pb_Ok->setDefault(true);
            ui->pb_Ok->setAutoDefault(true);
            ui->pb_Ok->setFocus();
            m_lastConfirmation = QMessageBox::Ok;
            break;
        case QMessageBox::Cancel:
            ui->pb_Ok->setDefault(false);
            ui->pb_Ok->setAutoDefault(false);
            ui->pb_Cancel->setDefault(true);
            ui->pb_Cancel->setAutoDefault(true);
            ui->pb_Cancel->setFocus();
            m_lastConfirmation = QMessageBox::Cancel;
            break;
        default:
            break;
        }
    }

    bool COverlayMessages::hasPendingConfirmation() const
    {
        return m_awaitingConfirmation;
    }

    void COverlayMessages::setReducedInfo(bool reduced)
    {
        ui->form_StatusMessage->setReducedInfo(reduced);
        ui->form_StatusMessageSmall->setReducedInfo(reduced);
    }

    void COverlayMessages::addShadow(QColor color)
    {
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
        color.setAlpha(96);
        shadow->setColor(color);
        this->setGraphicsEffect(shadow);
    }

    void COverlayMessages::keyPressEvent(QKeyEvent *event)
    {
        if (!this->isVisible()) { QFrame::keyPressEvent(event); }
        if (event->key() == Qt::Key_Escape)
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
        ui->fr_Confirmation->setVisible(false);
        if (m_awaitingConfirmation)
        {
            emit this->confirmationCompleted();
        }
        else
        {
            m_lastConfirmation = QMessageBox::Cancel;
        }
        m_awaitingConfirmation = false;
        m_okLambda = nullptr;

        if (!m_pendingMessageCalls.isEmpty())
        {
            std::function<void()> f = m_pendingMessageCalls.constFirst();
            m_pendingMessageCalls.removeFirst();
            const QPointer<COverlayMessages> myself(this);
            QTimer::singleShot(500, this, [=] {
                if (!myself) { return; }
                if (!sGui || sGui->isShuttingDown()) { return; }
                f();
            });
        }
    }

    void COverlayMessages::display(int timeOutMs)
    {
        this->show();
        this->raise();
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
} // ns
