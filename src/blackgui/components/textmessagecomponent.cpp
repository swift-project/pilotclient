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
            connect(this->ui->le_textMessages, &QLineEdit::returnPressed, this, &CTextMessageComponent::ps_commandEntered);

            this->m_clearTextEditAction = new QAction("Clear", this);
            connect(this->m_clearTextEditAction, &QAction::triggered, this, &CTextMessageComponent::ps_clearTextEdit);

            ui->te_TextMessagesAll->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesAll, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::ps_showContextMenuForTextEdit);

            ui->te_TextMessagesUnicom->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesUnicom, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::ps_showContextMenuForTextEdit);

            ui->te_TextMessagesCOM1->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesCOM1, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::ps_showContextMenuForTextEdit);

            ui->te_TextMessagesCOM2->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->te_TextMessagesCOM2, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::ps_showContextMenuForTextEdit);
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
        void CTextMessageComponent::textMessagesReceived(const CTextMessageList &messages, bool sending)
        {
            if (messages.isEmpty()) return;
            foreach(CTextMessage message, messages)
            {
                bool relevantForMe = false;
                QString m = message.asString(true, false, "\t");

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
                if (relevantForMe || message.isServerMessage()) { this->ui->te_TextMessagesAll->append(m); }
                if (!relevantForMe) { return; }

                // overlay message if this channel is not selected
                if (!sending && !message.isSendToUnicom() && !message.isServerMessage())
                {
                    // if the channel is selected, do nothing
                    if (!this->isCorrespondingTextMessageTabSelected(message))
                    {
                        emit this->displayInInfoWindow(message.toCVariant(), 5 * 1000);
                    }
                }
            }
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

            connect(this, &CTextMessageComponent::commandEntered, this->getIContextOwnAircraft(), &IContextOwnAircraft::parseCommandLine);
            connect(this, &CTextMessageComponent::commandEntered, this->getIContextNetwork(), &IContextNetwork::parseCommandLine);
            connect(this, &CTextMessageComponent::commandEntered, this->getIContextAudio(), &IContextAudio::parseCommandLine);
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
            connect(textEdit, &QTextEdit::customContextMenuRequested, this, &CTextMessageComponent::ps_showContextMenuForTextEdit);
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
            for (int index = 0; index < this->ui->tw_TextMessages->count(); index++)
            {
                QString tabName = this->ui->tw_TextMessages->tabText(index);
                if (tabName.indexOf(n, 0, Qt::CaseInsensitive) < 0) continue;
                QWidget *tab = this->ui->tw_TextMessages->widget(index);
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
            int index = this->ui->tw_TextMessages->currentIndex();
            if (index < 0 || index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesAll)) { return tm; }

            // from
            tm.setSenderCallsign(this->getOwnAircraft().getCallsign());

            // frequency text message?
            if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM1))
            {
                tm.setFrequency(this->getOwnAircraft().getCom1System().getFrequencyActive());
            }
            else if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM2))
            {
                tm.setFrequency(this->getOwnAircraft().getCom2System().getFrequencyActive());
            }
            else if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesUnicom))
            {
                tm.setFrequency(CPhysicalQuantitiesConstants::FrequencyUnicom());
            }
            else
            {
                // not a standard channel
                QString selectedTabText = this->ui->tw_TextMessages->tabText(index);
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

        /*
         * Show the context menu for text edit area
         */
        void CTextMessageComponent::ps_showContextMenuForTextEdit(const QPoint &pt)
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

        /*
         * Clear a text edit area
         */
        void CTextMessageComponent::ps_clearTextEdit()
        {
            if (!this->m_currentTextEdit) return;
            this->m_currentTextEdit->clear();
            this->m_currentTextEdit = nullptr;
        }

        /*
         * Top level was changed
         */
        void CTextMessageComponent::ps_topLevelChanged(QWidget *widget, bool topLevel)
        {
            Q_UNUSED(widget);
            this->ui->le_textMessages->setVisible(topLevel);
        }

        /*
         * Command line entered
         */
        void CTextMessageComponent::ps_commandEntered()
        {
            if (!this->ui->le_textMessages->isVisible()) { return; }
            const QString cl(this->ui->le_textMessages->text().trimmed().simplified());
            if (cl.isEmpty()) { return; }
            this->ui->le_textMessages->clear();

            bool handledMyself = this->parseCommandLine(cl);
            if (handledMyself) { return; }

            // not my business, relay
            emit commandEntered(cl);
        }

        /*
         * Command entered
         */
        bool CTextMessageComponent::parseCommandLine(const QString &commandLine)
        {
            static CSimpleCommandParser parser(
            {
                ".msg", ".m"
            });
            if (commandLine.isEmpty()) { return false; }
            parser.parse(commandLine);
            if (parser.isKnownCommand())
            {
                if (parser.matchesCommand(".msg", ".m"))
                {
                    if (!this->getIContextNetwork()->isConnected())
                    {
                        CLogMessage(this).error("network needs to be connected");
                        return false;
                    }
                    if (parser.countParts() < 3)
                    {
                        CLogMessage(this).error("incorrect message");
                        return false;
                    }
                    QString receiver = parser.part(1).trimmed(); // receiver

                    // select current tab by command
                    this->setVisible(true);
                    if (receiver == "c1" || receiver == "com1")
                    {
                        this->ui->tw_TextMessages->setCurrentWidget(this->ui->tb_TextMessagesCOM1);
                    }
                    else if (receiver == "c2" || receiver == "com2")
                    {
                        this->ui->tw_TextMessages->setCurrentWidget(this->ui->tb_TextMessagesCOM2);
                    }
                    else if (receiver == "u" || receiver == "unicom" || receiver == "uni")
                    {
                        this->ui->tw_TextMessages->setCurrentWidget(this->ui->tb_TextMessagesUnicom);
                    }
                    else
                    {
                        QWidget *tab = this->findTextMessageTabByName(receiver.trimmed());
                        if (tab == nullptr) tab = this->addNewTextMessageTab(receiver.trimmed().toUpper());
                        this->ui->tw_TextMessages->setCurrentWidget(tab);
                    }
                    CTextMessage tm = this->getTextMessageStubForChannel();
                    QString msg(parser.remainingStringAfter(2));
                    tm.setMessage(msg);
                    if (tm.isEmpty()) { return false; }
                    CTextMessageList tml(tm);
                    this->getIContextNetwork()->sendTextMessages(tml);
                    this->textMessagesReceived(tml, true);
                    return true;
                }
                else
                {
                    // no command
                    return false;
                }
            }
            else
            {
                // only if visible
                if (this->isVisible()) { return false; }

                // single line, no command
                // line is considered to be a message to the selected channel, send
                if (!this->isNetworkConnected())
                {
                    CLogMessage(this).error("network needs to be connected");
                    return false;
                }

                int index = this->ui->tw_TextMessages->currentIndex();
                if (index < 0 || index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesAll))
                {
                    CLogMessage(this).error("incorrect channel");
                }
                else
                {
                    CTextMessage tm = this->getTextMessageStubForChannel();
                    tm.setMessage(commandLine.simplified().trimmed());
                    if (tm.isEmpty()) { return true; }
                    CTextMessageList textMessageList(tm);
                    this->getIContextNetwork()->sendTextMessages(textMessageList);
                    this->textMessagesReceived(textMessageList, true);
                }
                return true;
            }
        }
    } // namespace
} // namespace
