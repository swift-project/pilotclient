/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_textmessagecomponent.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/views/textmessageview.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/guiapplication.h"
#include "blackgui/textmessagetextedit.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/application.h"
#include "blackcore/corefacade.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/user.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/sequence.h"
#include "blackmisc/verify.h"

#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
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
            ui->lep_textMessages->setVisible(false);
            ui->tvp_TextMessagesAll->setResizeMode(CTextMessageView::ResizingAuto);
            ui->comp_AtcStations->setWithIcons(false);

            // lep_textMessages is the own line edit
            bool c = connect(ui->lep_textMessages, &CLineEditHistory::returnPressed, this, &CTextMessageComponent::textMessageEntered);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(ui->gb_Settings, &QGroupBox::toggled, this, &CTextMessageComponent::onSettingsChecked);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(ui->gb_MessageTo, &QGroupBox::toggled, this, &CTextMessageComponent::onMessageToChecked);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

            c = connect(ui->comp_AtcStations, &CAtcButtonComponent::requestAtcStation, this, &CTextMessageComponent::onAtcButtonClicked);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

            // style sheet
            c = connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CTextMessageComponent::onStyleSheetChanged, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(ui->comp_SettingsStyle, &CSettingsTextMessageStyle::changed, this, &CTextMessageComponent::updateSettings, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

            if (sGui && sGui->getCoreFacade() && sGui->getIContextNetwork() && sGui->getIContextOwnAircraft())
            {
                c = connect(this, &CTextMessageComponent::commandEntered, sGui->getCoreFacade(), &CCoreFacade::parseCommandLine);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
                c = connect(sGui->getIContextNetwork(), &IContextNetwork::textMessagesReceived, this, &CTextMessageComponent::onTextMessageReceived, Qt::QueuedConnection);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
                c = connect(sGui->getIContextNetwork(), &IContextNetwork::textMessageSent, this, &CTextMessageComponent::onTextMessageSent, Qt::QueuedConnection);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
                c = connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CTextMessageComponent::onChangedAircraftCockpit);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            }
            Q_UNUSED(c);

            // init by settings
            const QPointer<CTextMessageComponent> myself(this);
            QTimer::singleShot(2000, this, [ = ]
            {
                // init decoupled when sub components are fully init
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                this->onSettingsChanged();
                this->onChangedAircraftCockpit();
            });
        }

        CTextMessageComponent::~CTextMessageComponent()
        { }

        bool CTextMessageComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            bool c = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            c = c && connect(this->getDockWidgetInfoArea(), &CDockWidgetInfoArea::widgetTopLevelChanged, this, &CTextMessageComponent::topLevelChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            return c;
        }

        QWidget *CTextMessageComponent::getTabWidget(TextMessageTab tab) const
        {
            switch (tab)
            {
            case TextMessagesAll:    return ui->tb_TextMessagesAll;
            case TextMessagesCom1:   return ui->tb_TextMessagesCOM1;
            case TextMessagesCom2:   return ui->tb_TextMessagesCOM2;
            case TextMessagesUnicom: return ui->tb_TextMessagesUnicom;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong index");
                break;
            }
            return nullptr;
        }

        CTextMessageTextEdit *CTextMessageComponent::getTextEdit(TextMessageTab tab) const
        {
            QWidget *w = this->getTabWidget(tab);
            if (!w) { return nullptr; }
            return this->findChild<CTextMessageTextEdit *>();
        }

        void CTextMessageComponent::selectTabWidget(TextMessageTab tab)
        {
            QWidget *w = this->getTabWidget(tab);
            if (w)
            {
                ui->tw_TextMessages->setCurrentWidget(w);
            }
        }

        void CTextMessageComponent::selectTabWidget(const CCallsign &callsign, bool addIfNotExisting)
        {
            QWidget *tab = this->findTextMessageTabByCallsign(callsign);
            if (!tab && addIfNotExisting) { tab = this->addNewTextMessageTab(callsign); }
            if (!tab) { return; }
            ui->tw_TextMessages->setCurrentWidget(tab);
        }

        bool CTextMessageComponent::isCloseableTab(const QWidget *tabWidget) const
        {
            if (!tabWidget) { return false; }
            return (tabWidget != ui->tb_TextMessagesAll  && tabWidget != ui->tb_TextMessagesCOM1 &&
                    tabWidget != ui->tb_TextMessagesCOM2 && tabWidget != ui->tb_TextMessagesUnicom);
        }

        void CTextMessageComponent::displayTextMessage(const CTextMessageList &messages)
        {
            if (messages.isEmpty()) { return; }
            if (!sGui || sGui->isShuttingDown()) { return; }
            const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
            const CTextMessageSettings msgSettings(m_messageSettings.getThreadLocal());
            const bool playNotification = sGui && sGui->getIContextAudio();
            const bool audioCsMentioned = playNotification && m_audioSettings.get().textCallsignMentioned();

            for (const CTextMessage &message : messages)
            {
                bool relevantForMe = false;

                // SELCAL
                if (!m_usedAsOverlayWidget && message.isSelcalMessage() && ownAircraft.isSelcalSelected(message.getSelcalCode()))
                {
                    // this is SELCAL for me
                    if (playNotification)
                    {
                        sGui->getIContextAudio()->playSelcalTone(message.getSelcalCode());
                    }

                    if (msgSettings.popupSelcalMessages())
                    {
                        this->emitDisplayInInfoWindow(CLogMessage(this).info(u"SELCAL received"), 3 * 1000);
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

                    // callsign mentioned notification
                    if (relevantForMe && audioCsMentioned && ownAircraft.hasCallsign() && message.mentionsCallsign(ownAircraft.getCallsign()))
                    {
                        sGui->getIContextAudio()->playNotification(CNotificationSounds::NotificationTextCallsignMentioned, false);
                    }
                }
                else if (message.isPrivateMessage() && !message.isServerMessage())
                {
                    // private message
                    this->addPrivateChannelTextMessage(message);
                    relevantForMe = true;
                }

                // message for me? right frequency? otherwise quit
                if (this->hasAllMessagesTab() && (relevantForMe || message.isServerMessage()))
                {
                    ui->tvp_TextMessagesAll->insert(message);
                }
                if (!relevantForMe) { continue; }

                // overlay message if this channel is not selected
                if (!message.wasSent() && !message.isSendToUnicom() && !message.isServerMessage())
                {
                    // if the channel is selected, do nothing
                    if (!this->isCorrespondingTextMessageTabSelected(message))
                    {
                        if (msgSettings.popup(message, ownAircraft))
                        {
                            this->emitDisplayInInfoWindow(CVariant::from(message), 15 * 1000);
                        }
                    }
                } // message
            } // for
        }

        void CTextMessageComponent::onChangedAircraftCockpit()
        {
            this->showCurrentFrequenciesFromCockpit();
        }

        void CTextMessageComponent::onSettingsChecked(bool checked)
        {
            ui->comp_SettingsOverlay->setVisible(checked);
            ui->comp_SettingsStyle->setVisible(checked);
            ui->gb_Settings->setFlat(!checked);
        }

        void CTextMessageComponent::onMessageToChecked(bool checked)
        {
            ui->comp_AtcStations->setVisible(checked);
            ui->gb_MessageTo->setFlat(!checked);
        }

        void CTextMessageComponent::onSettingsChanged()
        {
            QList<CTextMessageTextEdit *> textEdits = this->findAllTextEdit();
            const QString style = this->getStyleSheet();
            for (CTextMessageTextEdit *textEdit : textEdits)
            {
                textEdit->setStyleSheetForContent(style);
            }
            ui->comp_SettingsStyle->setStyle(this->getStyleSheet());
            this->update(); // refresh window
        }

        void CTextMessageComponent::onStyleSheetChanged()
        {
            this->onSettingsChanged();
        }

        void CTextMessageComponent::onAtcButtonClicked(const CAtcStation &station)
        {
            if (station.getCallsign().isEmpty()) { return; }
            this->addNewTextMessageTab(station.getCallsign());
        }

        void CTextMessageComponent::updateSettings()
        {
            const QString style = ui->comp_SettingsStyle->getStyle();
            CTextMessageSettings s = m_messageSettings.get();
            s.setStyleSheet(style);
            const CStatusMessage m = m_messageSettings.setAndSave(s);
            CLogMessage::preformatted(m);
            this->onStyleSheetChanged();
        }

        QList<CTextMessageTextEdit *> CTextMessageComponent::findAllTextEdit() const
        {
            return this->findChildren<CTextMessageTextEdit *>();
        }

        QString CTextMessageComponent::getStyleSheet() const
        {
            const QString styleSheet = m_messageSettings.get().getStyleSheet();
            return styleSheet.isEmpty() && sGui ?
                   sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameTextMessage()) :
                   styleSheet;
        }

        bool CTextMessageComponent::isCorrespondingTextMessageTabSelected(const CTextMessage &textMessage) const
        {
            if (!this->isVisibleWidgetHack()) { return false; }
            if (!textMessage.hasValidRecipient()) { return false; }
            if (textMessage.isEmpty()) { return false; } // ignore empty message
            if (textMessage.isPrivateMessage())
            {
                // private message
                const CCallsign cs = textMessage.getSenderCallsign();
                if (cs.isEmpty()) { return false; }
                const QWidget *tab = this->findTextMessageTabByCallsign(cs, false);
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
            return sGui && sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected();
        }

        void CTextMessageComponent::showCurrentFrequenciesFromCockpit()
        {
            const CSimulatedAircraft ownAircraft = this->getOwnAircraft();

            const CFrequency freq1 = ownAircraft.getCom1System().getFrequencyActive();
            const CFrequency freq2 = ownAircraft.getCom2System().getFrequencyActive();

            CAtcStationList f1Stations;
            CAtcStationList f2Stations;
            if (sGui && sGui->getIContextNetwork())
            {
                const CComSystem::ChannelSpacing spacing = CComSystem::ChannelSpacing25KHz;
                f1Stations = sGui->getIContextNetwork()->getOnlineStationsForFrequency(freq1, spacing);
                f2Stations = sGui->getIContextNetwork()->getOnlineStationsForFrequency(freq2, spacing);
            }

            const QString f1n = QString::asprintf("%03.3f", freq1.valueRounded(CFrequencyUnit::MHz(), 3));
            const QString f2n = QString::asprintf("%03.3f", freq2.valueRounded(CFrequencyUnit::MHz(), 3));
            QString f1 = QStringLiteral("COM1: %1").arg(f1n);
            QString f2 = QStringLiteral("COM2: %1").arg(f2n);
            if (f1Stations.size() == 1)
            {
                f1 += u' ' % f1Stations.front().getCallsignAndControllerRealName();
            }
            if (f2Stations.size() == 1)
            {
                f2 += u' ' % f2Stations.front().getCallsignAndControllerRealName();
            }

            ui->tb_TextMessagesCOM1->setToolTip(f1);
            ui->tb_TextMessagesCOM1->setToolTip(f2);

            ui->tw_TextMessages->setTabText(ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM1), f1);
            ui->tw_TextMessages->setTabText(ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM2), f2);
        }

        QWidget *CTextMessageComponent::addNewTextMessageTab(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return nullptr; }
            QWidget *w = this->findTextMessageTabByCallsign(callsign, false);
            if (w) { return w; }

            const QString tabName = callsign.asString();
            const bool supervisor = callsign.isSupervisorCallsign();
            QWidget *newTabWidget = new QWidget(this);
            newTabWidget->setObjectName("Tab widget " + tabName);
            QPushButton *closeButton = new QPushButton("Close", newTabWidget);
            QVBoxLayout *layout = new QVBoxLayout(newTabWidget);
            CTextMessageTextEdit *textEdit = new CTextMessageTextEdit(newTabWidget);
            textEdit->setObjectName("tep_" + tabName);
            int marginLeft, marginRight, marginTop, marginBottom;
            ui->tb_TextMessagesAll->layout()->getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);
            newTabWidget->layout()->setContentsMargins(marginLeft, marginTop, marginRight, 2);
            layout->addWidget(textEdit);
            layout->addWidget(closeButton);
            newTabWidget->setLayout(layout);
            textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
            textEdit->setProperty("supervisormsg", supervisor);

            const int index = ui->tw_TextMessages->addTab(newTabWidget, tabName);
            QToolButton *closeButtonInTab = new QToolButton(newTabWidget);
            closeButtonInTab->setText("[X]");
            closeButtonInTab->setProperty("supervisormsg", supervisor);
            QTabBar *bar = ui->tw_TextMessages->tabBar();
            bar->setTabButton(index, QTabBar::RightSide, closeButtonInTab); // changes parent
            if (supervisor)
            {
                //! \fixme hardcoded stylesheet color
                bar->setTabIcon(index, callsign.toPixmap());
                bar->setTabTextColor(index, QColor(Qt::yellow));
            }
            ui->tw_TextMessages->setCurrentIndex(index);
            closeButtonInTab->setProperty("tabName", tabName);
            closeButton->setProperty("tabName", tabName);

            connect(closeButton, &QPushButton::released, this, &CTextMessageComponent::closeTextMessageTab);
            connect(closeButtonInTab, &QPushButton::released, this, &CTextMessageComponent::closeTextMessageTab);

            if (sGui && !sGui->isShuttingDown() && sGui->getIContextNetwork())
            {
                const QString realName = sGui->getIContextNetwork()->getUserForCallsign(CCallsign(tabName)).getRealName();
                if (!realName.isEmpty()) { ui->tw_TextMessages->setTabToolTip(index, realName); }
            }
            return newTabWidget;
        }

        void CTextMessageComponent::addPrivateChannelTextMessage(const CTextMessage &textMessage)
        {
            if (!textMessage.isPrivateMessage()) { return; }
            const CCallsign cs = textMessage.wasSent() ? textMessage.getRecipientCallsign() : textMessage.getSenderCallsign();
            if (cs.isEmpty()) { return; }
            const QWidget *tab = this->findTextMessageTabByCallsign(cs);
            if (!tab) { tab = this->addNewTextMessageTab(cs); }
            Q_ASSERT_X(tab, Q_FUNC_INFO, "Missing tab");
            CTextMessageTextEdit *textEdit = tab->findChild<CTextMessageTextEdit *>();
            BLACK_VERIFY_X(textEdit, Q_FUNC_INFO, "Missing text edit");
            if (!textEdit) { return; } // do not crash, though this situation should not happen
            textEdit->insertTextMessage(textMessage);

            // sound
            if (!m_usedAsOverlayWidget && sGui && !sGui->isShuttingDown() && sGui->getIContextAudio() && m_audioSettings.get().textMessagePrivate())
            {
                sGui->getIContextAudio()->playNotification(CNotificationSounds::NotificationTextMessagePrivate, true);
            }
        }

        CSimulatedAircraft CTextMessageComponent::getOwnAircraft() const
        {
            if (!sGui || !sGui->getIContextOwnAircraft()) { return CSimulatedAircraft(); }
            return sGui->getIContextOwnAircraft()->getOwnAircraft();
        }

        QWidget *CTextMessageComponent::findTextMessageTabByCallsign(const CCallsign &callsign, bool callsignResolution) const
        {
            QWidget *w = this->findTextMessageTabByName(callsign.asString());
            if (w) { return w; }
            if (!callsignResolution) { return nullptr; }

            // resolve callsign to COM tab
            if (!sGui || !sGui->getIContextNetwork()) { return nullptr; }
            const CAtcStation station(sGui->getIContextNetwork()->getOnlineStationForCallsign(callsign));
            if (!station.getCallsign().isEmpty())
            {
                const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
                if (ownAircraft.getCom1System().isActiveFrequencyWithin25kHzChannel(station.getFrequency()))
                {
                    return this->getTabWidget(TextMessagesCom1);
                }
                else if (ownAircraft.getCom2System().isActiveFrequencyWithin25kHzChannel(station.getFrequency()))
                {
                    return this->getTabWidget(TextMessagesCom2);
                }
            }
            return nullptr;
        }

        QWidget *CTextMessageComponent::findTextMessageTabByName(const QString &name) const
        {
            if (name.isEmpty()) { return nullptr; }
            const QString n = name.trimmed();

            // search the private message tabs first
            for (int index = ui->tw_TextMessages->count() - 1; index >= 0; index--)
            {
                const QString tabName = ui->tw_TextMessages->tabText(index);
                if (!tabName.startsWith(name, Qt::CaseInsensitive)) { continue; }
                QWidget *tab = ui->tw_TextMessages->widget(index);
                return tab;
            }
            return nullptr;
        }

        void CTextMessageComponent::closeTextMessageTab()
        {
            int index = -1;
            const QObject *sender = QObject::sender(); // the button
            const QString tabName = sender->property("tabName").toString();
            QWidget *tw = this->findTextMessageTabByName(tabName);
            if (!this->isCloseableTab(tw)) { return; }
            if (tw) { index = ui->tw_TextMessages->indexOf(tw); }
            if (index >= 0) { ui->tw_TextMessages->removeTab(index); }
        }

        void CTextMessageComponent::topLevelChanged(QWidget *widget, bool topLevel)
        {
            // own input field if floating window
            Q_UNUSED(widget);
            ui->lep_textMessages->setVisible(topLevel);
        }

        void CTextMessageComponent::textMessageEntered()
        {
            if (!ui->lep_textMessages->isVisible()) { return; }
            if (!this->isVisible()) { return; }

            const QString cl(ui->lep_textMessages->getLastEnteredLineFormatted());
            if (!cl.isEmpty())
            {
                this->handleEnteredTextMessage(cl);
            }
        }

        bool CTextMessageComponent::isVisibleWidgetHack() const
        {
            return m_usedAsOverlayWidget ? true : this->isVisibleWidget();
        }

        void CTextMessageComponent::emitDisplayInInfoWindow(const CVariant &message, int displayDurationMs)
        {
            if (m_usedAsOverlayWidget) { return; }
            emit this->displayInInfoWindow(message, displayDurationMs);
        }

        void CTextMessageComponent::handleEnteredTextMessage(const QString &textMessage)
        {
            if (!this->isVisibleWidgetHack()) { return; }

            QString cl(textMessage.trimmed().simplified());
            if (cl.isEmpty()) { return; }

            // is this a command?
            if (!cl.startsWith("."))
            {
                // build a command line -> e.g. ".msg 122.8 fooBar"
                cl = this->textMessageToCommand(cl);
            }

            // relay the command
            if (cl.isEmpty()) { return; }
            emit this->commandEntered(cl, this->componentIdentifier());
        }

        QString CTextMessageComponent::textMessageToCommand(const QString &enteredLine)
        {
            // only if visible
            if (enteredLine.isEmpty()) { return {}; }

            const int index = ui->tw_TextMessages->currentIndex();
            QString cmd(".msg ");
            if (index < 0 || index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesAll))
            {
                CLogMessage(this).validationError(u"Incorrect message channel");
                return {};
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
            if (!m_activeReceive) { return; }
            this->displayTextMessage(messages);
        }

        void CTextMessageComponent::onTextMessageSent(const CTextMessage &sentMessage)
        {
            if (!m_activeSend) { return; }
            this->displayTextMessage(sentMessage);
        }

        bool CTextMessageComponent::handleGlobalCommandLineText(const QString &commandLine, const CIdentifier &originator)
        {
            if (originator == this->componentIdentifier()) { return false; }
            if (commandLine.isEmpty() || commandLine.startsWith(".")) { return false; }

            // no "dot" command input
            if (!this->isVisibleWidgetHack()) { return false; } // invisible, do ignore
            this->handleEnteredTextMessage(commandLine); // handle as it was entered by own command line

            return false; // we never handle the message directly, but forward it
        }

        void CTextMessageComponent::showCorrespondingTab(const CCallsign &callsign)
        {
            if (callsign.isEmpty())
            {
                CLogMessage(this).warning(u"No callsign to display text message");
                return;
            }
            QWidget *w = this->findTextMessageTabByCallsign(callsign, true);
            if (!w && sGui && sGui->getIContextNetwork())
            {
                if (!callsign.isAtcCallsign() && sGui->getIContextNetwork()->isAircraftInRange(callsign))
                {
                    // we assume a private message from a pilot
                    w = this->addNewTextMessageTab(callsign);
                }
                else if (sGui->getIContextNetwork()->isOnlineStation(callsign))
                {
                    // we assume a private message of ATC
                    w = this->addNewTextMessageTab(callsign);
                }
            }
            if (!w) { return; }
            ui->tw_TextMessages->setCurrentWidget(w);

            // force display
            if (!m_usedAsOverlayWidget) { this->displayMyself(); }
        }

        void CTextMessageComponent::fontSizeMinus()
        {
            ui->comp_SettingsStyle->fontSizeMinus();
        }

        void CTextMessageComponent::fontSizePlus()
        {
            ui->comp_SettingsStyle->fontSizePlus();
        }

        void CTextMessageComponent::setTab(TextMessageTab tab)
        {
            // set via widget, as ALL can be removed
            switch (tab)
            {
            case TextMessagesAll : ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesAll); break;
            case TextMessagesCom1: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesCOM1); break;
            case TextMessagesCom2: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesCOM2); break;
            case TextMessagesUnicom: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesUnicom); break;
            default: break;
            }
        }

        void CTextMessageComponent::setAtcButtonsRowsColumns(int rows, int cols, bool setMaxElements)
        {
            ui->comp_AtcStations->setRowsColumns(rows, cols, setMaxElements);
        }

        void CTextMessageComponent::setAtcButtonsBackgroundUpdates(bool backgroundUpdates)
        {
            ui->comp_AtcStations->setBackgroundUpdates(backgroundUpdates);
        }

        void CTextMessageComponent::updateAtcStationsButtons()
        {
            ui->comp_AtcStations->updateStations();
        }

        bool CTextMessageComponent::hasAllMessagesTab() const
        {
            return ui->tw_TextMessages->widget(0) == ui->tb_TextMessagesAll;
        }

        void CTextMessageComponent::showSettings(bool show)
        {
            ui->gb_Settings->setVisible(show);
        }

        void CTextMessageComponent::showTextMessageEntry(bool show)
        {
            ui->lep_textMessages->setVisible(show);
        }

        void CTextMessageComponent::focusTextEntry()
        {
            if (!ui->lep_textMessages->isVisible()) { return; }
            ui->lep_textMessages->setFocus();
        }

        void CTextMessageComponent::removeAllMessagesTab()
        {
            ui->tw_TextMessages->removeTab(0);
        }
    } // namespace
} // namespace
