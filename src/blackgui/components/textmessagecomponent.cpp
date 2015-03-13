/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "textmessagecomponent.h"
#include "ui_textmessagecomponent.h"
#include "blackcore/context_audio.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/notificationsounds.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simplecommandparser.h"

#include <QPushButton>
#include <QMenu>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Settings;

namespace BlackGui
{
    namespace Components
    {

        CTextMessageComponent::CTextMessageComponent(QWidget *parent) :
            QFrame(parent),
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CTextMessageComponent)
        {
            ui->setupUi(this);

            this->ui->le_textMessages->setVisible(false);
            connect(this->ui->le_textMessages, &QLineEdit::returnPressed, this, &CTextMessageComponent::ps_textMessageEntered);

            this->ui->tvp_TextMessagesAll->rowsResizeModeToContent();
            this->ui->tvp_TextMessagesAll->setResizeMode(CTextMessageView::ResizingAuto);
        }

        CTextMessageComponent::~CTextMessageComponent()
        { }

        QWidget *CTextMessageComponent::getTabWidget(CTextMessageComponent::Tab tab)
        {
            switch (tab)
            {
            case TextMessagesAll:
                return this->ui->tb_TextMessagesAll;
            case TextMessagesCom1:
                return this->ui->tb_TextMessagesCOM1;
            case TextMessagesCom2:
                return this->ui->tb_TextMessagesCOM2;
            case TextMessagesUnicom:
                return this->ui->tb_TextMessagesUnicom;
            default:
                qFatal("Wrong index");
                break;
            }
            return nullptr;
        }

        /*
         * Text messages received or send, append to GUI
         */
        void CTextMessageComponent::displayTextMessage(const CTextMessageList &messages)
        {
            if (messages.isEmpty()) return;
            foreach(CTextMessage message, messages)
            {
                bool relevantForMe = false;

                // SELCAL
                if (message.isSelcalMessage() && getOwnAircraft().isSelcalSelected(message.getSelcalCode()))
                {
                    // this is SELCAL for me
                    if (this->getIContextAudio())
                    {
                        this->getIContextAudio()->playSelcalTone(message.getSelcalCode());
                    }
                    else
                    {
                        emit this->displayInInfoWindow(CLogMessage(this).info("SELCAL received"), 3 * 1000);
                    }
                    continue;
                }

                // UNICOM
                if (message.isSendToUnicom())
                {
                    this->ui->tep_TextMessagesUnicom->insertTextMessage(message);
                    relevantForMe = true;
                }

                // check message
                if (message.isRadioMessage())
                {
                    // check for own COM frequencies
                    if (message.isSendToFrequency(this->getOwnAircraft().getCom1System().getFrequencyActive()))
                    {
                        this->ui->tep_TextMessagesCOM1->insertTextMessage(message);
                        relevantForMe = true;
                    }
                    if (message.isSendToFrequency(this->getOwnAircraft().getCom2System().getFrequencyActive()))
                    {
                        this->ui->tep_TextMessagesCOM2->insertTextMessage(message);
                        relevantForMe = true;
                    }
                }
                else if (message.isPrivateMessage() && !message.isServerMessage())
                {
                    // private message
                    this->addPrivateChannelTextMessage(message);
                    relevantForMe = true;
                }

                // message for me? right frequency? otherwise quit
                if (relevantForMe || message.isServerMessage())
                {
                    this->ui->tvp_TextMessagesAll->insert(message);
                }
                if (!relevantForMe) { return; }

                // overlay message if this channel is not selected
                if (!message.wasSent() && !message.isSendToUnicom() && !message.isServerMessage())
                {
                    // if the channel is selected, do nothing
                    if (!this->isCorrespondingTextMessageTabSelected(message))
                    {
                        emit this->displayInInfoWindow(message.toCVariant(), 5 * 1000);
                    }
                }
            }
        }

        const QString &CTextMessageComponent::componentOriginator()
        {
            // string is generated once, the timestamp allows to use multiple
            // components (as long as they are not generated at the same ms)
            static const QString o = QString("TEXTMESSAGECOMPONENT:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
            return o;
        }

        void CTextMessageComponent::ps_onChangedAircraftCockpit()
        {
            this->showCurrentFrequenciesFromCockpit();
        }

        void CTextMessageComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextAudio());

            connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CTextMessageComponent::ps_onChangedAircraftCockpit);
            connect(this->getDockWidgetInfoArea(), &CDockWidgetInfoArea::widgetTopLevelChanged, this, &CTextMessageComponent::ps_topLevelChanged);
            connect(this, &CTextMessageComponent::commandEntered, this->getRuntime(), &CRuntime::parseCommandLine);
        }

        /*
         * Is the tab of the message's receiver selected?
         */
        bool CTextMessageComponent::isCorrespondingTextMessageTabSelected(CTextMessage textMessage) const
        {
            if (!this->isVisibleWidget()) { return false; }
            if (!textMessage.hasValidRecipient()) { return false; }
            if (textMessage.isEmpty()) { return false; } // ignore empty message
            if (textMessage.isPrivateMessage())
            {
                // private message
                CCallsign cs = textMessage.getSenderCallsign();
                if (cs.isEmpty()) return false;
                QWidget *tab = this->findTextMessageTabByName(cs.getStringAsSet());
                if (!tab) return false;
                return this->ui->tw_TextMessages->currentWidget() == tab;
            }
            else
            {
                // frequency message
                const CAircraft ownAircraft = this->getOwnAircraft();
                if (this->ui->tw_TextMessages->currentWidget() == this->ui->tb_TextMessagesAll) { return true; }
                if (textMessage.isSendToFrequency(ownAircraft.getCom1System().getFrequencyActive()))
                {
                    return this->ui->tw_TextMessages->currentWidget() == this->ui->tb_TextMessagesCOM1;
                }
                if (textMessage.isSendToFrequency(ownAircraft.getCom2System().getFrequencyActive()))
                {
                    return this->ui->tw_TextMessages->currentWidget() == this->ui->tb_TextMessagesCOM2;
                }
                return false;
            }
        }

        void CTextMessageComponent::showCurrentFrequenciesFromCockpit()
        {
            const CAircraft ownAircraft = this->getOwnAircraft();
            QString f1n, f2n;
            f1n.sprintf("%03.3f", ownAircraft.getCom1System().getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3));
            f2n.sprintf("%03.3f", ownAircraft.getCom2System().getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3));
            const QString f1 = QString("COM1: %1").arg(f1n);
            const QString f2 = QString("COM2: %1").arg(f2n);
            this->ui->tb_TextMessagesCOM1->setToolTip(f1);
            this->ui->tb_TextMessagesCOM1->setToolTip(f2);
            this->ui->tw_TextMessages->setTabText(this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM1), f1);
            this->ui->tw_TextMessages->setTabText(this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM2), f2);
        }

        QWidget *CTextMessageComponent::addNewTextMessageTab(const QString &tabName)
        {
            QWidget *newTab = new QWidget(this);
            QPushButton *closeButton = new QPushButton("Close", newTab);
            QVBoxLayout *layout = new QVBoxLayout(newTab);
            CTextMessageTextEdit *textEdit = new CTextMessageTextEdit(newTab);
            int marginLeft, marginRight, marginTop, marginBottom;
            this->ui->tb_TextMessagesAll->layout()->getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);
            newTab->layout()->setContentsMargins(marginLeft, marginTop, marginRight, 2);
            layout->addWidget(textEdit);
            layout->addWidget(closeButton);
            newTab->setLayout(layout);
            textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
            int index = this->ui->tw_TextMessages->addTab(newTab, tabName);
            this->connect(closeButton, &QPushButton::released, this, &CTextMessageComponent::ps_closeTextMessageTab);
            this->ui->tw_TextMessages->setCurrentIndex(index);

            if (this->getIContextNetwork())
            {
                QString realName = this->getIContextNetwork()->getUserForCallsign(CCallsign(tabName)).getRealName();
                if (!realName.isEmpty()) this->ui->tw_TextMessages->setTabToolTip(index, realName);
            }
            return newTab;
        }

        void CTextMessageComponent::addPrivateChannelTextMessage(const CTextMessage &textMessage)
        {
            if (!textMessage.isPrivateMessage()) { return; }
            CCallsign cs = textMessage.getRecipientCallsign();
            if (cs.isEmpty()) { return; }
            QString csStr(cs.asString());
            QWidget *tab = this->findTextMessageTabByName(csStr);
            if (tab == nullptr) { tab = this->findTextMessageTabByName(csStr); }
            if (tab == nullptr) { tab = this->addNewTextMessageTab(csStr); }
            Q_ASSERT(tab != nullptr);
            CTextMessageTextEdit *textEdit = tab->findChild<CTextMessageTextEdit *>();
            Q_ASSERT(textEdit != nullptr);
            if (textEdit == nullptr) { return; } // do not crash, though this situation could not happen
            textEdit->insertTextMessage(textMessage);

            // sound
            if (this->getIContextAudio())
            {
                this->getIContextAudio()->playNotification(BlackSound::CNotificationSounds::NotificationTextMessagePrivate, true);
            }
        }

        QWidget *CTextMessageComponent::findTextMessageTabByName(const QString &name) const
        {
            if (name.isEmpty()) { return nullptr; }
            QString n = name.trimmed();
            for (int index = 0; index < this->ui->tw_TextMessages->count(); index++)
            {
                QString tabName = this->ui->tw_TextMessages->tabText(index);
                if (tabName.indexOf(n, 0, Qt::CaseInsensitive) < 0) { continue; }
                QWidget *tab = this->ui->tw_TextMessages->widget(index);
                return tab;
            }
            return nullptr;
        }

        void CTextMessageComponent::ps_closeTextMessageTab()
        {
            QObject *sender = QObject::sender();
            QWidget *parentWidget = qobject_cast<QWidget *>(sender->parent());
            Q_ASSERT(parentWidget);
            int index = -1;

            while (index < 0 && parentWidget)
            {
                index =  this->ui->tw_TextMessages->indexOf(parentWidget);
                parentWidget = parentWidget->parentWidget();
            }
            if (index >= 0) { this->ui->tw_TextMessages->removeTab(index); }
        }

        void CTextMessageComponent::ps_topLevelChanged(QWidget *widget, bool topLevel)
        {
            // own input field if floating window
            Q_UNUSED(widget);
            this->ui->le_textMessages->setVisible(topLevel);
        }

        void CTextMessageComponent::ps_textMessageEntered()
        {
            if (!this->ui->le_textMessages->isVisible()) { return; }
            if (!this->isVisible()) { return; }

            QString cl(this->ui->le_textMessages->text().trimmed().simplified());
            this->handleEnteredTextMessage(cl);
        }

        void CTextMessageComponent::handleEnteredTextMessage(const QString &textMessage)
        {
            if (!this->isVisibleWidget()) { return; }

            QString cl(textMessage.trimmed().simplified());
            if (cl.isEmpty()) { return; }

            // is this a command?
            if (!cl.startsWith("."))
            {
                // build a command line
                cl = this->textMessageToCommand(cl);
            }

            // relay the command
            if (cl.isEmpty()) { return; }
            emit commandEntered(cl, componentOriginator());
        }

        QString CTextMessageComponent::textMessageToCommand(const QString &enteredLine)
        {
            // only if visible
            if (enteredLine.isEmpty()) { return ""; }

            int index = this->ui->tw_TextMessages->currentIndex();
            QString cmd(".msg ");
            if (index < 0 || index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesAll))
            {
                CLogMessage(this).validationError("Incorrect message channel");
                return "";
            }
            else
            {
                if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM1))
                {
                    cmd.append(QString::number(this->getOwnAircraft().getCom1System().getFrequencyActive().valueRounded(3)));
                }
                else if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM2))
                {
                    cmd.append(QString::number(this->getOwnAircraft().getCom2System().getFrequencyActive().valueRounded(3)));
                }
                else if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesUnicom))
                {
                    cmd.append(QString::number(CPhysicalQuantitiesConstants::FrequencyUnicom().valueRounded(3)));
                }
                else
                {
                    // not a standard channel
                    QString selectedTabText = this->ui->tw_TextMessages->tabText(index).trimmed();
                    bool isNumber;
                    double frequency = selectedTabText.toDouble(&isNumber);
                    if (isNumber)
                    {
                        CFrequency radioFrequency = CFrequency(frequency, CFrequencyUnit::MHz());
                        if (CComSystem::isValidCivilAviationFrequency(radioFrequency))
                        {
                            cmd.append(QString::number(radioFrequency.valueRounded(3)));
                        }
                        else
                        {
                            cmd.append(selectedTabText);
                        }
                    }
                    else
                    {
                        cmd.append(selectedTabText);
                    }
                }
                cmd.append(" ").append(enteredLine);
                return cmd;
            }
            return "";
        }

        void CTextMessageComponent::onTextMessageReceived(const CTextMessageList &messages)
        {
            this->displayTextMessage(messages);
        }

        void CTextMessageComponent::onTextMessageSent(const CTextMessage &sentMessage)
        {
            this->displayTextMessage(CTextMessageList({ sentMessage }));
        }

        bool CTextMessageComponent::handleGlobalCommandLine(const QString &commandLine, const QString &originator)
        {
            if (originator == componentOriginator()) { return false; }
            if (commandLine.isEmpty() || commandLine.startsWith(".")) { return false; }

            // non command input
            if (!this->isVisibleWidget()) { return false; } // invisble, do ignore
            this->handleEnteredTextMessage(commandLine); // handle as it was entered by own command line

            return false; // we never handle the message directly, but forward it
        }

    } // namespace
} // namespace
