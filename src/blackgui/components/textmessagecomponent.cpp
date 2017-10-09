/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/corefacade.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/guiapplication.h"
#include "blackgui/textmessagetextedit.h"
#include "blackgui/views/textmessageview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/verify.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/sequence.h"
#include "ui_textmessagecomponent.h"

#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Settings;
using namespace BlackGui::Views;
using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CTextMessageComponent::CTextMessageComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CTextMessageComponent)
        {
            ui->setupUi(this);
            ui->tw_TextMessages->setCurrentIndex(0);
            ui->le_textMessages->setVisible(false);
            ui->tvp_TextMessagesAll->setResizeMode(CTextMessageView::ResizingAuto);

            bool c = connect(ui->le_textMessages, &QLineEdit::returnPressed, this, &CTextMessageComponent::ps_textMessageEntered);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CTextMessageComponent::ps_onChangedAircraftCockpit);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(this, &CTextMessageComponent::commandEntered, sApp->getCoreFacade(), &CCoreFacade::parseCommandLine);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            Q_UNUSED(c);
        }

        CTextMessageComponent::~CTextMessageComponent()
        { }

        bool CTextMessageComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            bool c = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            c = c && connect(this->getDockWidgetInfoArea(), &CDockWidgetInfoArea::widgetTopLevelChanged, this, &CTextMessageComponent::ps_topLevelChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            return c;
        }

        QWidget *CTextMessageComponent::getTabWidget(CTextMessageComponent::Tab tab) const
        {
            switch (tab)
            {
            case TextMessagesAll:
                return ui->tb_TextMessagesAll;
            case TextMessagesCom1:
                return ui->tb_TextMessagesCOM1;
            case TextMessagesCom2:
                return ui->tb_TextMessagesCOM2;
            case TextMessagesUnicom:
                return ui->tb_TextMessagesUnicom;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong index");
                break;
            }
            return nullptr;
        }

        void CTextMessageComponent::selectTabWidget(CTextMessageComponent::Tab tab)
        {
            QWidget *w = getTabWidget(tab);
            if (w)
            {
                ui->tw_TextMessages->setCurrentWidget(w);
            }
        }

        void CTextMessageComponent::displayTextMessage(const CTextMessageList &messages)
        {
            if (messages.isEmpty()) return;
            const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
            const CTextMessageSettings msgSettings(this->m_messageSettings.getThreadLocal());

            for (const CTextMessage &message : messages)
            {
                bool relevantForMe = false;

                // SELCAL
                if (message.isSelcalMessage() && ownAircraft.isSelcalSelected(message.getSelcalCode()))
                {
                    // this is SELCAL for me
                    if (sGui && sGui->getIContextAudio())
                    {
                        sGui->getIContextAudio()->playSelcalTone(message.getSelcalCode());
                    }

                    if (msgSettings.popupSelcalMessages())
                    {
                        emit this->displayInInfoWindow(CLogMessage(this).info("SELCAL received"), 3 * 1000);
                    }
                    continue;
                }

                // UNICOM
                if (message.isSendToUnicom())
                {
                    ui->tep_TextMessagesUnicom->insertTextMessage(message);
                    relevantForMe = true;
                }

                // check message
                if (message.isRadioMessage())
                {
                    // check for own COM frequencies
                    if (message.isSendToFrequency(ownAircraft.getCom1System().getFrequencyActive()))
                    {
                        ui->tep_TextMessagesCOM1->insertTextMessage(message);
                        relevantForMe = true;
                    }
                    if (message.isSendToFrequency(ownAircraft.getCom2System().getFrequencyActive()))
                    {
                        ui->tep_TextMessagesCOM2->insertTextMessage(message);
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
                    ui->tvp_TextMessagesAll->insert(message);
                }
                if (!relevantForMe) { return; }

                // overlay message if this channel is not selected
                if (!message.wasSent() && !message.isSendToUnicom() && !message.isServerMessage())
                {
                    // if the channel is selected, do nothing
                    if (!this->isCorrespondingTextMessageTabSelected(message))
                    {
                        if (msgSettings.popup(message, ownAircraft))
                        {
                            emit this->displayInInfoWindow(CVariant::from(message), 5 * 1000);
                        }
                    }
                }
            }
        }

        CIdentifier CTextMessageComponent::componentIdentifier()
        {
            if (m_identifier.getName().isEmpty())
                m_identifier = CIdentifier(QStringLiteral("TEXTMESSAGECOMPONENT"));

            return m_identifier;
        }

        void CTextMessageComponent::ps_onChangedAircraftCockpit()
        {
            this->showCurrentFrequenciesFromCockpit();
        }

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
                if (!tab) { return false; }
                return ui->tw_TextMessages->currentWidget() == tab;
            }
            else
            {
                // frequency message
                const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
                if (ui->tw_TextMessages->currentWidget() == ui->tb_TextMessagesAll) { return true; }
                if (textMessage.isSendToFrequency(ownAircraft.getCom1System().getFrequencyActive()))
                {
                    return ui->tw_TextMessages->currentWidget() == ui->tb_TextMessagesCOM1;
                }
                if (textMessage.isSendToFrequency(ownAircraft.getCom2System().getFrequencyActive()))
                {
                    return ui->tw_TextMessages->currentWidget() == ui->tb_TextMessagesCOM2;
                }
                return false;
            }
        }

        bool CTextMessageComponent::isNetworkConnected() const
        {
            return sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected() ;
        }

        void CTextMessageComponent::showCurrentFrequenciesFromCockpit()
        {
            const CSimulatedAircraft ownAircraft = this->getOwnAircraft();
            QString f1n, f2n;
            f1n.sprintf("%03.3f", ownAircraft.getCom1System().getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3));
            f2n.sprintf("%03.3f", ownAircraft.getCom2System().getFrequencyActive().valueRounded(CFrequencyUnit::MHz(), 3));
            const QString f1 = QString("COM1: %1").arg(f1n);
            const QString f2 = QString("COM2: %1").arg(f2n);
            ui->tb_TextMessagesCOM1->setToolTip(f1);
            ui->tb_TextMessagesCOM1->setToolTip(f2);
            ui->tw_TextMessages->setTabText(ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM1), f1);
            ui->tw_TextMessages->setTabText(ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM2), f2);
        }

        QWidget *CTextMessageComponent::addNewTextMessageTab(const CCallsign &callsign)
        {
            Q_ASSERT(!callsign.isEmpty());
            return addNewTextMessageTab(callsign.asString());
        }

        QWidget *CTextMessageComponent::addNewTextMessageTab(const QString &tabName)
        {
            QWidget *newTab = new QWidget(this);
            QPushButton *closeButton = new QPushButton("Close", newTab);
            QVBoxLayout *layout = new QVBoxLayout(newTab);
            CTextMessageTextEdit *textEdit = new CTextMessageTextEdit(newTab);
            int marginLeft, marginRight, marginTop, marginBottom;
            ui->tb_TextMessagesAll->layout()->getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);
            newTab->layout()->setContentsMargins(marginLeft, marginTop, marginRight, 2);
            layout->addWidget(textEdit);
            layout->addWidget(closeButton);
            newTab->setLayout(layout);
            textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
            const int index = ui->tw_TextMessages->addTab(newTab, tabName);
            connect(closeButton, &QPushButton::released, this, &CTextMessageComponent::ps_closeTextMessageTab);
            ui->tw_TextMessages->setCurrentIndex(index);

            if (sGui && sGui->getIContextNetwork())
            {
                QString realName = sGui->getIContextNetwork()->getUserForCallsign(CCallsign(tabName)).getRealName();
                if (!realName.isEmpty()) ui->tw_TextMessages->setTabToolTip(index, realName);
            }
            return newTab;
        }

        void CTextMessageComponent::addPrivateChannelTextMessage(const CTextMessage &textMessage)
        {
            if (!textMessage.isPrivateMessage()) { return; }
            const CCallsign cs = textMessage.wasSent() ? textMessage.getRecipientCallsign() : textMessage.getSenderCallsign();
            if (cs.isEmpty()) { return; }
            QWidget *tab = this->findTextMessageTabByCallsign(cs);
            if (!tab) { tab = this->addNewTextMessageTab(cs); }
            Q_ASSERT_X(tab, Q_FUNC_INFO, "Missing tab");
            CTextMessageTextEdit *textEdit = tab->findChild<CTextMessageTextEdit *>();
            BLACK_VERIFY_X(textEdit, Q_FUNC_INFO, "Missing text edit");
            if (!textEdit) { return; } // do not crash, though this situation could not happen
            textEdit->insertTextMessage(textMessage);

            // sound
            if (sGui && sGui->getIContextAudio())
            {
                sGui->getIContextAudio()->playNotification(CNotificationSounds::NotificationTextMessagePrivate, true);
            }
        }

        CSimulatedAircraft CTextMessageComponent::getOwnAircraft() const
        {
            Q_ASSERT(sGui && sGui->getIContextOwnAircraft());
            return sGui->getIContextOwnAircraft()->getOwnAircraft();
        }

        QWidget *CTextMessageComponent::findTextMessageTabByCallsign(const CCallsign &callsign, bool callsignResolution) const
        {
            QWidget *w = findTextMessageTabByName(callsign.asString());
            if (w) { return w; }
            if (!callsignResolution) { return nullptr; }

            // resolve callsign
            const CAtcStation station(sGui->getIContextNetwork()->getOnlineStationForCallsign(callsign));
            if (!station.getCallsign().isEmpty())
            {
                const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
                if (ownAircraft.getCom1System().isActiveFrequencyWithin25kHzChannel(station.getFrequency()))
                {
                    return getTabWidget(TextMessagesCom1);
                }
                else if (ownAircraft.getCom2System().isActiveFrequencyWithin25kHzChannel(station.getFrequency()))
                {
                    return getTabWidget(TextMessagesCom2);
                }
            }
            return nullptr;
        }

        QWidget *CTextMessageComponent::findTextMessageTabByName(const QString &name) const
        {
            if (name.isEmpty()) { return nullptr; }
            const QString n = name.trimmed();
            for (int index = 0; index < ui->tw_TextMessages->count(); index++)
            {
                QString tabName = ui->tw_TextMessages->tabText(index);
                if (tabName.indexOf(n, 0, Qt::CaseInsensitive) < 0) { continue; }
                QWidget *tab = ui->tw_TextMessages->widget(index);
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
                index =  ui->tw_TextMessages->indexOf(parentWidget);
                parentWidget = parentWidget->parentWidget();
            }
            if (index >= 0) { ui->tw_TextMessages->removeTab(index); }
        }

        void CTextMessageComponent::ps_topLevelChanged(QWidget *widget, bool topLevel)
        {
            // own input field if floating window
            Q_UNUSED(widget);
            ui->le_textMessages->setVisible(topLevel);
        }

        void CTextMessageComponent::ps_textMessageEntered()
        {
            if (!ui->le_textMessages->isVisible()) { return; }
            if (!this->isVisible()) { return; }

            const QString cl(ui->le_textMessages->text().trimmed().simplified());
            ui->le_textMessages->clear();
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
            emit commandEntered(cl, componentIdentifier());
        }

        QString CTextMessageComponent::textMessageToCommand(const QString &enteredLine)
        {
            // only if visible
            if (enteredLine.isEmpty()) { return ""; }

            const int index = ui->tw_TextMessages->currentIndex();
            QString cmd(".msg ");
            if (index < 0 || index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesAll))
            {
                CLogMessage(this).validationError("Incorrect message channel");
                return "";
            }
            else
            {
                if (index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM1))
                {
                    cmd.append(QString::number(this->getOwnAircraft().getCom1System().getFrequencyActive().valueRounded(3)));
                }
                else if (index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM2))
                {
                    cmd.append(QString::number(this->getOwnAircraft().getCom2System().getFrequencyActive().valueRounded(3)));
                }
                else if (index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesUnicom))
                {
                    cmd.append(QString::number(CPhysicalQuantitiesConstants::FrequencyUnicom().valueRounded(3)));
                }
                else
                {
                    // not a standard channel
                    bool isNumber;
                    const QString selectedTabText = ui->tw_TextMessages->tabText(index).trimmed();
                    const double frequency = selectedTabText.toDouble(&isNumber);
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
        }

        void CTextMessageComponent::onTextMessageReceived(const CTextMessageList &messages)
        {
            this->displayTextMessage(messages);
        }

        void CTextMessageComponent::onTextMessageSent(const CTextMessage &sentMessage)
        {
            this->displayTextMessage(sentMessage);
        }

        bool CTextMessageComponent::handleGlobalCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            if (originator == componentIdentifier()) { return false; }
            if (commandLine.isEmpty() || commandLine.startsWith(".")) { return false; }

            // non command input
            if (!this->isVisibleWidget()) { return false; } // invisble, do ignore
            this->handleEnteredTextMessage(commandLine); // handle as it was entered by own command line

            return false; // we never handle the message directly, but forward it
        }

        void CTextMessageComponent::showCorrespondingTab(const CCallsign &callsign)
        {
            if (callsign.isEmpty())
            {
                CLogMessage(this).warning("No callsign to display text message");
                return;
            }
            QWidget *w = findTextMessageTabByCallsign(callsign, true);
            if (!w && sGui->getIContextNetwork())
            {
                CSimulatedAircraft aircraft(sGui->getIContextNetwork()->getAircraftInRangeForCallsign(callsign));
                if (!aircraft.getCallsign().isEmpty())
                {
                    // we assume a private message
                    w = this->addNewTextMessageTab(aircraft.getCallsign());
                }
            }
            if (!w) { return; }
            ui->tw_TextMessages->setCurrentWidget(w);
            this->displayMyself();
        }

    } // namespace
} // namespace
