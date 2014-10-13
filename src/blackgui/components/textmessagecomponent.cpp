/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "textmessagecomponent.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/notificationsounds.h"
#include "blackmisc/logmessage.h"
#include "ui_textmessagecomponent.h"

#include <QPushButton>
#include <QMenu>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Settings;

namespace BlackGui
{
    namespace Components
    {

        CTextMessageComponent::CTextMessageComponent(QWidget *parent) :
            QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CTextMessageComponent), m_selcalCallback(nullptr), m_clearTextEditAction(nullptr), m_currentTextEdit(nullptr)
        {
            ui->setupUi(this);
            this->m_clearTextEditAction = new QAction("Clear", this);
            connect(this->m_clearTextEditAction, &QAction::triggered, this, &CTextMessageComponent::clearTextEdit);

            ui->te_TextMessagesAll->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesAll, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::showContextMenuForTextEdit);

            ui->te_TextMessagesUnicom->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesUnicom, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::showContextMenuForTextEdit);

            ui->te_TextMessagesCOM1->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesCOM1, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::showContextMenuForTextEdit);

            ui->te_TextMessagesCOM2->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesCOM2, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::showContextMenuForTextEdit);
        }

        CTextMessageComponent::~CTextMessageComponent()
        {
            delete ui;
        }

        QWidget *CTextMessageComponent::getTab(CTextMessageComponent::Tab tab)
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
        void CTextMessageComponent::appendTextMessagesToGui(const CTextMessageList &messages, bool sending)
        {
            if (messages.isEmpty()) return;
            foreach(CTextMessage message, messages)
            {
                const QString currentSelcal = this->m_selcalCallback ? this->m_selcalCallback() : "";
                if (CSelcal::isValidCode(currentSelcal) && message.isSelcalMessageFor(currentSelcal))
                {
                    if (this->getOwnAircraft().isActiveFrequencyWithin25kHzChannel(message.getFrequency()))
                    {
                        // this is SELCAL for me
                        if (this->getIContextAudio())
                        {
                            this->getIContextAudio()->playSelcalTone(currentSelcal);
                        }
                        else
                        {
                            emit this->displayInInfoWindow(CLogMessage(this).info("SELCAL received"), 3 * 1000);
                        }
                    }
                    continue; // not displayed
                }

                bool relevantForMe = false;
                QString m = message.asString(true, false, "\t");

                if (message.isSendToUnicom())
                {
                    this->ui->te_TextMessagesUnicom->append(m);
                    relevantForMe = true;
                }

                // check message
                if (message.isRadioMessage())
                {
                    // check for own COM frequencies
                    if (message.isSendToFrequency(this->getOwnAircraft().getCom1System().getFrequencyActive()))
                    {
                        this->ui->te_TextMessagesCOM1->append(m);
                        relevantForMe = true;
                    }
                    if (message.isSendToFrequency(this->getOwnAircraft().getCom2System().getFrequencyActive()))
                    {
                        this->ui->te_TextMessagesCOM2->append(m);
                        relevantForMe = true;
                    }
                }
                else if (message.isPrivateMessage() && !message.isServerMessage())
                {
                    // private message
                    this->addPrivateChannelTextMessage(message, sending);
                    relevantForMe = true;
                }

                // message for me? right frequency? otherwise quit
                if (relevantForMe || message.isServerMessage()) this->ui->te_TextMessagesAll->append(m);
                if (!relevantForMe) return;

                // overlay message if this channel is not selected
                if (!sending && !message.isSendToUnicom() && !message.isServerMessage())
                {
                    // if the channel is selected, do nothing
                    if (!this->isCorrespondingTextMessageTabSelected(message))
                        emit this->displayInInfoWindow(message.toCVariant(), 5 * 1000);
                }
            }
        }

        void CTextMessageComponent::changedAircraftCockpit()
        {
            this->showCurrentFrequenciesFromCockpit();
        }

        void CTextMessageComponent::runtimeHasBeenSet()
        {
            connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CTextMessageComponent::changedAircraftCockpit);
        }

        /*
         * Is the tab of the message's receiver selected?
         */
        bool CTextMessageComponent::isCorrespondingTextMessageTabSelected(CTextMessage textMessage) const
        {
            if (!this->isVisible()) return false;
            if (!textMessage.hasValidRecipient()) return false;
            if (textMessage.isEmpty()) return false; // ignore empty message
            if (textMessage.isPrivateMessage())
            {
                // private message
                CCallsign cs = textMessage.getSenderCallsign();
                if (cs.isEmpty()) return false;
                QWidget *tab = this->findTextMessageTabByName(cs.getStringAsSet());
                if (!tab) return false;
                return this->currentWidget() == tab;
            }
            else
            {
                // frequency message
                const CAircraft ownAircraft = this->getOwnAircraft();
                if (this->currentWidget() == this->ui->tb_TextMessagesAll) return true;
                if (textMessage.isSendToFrequency(ownAircraft.getCom1System().getFrequencyActive()))
                    return this->currentWidget() == this->ui->tb_TextMessagesCOM1;
                if (textMessage.isSendToFrequency(ownAircraft.getCom2System().getFrequencyActive()))
                    return this->currentWidget() == this->ui->tb_TextMessagesCOM2;
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
            this->setTabText(this->indexOf(this->ui->tb_TextMessagesCOM1), f1);
            this->setTabText(this->indexOf(this->ui->tb_TextMessagesCOM2), f2);
        }

        /*
         * Add new text message tab
         */
        QWidget *CTextMessageComponent::addNewTextMessageTab(const QString &tabName)
        {
            QWidget *newTab = new QWidget(this);
            QPushButton *closeButton = new QPushButton("Close", newTab);
            QVBoxLayout *layout = new QVBoxLayout(newTab);
            QTextEdit *textEdit = new QTextEdit(newTab);
            int marginLeft, marginRight, marginTop, marginBottom;
            this->ui->tb_TextMessagesAll->layout()->getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);
            newTab->layout()->setContentsMargins(marginLeft, marginTop, marginRight, 2);
            textEdit->setReadOnly(true);
            textEdit->setWordWrapMode(QTextOption::NoWrap);
            layout->addWidget(textEdit);
            layout->addWidget(closeButton);
            newTab->setLayout(layout);
            textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(textEdit, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::showContextMenuForTextEdit);
            int index = this->addTab(newTab, tabName);
            this->connect(closeButton, &QPushButton::released, this, &CTextMessageComponent::closeTextMessageTab);
            this->setCurrentIndex(index);

            if (this->getIContextNetwork())
            {
                QString realName = this->getIContextNetwork()->getUserForCallsign(CCallsign(tabName)).getRealName();
                if (!realName.isEmpty()) this->setTabToolTip(index, realName);
            }
            return newTab;
        }

        /*
         * Add a private channel text message
         */
        void CTextMessageComponent::addPrivateChannelTextMessage(const CTextMessage &textMessage, bool sending)
        {
            if (!textMessage.isPrivateMessage()) return;
            CCallsign cs = sending ? textMessage.getRecipientCallsign() : textMessage.getSenderCallsign();
            if (cs.isEmpty()) return;
            QWidget *tab = this->findTextMessageTabByName(cs.getStringAsSet());
            if (tab == nullptr) tab = this->findTextMessageTabByName(cs.asString());
            if (tab == nullptr) tab = this->addNewTextMessageTab(cs.getStringAsSet());
            Q_ASSERT(tab != nullptr);
            QTextEdit *textEdit = tab->findChild<QTextEdit *>();
            Q_ASSERT(textEdit != nullptr);
            if (textEdit == nullptr) return; // do not crash, though this situation could not happen
            textEdit->append(textMessage.asString(true, false, "\t"));

            // sound
            if (this->getIContextAudio())
                this->getIContextAudio()->playNotification(BlackSound::CNotificationSounds::NotificationTextMessagePrivate, true);
        }

        /*
         * Message tab by name
         */
        QWidget *CTextMessageComponent::findTextMessageTabByName(const QString &name) const
        {
            if (name.isEmpty()) return nullptr;
            QString n = name.trimmed();
            for (int index = 0; index < this->count(); index++)
            {
                QString tabName = this->tabText(index);
                if (tabName.indexOf(n, 0, Qt::CaseInsensitive) < 0) continue;
                QWidget *tab = this->widget(index);
                return tab;
            }
            return nullptr;
        }

        /*
         * Text message stub (sender/receiver) for current channel
         */
        CTextMessage CTextMessageComponent::getTextMessageStubForChannel()
        {
            CTextMessage tm;
            int index = this->currentIndex();
            if (index < 0) return tm;
            if (index == this->indexOf(this->ui->tb_TextMessagesAll)) return tm;

            // from
            tm.setSenderCallsign(this->getOwnAircraft().getCallsign());

            // frequency text message?
            if (index == this->indexOf(this->ui->tb_TextMessagesCOM1))
            {
                tm.setFrequency(this->getOwnAircraft().getCom1System().getFrequencyActive());
            }
            else if (index == this->indexOf(this->ui->tb_TextMessagesCOM2))
            {
                tm.setFrequency(this->getOwnAircraft().getCom2System().getFrequencyActive());
            }
            else if (index == this->indexOf(this->ui->tb_TextMessagesUnicom))
            {
                tm.setFrequency(CPhysicalQuantitiesConstants::FrequencyUnicom());
            }
            else
            {
                // not a standard channel
                QString selectedTabText = this->tabText(index);
                bool isNumber;
                double frequency = selectedTabText.toDouble(&isNumber);
                if (isNumber)
                {
                    CFrequency radioFrequency = CFrequency(frequency, CFrequencyUnit::MHz());
                    if (CComSystem::isValidCivilAviationFrequency(radioFrequency))
                    {
                        tm.setFrequency(radioFrequency);
                    }
                    else
                    {
                        CCallsign toCallsign(selectedTabText);
                        tm.setRecipientCallsign(toCallsign);
                    }
                }
                else
                {
                    CCallsign toCallsign(selectedTabText);
                    tm.setRecipientCallsign(toCallsign);
                }
            }
            return tm; // now valid message stub with receiver
        }

        /*
         * Close message tab
         */
        void CTextMessageComponent::closeTextMessageTab()
        {
            QObject *sender = QObject::sender();
            QWidget *parentWidget = qobject_cast<QWidget *>(sender->parent());
            Q_ASSERT(parentWidget);
            int index = -1;

            while (index < 0 && parentWidget)
            {
                index =  this->indexOf(parentWidget);
                parentWidget = parentWidget->parentWidget();
            }
            if (index >= 0) this->removeTab(index);
        }

        void CTextMessageComponent::showContextMenuForTextEdit(const QPoint &pt)
        {
            QObject *sender = QObject::sender();
            this->m_currentTextEdit = qobject_cast<QTextEdit *>(sender);
            Q_ASSERT(this->m_currentTextEdit);

            QMenu *menu = this->m_currentTextEdit->createStandardContextMenu();
            menu->setParent(this->m_currentTextEdit);
            menu->setObjectName(this->m_currentTextEdit->objectName().append("_contextMenu"));
            menu->addSeparator();
            menu->addAction(this->m_clearTextEditAction);
            menu->exec(this->m_currentTextEdit->mapToGlobal(pt));
            delete menu;
        }

        void CTextMessageComponent::clearTextEdit()
        {
            if (!this->m_currentTextEdit) return;
            this->m_currentTextEdit->clear();
            this->m_currentTextEdit = nullptr;
        }

        /*
         * Command entered
         */
        void CTextMessageComponent::commandEntered()
        {
            // TODO: just a first draft of the command line parser
            // needs to be refactored, as soon as a first version works

            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
            Q_ASSERT(lineEdit);

            QString cmdLine = lineEdit->text().simplified();
            if (cmdLine.isEmpty()) return;
            QList<QString> parts = cmdLine.toLower().split(' ');
            if (parts.length() < 1) return;
            QString cmd = parts[0].startsWith('.') ? parts[0].toLower() : "";
            if (cmd == ".m" || cmd == ".msg")
            {
                if (!this->getIContextNetwork() || !this->getIContextNetwork()->isConnected())
                {
                    CLogMessage(this).error("network needs to be connected");
                    return;
                }
                if (parts.length() < 3)
                {
                    CLogMessage(this).error("incorrect message");
                    return;
                }
                QString p = parts[1].trimmed(); // receiver

                // select current tab by command
                this->setVisible(true);
                if (p == "c1" || p == "com1")
                {
                    this->setCurrentWidget(this->ui->tb_TextMessagesCOM1);
                }
                else if (p == "c2" || p == "com2")
                {
                    this->setCurrentWidget(this->ui->tb_TextMessagesCOM2);
                }
                else if (p == "u" || p == "unicom" || p == "uni")
                {
                    this->setCurrentWidget(this->ui->tb_TextMessagesUnicom);
                }
                else
                {
                    QWidget *tab = this->findTextMessageTabByName(p.trimmed());
                    if (tab == nullptr) tab = this->addNewTextMessageTab(p.trimmed().toUpper());
                    this->setCurrentWidget(tab);
                }
                CTextMessage tm = this->getTextMessageStubForChannel();
                int index = cmdLine.indexOf(tm.getRecipientCallsign().getStringAsSet(), 0, Qt::CaseInsensitive);
                if (index < 0)
                {
                    CLogMessage(this).error("incomplete message");
                    return;
                }
                QString msg(cmdLine.mid(index + tm.getRecipientCallsign().asString().length() + 1));
                tm.setMessage(msg);
                if (tm.isEmpty()) return;
                if (!this->isNetworkConnected()) return;
                CTextMessageList tml(tm);
                this->getIContextNetwork()->sendTextMessages(tml);
                this->appendTextMessagesToGui(tml, true);
                lineEdit->setText("");
            }
            else if (cmd.startsWith("."))
            {
                // dump CMDs
            }
            else
            {
                // single line, no command
                // line is considered to be a message to the selected channel, send
                if (!this->isNetworkConnected())
                {
                    CLogMessage(this).error("network needs to be connected");
                    return;
                }

                if (!this->isVisible())
                {
                    CLogMessage(this).error("text messages can only be sent from corresponding page");
                    return;
                }

                int index = this->currentIndex();
                if (index < 0 || index == this->indexOf(this->ui->tb_TextMessagesAll))
                {
                    CLogMessage(this).error("incorrect channel");
                }
                else
                {
                    CTextMessage tm = this->getTextMessageStubForChannel();
                    tm.setMessage(cmdLine);
                    if (tm.isEmpty()) return;
                    if (!this->getIContextNetwork()) return;
                    CTextMessageList textMessageList(tm);
                    this->getIContextNetwork()->sendTextMessages(textMessageList);
                    this->appendTextMessagesToGui(textMessageList, true);
                    lineEdit->setText("");
                }
            }
        }
    }
}

