// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/textmessagecomponent.h"

#include <QApplication>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStringBuilder>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

#include "ui_textmessagecomponent.h"

#include "core/application.h"
#include "core/context/contextaudio.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "core/corefacade.h"
#include "gui/dockwidgetinfoarea.h"
#include "gui/guiapplication.h"
#include "gui/textmessagetextedit.h"
#include "gui/views/textmessageview.h"
#include "misc/audio/notificationsounds.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/iterator.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/textmessage.h"
#include "misc/network/user.h"
#include "misc/pq/constants.h"
#include "misc/pq/frequency.h"
#include "misc/pq/units.h"
#include "misc/sequence.h"
#include "misc/verify.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::gui;
using namespace swift::gui::settings;
using namespace swift::gui::views;
using namespace swift::misc::network;
using namespace swift::misc::audio;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CTextMessageComponent::CTextMessageComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CTextMessageComponent)
    {
        ui->setupUi(this);
        ui->tw_TextMessages->setCurrentIndex(0);
        ui->fr_TextMessage->setVisible(false);
        ui->tvp_TextMessagesAll->setResizeMode(CTextMessageView::ResizingAuto);
        ui->tvp_TextMessagesAll->setWordWrap(false);
        ui->comp_AtcStations->setWithIcons(false);

        // lep_textMessages is the own line edit
        bool c = connect(ui->lep_TextMessages, &CLineEditHistory::returnPressedUnemptyLine, this,
                         &CTextMessageComponent::textMessageEntered, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        c = connect(ui->gb_Settings, &QGroupBox::toggled, this, &CTextMessageComponent::onSettingsChecked,
                    Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        c = connect(ui->gb_MessageTo, &QGroupBox::toggled, this, &CTextMessageComponent::onMessageToChecked,
                    Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

        c = connect(ui->comp_AtcStations, &CAtcButtonComponent::requestAtcStation, this,
                    &CTextMessageComponent::onAtcButtonClicked, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

        c = connect(ui->cb_LatestFirst, &QCheckBox::toggled, this, &CTextMessageComponent::onLatestFirstChanged,
                    Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

        // style sheet
        c = connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CTextMessageComponent::onStyleSheetChanged,
                    Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        c = connect(ui->comp_SettingsStyle, &CSettingsTextMessageStyle::changed, this,
                    &CTextMessageComponent::updateSettings, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");

        if (sGui && sGui->getCoreFacade() && sGui->getIContextNetwork() && sGui->getIContextOwnAircraft())
        {
            c = connect(this, &CTextMessageComponent::commandEntered, sGui->getCoreFacade(),
                        &CCoreFacade::parseCommandLine, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(sGui->getIContextNetwork(), &IContextNetwork::textMessagesReceived, this,
                        &CTextMessageComponent::onTextMessageReceived, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(sGui->getIContextNetwork(), &IContextNetwork::textMessageSent, this,
                        &CTextMessageComponent::onTextMessageSent, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
            c = connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this,
                        &CTextMessageComponent::onChangedAircraftCockpit, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        }
        Q_UNUSED(c)

        // init by settings
        const QPointer<CTextMessageComponent> myself(this);
        QTimer::singleShot(2000, this, [=] {
            // init decoupled when sub components are fully init
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            this->onSettingsChanged(); // init
            this->showCurrentFrequenciesFromCockpit();
            const bool latestFirst = m_messageSettings.get().isLatestFirst();
            ui->tvp_TextMessagesAll->setSorting(CTextMessage::IndexUtcTimestamp,
                                                latestFirst ? Qt::DescendingOrder : Qt::AscendingOrder);

            // hide for the beginning
            ui->gb_Settings->setChecked(false);
            ui->gb_MessageTo->setChecked(false);
        });
    }

    CTextMessageComponent::~CTextMessageComponent() {}

    bool CTextMessageComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
    {
        bool c = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
        c = c && connect(this->getDockWidgetInfoArea(), &CDockWidgetInfoArea::widgetTopLevelChanged, this,
                         &CTextMessageComponent::topLevelChanged, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        return c;
    }

    QWidget *CTextMessageComponent::getTabWidget(TextMessageTab tab) const
    {
        switch (tab)
        {
        case TextMessagesAll: return ui->tb_TextMessagesAll;
        case TextMessagesCom1: return ui->tb_TextMessagesCOM1;
        case TextMessagesCom2: return ui->tb_TextMessagesCOM2;
        case TextMessagesUnicom: return ui->tb_TextMessagesUnicom;
        default: Q_ASSERT_X(false, Q_FUNC_INFO, "Wrong index"); break;
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
        if (w) { ui->tw_TextMessages->setCurrentWidget(w); }
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
        return (tabWidget != ui->tb_TextMessagesAll && tabWidget != ui->tb_TextMessagesCOM1 &&
                tabWidget != ui->tb_TextMessagesCOM2 && tabWidget != ui->tb_TextMessagesUnicom);
    }

    void CTextMessageComponent::displayTextMessage(const CTextMessageList &messages)
    {
        using namespace std::chrono_literals;
        if (messages.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown()) { return; }
        const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
        const CTextMessageSettings msgSettings(m_messageSettings.getThreadLocal());
        const bool playNotification = sGui && sGui->getIContextAudio();
        const bool audioCsMentioned = playNotification && m_audioSettings.get().textCallsignMentioned();

        bool addedToAllMessages = false;
        for (const CTextMessage &message : messages)
        {
            bool relevantForMe = false;
            CNotificationSounds::NotificationFlag notification = CNotificationSounds::NoNotifications;

            // SELCAL
            if (!m_usedAsOverlayWidget && message.isSelcalMessage() &&
                ownAircraft.isSelcalSelected(message.getSelcalCode()))
            {
                // this is SELCAL for me
                if (playNotification) { sGui->getCContextAudioBase()->playSelcalTone(message.getSelcalCode()); }

                if (msgSettings.popupSelcalMessages())
                {
                    CStatusMessage msg = CLogMessage(this).info(u"SELCAL received");
                    this->emitDisplayInInfoWindow(CVariant::from(msg), 3s);
                }
                continue;
            }

            // UNICOM
            if (message.isSendToUnicom())
            {
                ui->tep_TextMessagesUnicom->insertTextMessage(message);

                // Message was received from others
                if (!message.wasSent()) { notification = CNotificationSounds::NotificationTextMessageUnicom; }
                relevantForMe = true;
            }

            // check message, handle special cases first
            if (message.isServerMessage())
            {
                // void
            }
            else if (message.isBroadcastMessage())
            {
                // FAKE private message
                this->addPrivateChannelTextMessage(message);
                relevantForMe = true;
            }
            else if (message.isRadioMessage())
            {
                // check for own COM frequencies
                if (message.isSendToFrequency(ownAircraft.getCom1System().getFrequencyActive()))
                {
                    ui->tep_TextMessagesCOM1->insertTextMessage(message);
                    if (!message.isSendToUnicom())
                    {
                        notification = CNotificationSounds::NotificationTextMessageFrequency;
                    }
                    relevantForMe = true;
                }
                if (message.isSendToFrequency(ownAircraft.getCom2System().getFrequencyActive()))
                {
                    ui->tep_TextMessagesCOM2->insertTextMessage(message);
                    if (!message.isSendToUnicom())
                    {
                        notification = CNotificationSounds::NotificationTextMessageFrequency;
                    }
                    relevantForMe = true;
                }

                // callsign mentioned notification
                if (relevantForMe && audioCsMentioned && ownAircraft.hasCallsign() &&
                    message.mentionsCallsign(ownAircraft.getCallsign()))
                {
                    notification = CNotificationSounds::NotificationTextCallsignMentioned;
                    // Flash taskbar icon
                    QApplication::alert(QWidget::topLevelWidget());
                }
            }
            else if (message.isPrivateMessage())
            {
                // private message
                this->addPrivateChannelTextMessage(message);
                relevantForMe = true;
                // Flash taskbar icon
                QApplication::alert(QWidget::topLevelWidget());
            }
            else
            {
                SWIFT_AUDIT_X(false, Q_FUNC_INFO, "Wrong message type");
                continue;
            }

            // message for me? right frequency? otherwise quit
            if (this->hasAllMessagesTab() && (relevantForMe || message.isServerMessage()))
            {
                ui->tvp_TextMessagesAll->push_back(message); // no sorting
                ui->tvp_TextMessagesAll->resort();
                addedToAllMessages = true;
            }
            if (!relevantForMe) { continue; }

            // Play notification
            if (playNotification && notification != CNotificationSounds::NoNotifications)
            {
                sGui->getCContextAudioBase()->playNotification(notification, true);
            }

            // overlay message if this channel is not selected
            if (message.isServerMessage()) { continue; }
            if (message.isBroadcastMessage()) { continue; }

            if (!message.wasSent() && !message.isSendToUnicom())
            {
                // if the channel is selected, do nothing
                if (!this->isCorrespondingTextMessageTabSelected(message))
                {
                    if (msgSettings.popup(message, ownAircraft))
                    {
                        this->emitDisplayInInfoWindow(CVariant::from(message), 15s);
                    }
                }
            } // message
        } // for

        if (addedToAllMessages && ui->tvp_TextMessagesAll->isSortedByTimestampPropertyLatestLast())
        {
            ui->tvp_TextMessagesAll->scrollToBottom();
        }
    }

    void CTextMessageComponent::onChangedAircraftCockpit(const CSimulatedAircraft &aircraft,
                                                         const CIdentifier &originator)
    {
        // this is called for every overlay widget as well
        Q_UNUSED(originator)
        if (!this->isActivated()) { return; }
        this->showCurrentFrequenciesFromCockpit(aircraft);
    }

    void CTextMessageComponent::onSettingsChecked(bool checked)
    {
        ui->comp_SettingsOverlay->setVisible(checked);
        ui->comp_SettingsStyle->setVisible(checked);
        ui->cb_LatestFirst->setVisible(checked);
        ui->gb_Settings->setFlat(!checked);
    }

    void CTextMessageComponent::onMessageToChecked(bool checked)
    {
        ui->comp_AtcStations->setVisible(checked);
        ui->gb_MessageTo->setFlat(!checked);
        if (checked) { ui->comp_AtcStations->updateStations(); }
    }

    void CTextMessageComponent::onSettingsChanged()
    {
        QList<CTextMessageTextEdit *> textEdits = this->findAllTextEdit();
        const QString style = this->getStyleSheet();
        const bool latestFirst = m_messageSettings.get().isLatestFirst();
        for (CTextMessageTextEdit *textEdit : textEdits)
        {
            textEdit->setLatestFirst(latestFirst);
            textEdit->setStyleSheetForContent(style);
        }
        ui->comp_SettingsStyle->setStyle(this->getStyleSheet());
        if (latestFirst != ui->cb_LatestFirst->isChecked()) { ui->cb_LatestFirst->setChecked(latestFirst); }
        this->update(); // refresh window
    }

    void CTextMessageComponent::onLatestFirstChanged(bool checked)
    {
        CTextMessageSettings s = m_messageSettings.get();
        if (s.isLatestFirst() == checked) { return; }
        s.setLatestFirst(checked);
        const CStatusMessage m = m_messageSettings.setAndSave(s);
        CLogMessage::preformatted(m);
        this->onSettingsChanged(); // latest first
    }

    void CTextMessageComponent::onStyleSheetChanged()
    {
        this->onSettingsChanged(); // style sheet
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
        s.setLatestFirst(ui->cb_LatestFirst->isChecked());
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
        this->showCurrentFrequenciesFromCockpit(ownAircraft);
    }

    void CTextMessageComponent::showCurrentFrequenciesFromCockpit(const CSimulatedAircraft &ownAircraft)
    {
        const CFrequency freq1 = ownAircraft.getCom1System().getFrequencyActive();
        const CFrequency freq2 = ownAircraft.getCom2System().getFrequencyActive();

        CAtcStationList f1Stations;
        CAtcStationList f2Stations;
        if (sGui && sGui->getIContextNetwork())
        {
            f1Stations = sGui->getIContextNetwork()->getOnlineStationsForFrequency(freq1);
            f2Stations = sGui->getIContextNetwork()->getOnlineStationsForFrequency(freq2);
        }

        const QString f1n = QString::asprintf("%03.3f", freq1.valueRounded(CFrequencyUnit::MHz(), 3));
        const QString f2n = QString::asprintf("%03.3f", freq2.valueRounded(CFrequencyUnit::MHz(), 3));
        QString f1 = QStringLiteral("COM1: %1").arg(f1n);
        QString f2 = QStringLiteral("COM2: %1").arg(f2n);
        if (f1Stations.size() == 1) { f1 += u' ' % f1Stations.front().getCallsignAndControllerRealName(); }
        if (f2Stations.size() == 1) { f2 += u' ' % f2Stations.front().getCallsignAndControllerRealName(); }

        ui->tb_TextMessagesCOM1->setToolTip(f1);
        ui->tb_TextMessagesCOM1->setToolTip(f2);

        ui->tw_TextMessages->setTabText(ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM1), f1);
        ui->tw_TextMessages->setTabText(ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM2), f2);

        // update tabs
        this->updateAllTabs();
    }

    QWidget *CTextMessageComponent::addNewTextMessageTab(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return nullptr; }
        QWidget *w = this->findTextMessageTabByCallsign(callsign, false);
        if (w) { return w; }

        const QString tabName = callsign.asString();
        const QString style = this->getStyleSheet();
        const bool supervisor = callsign.isSupervisorCallsign();
        QWidget *newTabWidget = new QWidget(this);
        newTabWidget->setObjectName(u"Tab widget " % tabName);
        newTabWidget->setProperty("callsign", callsign.asString());
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
        textEdit->setStyleSheetForContent(style);

        const int index = ui->tw_TextMessages->addTab(newTabWidget, this->getCallsignAndRealName(callsign));
        QToolButton *closeButtonInTab = new QToolButton(newTabWidget);
        closeButtonInTab->setText("[X]");
        closeButtonInTab->setProperty("supervisormsg", supervisor);
        QTabBar *bar = ui->tw_TextMessages->tabBar();
        bar->setTabButton(index, QTabBar::RightSide, closeButtonInTab); // changes parent
        if (supervisor)
        {
            //! \fixme hardcoded stylesheet color
            bar->setTabIcon(index, CIcon(callsign.toIcon()).toQIcon());
            bar->setTabTextColor(index, QColor(Qt::yellow));
        }
        ui->tw_TextMessages->setCurrentIndex(index);
        closeButtonInTab->setProperty("tabName", tabName);
        closeButton->setProperty("tabName", tabName);

        connect(closeButton, &QPushButton::released, this, &CTextMessageComponent::closeTextMessageTab);
        connect(closeButtonInTab, &QPushButton::released, this, &CTextMessageComponent::closeTextMessageTab);

        this->setTabWidgetDescription(callsign, index);
        return newTabWidget;
    }

    void CTextMessageComponent::setTabWidgetDescription(const CCallsign &callsign, int widgetIndex)
    {
        if (callsign.isEmpty()) { return; }

        QString realName;
        if (sGui && !sGui->isShuttingDown() && sGui->getIContextNetwork())
        {
            realName = sGui->getIContextNetwork()->getUserForCallsign(callsign).getRealName();
            if (!realName.isEmpty()) { ui->tw_TextMessages->setTabToolTip(widgetIndex, realName); }
        }
        const QString tt = realName.isEmpty() ? callsign.asString() : callsign.asString() % u": " % realName;
        ui->tw_TextMessages->setTabText(widgetIndex, tt);
    }

    QString CTextMessageComponent::getCallsignAndRealName(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return {}; }
        if (m_showRealNames && sGui && !sGui->isShuttingDown() && sGui->getIContextNetwork())
        {
            const QString realName = sGui->getIContextNetwork()->getUserForCallsign(callsign).getRealName();
            if (!realName.isEmpty()) { return callsign.asString() % u": " % realName; }
        }
        return callsign.asString();
    }

    void CTextMessageComponent::updateAllTabs()
    {
        for (int index = ui->tw_TextMessages->count() - 1; index >= 0; index--)
        {
            QWidget *tab = ui->tw_TextMessages->widget(index);
            if (!tab) { continue; }
            if (!tab->toolTip().isEmpty()) { continue; }
            const QString cs = tab->property("callsign").toString();
            if (cs.isEmpty()) { continue; }
            this->setTabWidgetDescription(CCallsign(cs), index);
        }
    }

    void CTextMessageComponent::addPrivateChannelTextMessage(const CTextMessage &textMessage)
    {
        if (!textMessage.isPrivateMessage()) { return; }
        CCallsign cs = textMessage.wasSent() ? textMessage.getRecipientCallsign() : textMessage.getSenderCallsign();
        if (cs.isEmpty()) { return; }

        const bool isBroadcast = textMessage.isBroadcastMessage();
        if (isBroadcast) { cs.markAsBroadcastCallsign(); }

        const bool isWallopMessage = textMessage.isWallopMessage();
        if (isWallopMessage) { cs.markAsWallopCallsign(); }

        const QWidget *tab = this->findTextMessageTabByCallsign(cs);
        if (!tab) { tab = this->addNewTextMessageTab(cs); }
        Q_ASSERT_X(tab, Q_FUNC_INFO, "Missing tab");
        CTextMessageTextEdit *textEdit = tab->findChild<CTextMessageTextEdit *>();
        SWIFT_VERIFY_X(textEdit, Q_FUNC_INFO, "Missing text edit");
        if (!textEdit) { return; } // do not crash, though this situation should not happen
        textEdit->insertTextMessage(textMessage);

        // sound
        if (textMessage.isServerMessage()) { return; }
        if (isBroadcast) { return; }

        const bool playSound = !textMessage.wasSent() && !m_usedAsOverlayWidget && sGui && !sGui->isShuttingDown() &&
                               sGui->getIContextAudio();
        if (sGui && sGui->getIContextAudio() && playSound)
        {
            const CSettings settings = m_audioSettings.get();
            if (textMessage.isSupervisorMessage() && settings.textMessageSupervisor())
            {
                sGui->getCContextAudioBase()->playNotification(CNotificationSounds::NotificationTextMessageSupervisor,
                                                               true);
            }
            else if (textMessage.isPrivateMessage() && settings.textMessagePrivate())
            {
                sGui->getCContextAudioBase()->playNotification(CNotificationSounds::NotificationTextMessagePrivate,
                                                               true);
            }
        }
    }

    CSimulatedAircraft CTextMessageComponent::getOwnAircraft() const
    {
        if (!sGui || !sGui->getIContextOwnAircraft()) { return CSimulatedAircraft(); }
        return sGui->getIContextOwnAircraft()->getOwnAircraft();
    }

    QWidget *CTextMessageComponent::findTextMessageTabByCallsign(const CCallsign &callsign,
                                                                 bool callsignResolution) const
    {
        // search the private message tabs by property first
        for (int index = ui->tw_TextMessages->count() - 1; index >= 0; index--)
        {
            QWidget *tab = ui->tw_TextMessages->widget(index);
            if (tab && tab->property("callsign").toString() == callsign.asString()) { return tab; }
        }

        QWidget *w = this->findTextMessageTabByName(callsign.asString());
        if (w) { return w; }
        if (!callsignResolution) { return nullptr; }

        // resolve callsign to COM tab
        if (!sGui || !sGui->getIContextNetwork()) { return nullptr; }
        const CAtcStation station(sGui->getIContextNetwork()->getOnlineStationForCallsign(callsign));
        if (!station.getCallsign().isEmpty())
        {
            const CSimulatedAircraft ownAircraft(this->getOwnAircraft());
            if (ownAircraft.getCom1System().isActiveFrequencySameFrequency(station.getFrequency()))
            {
                return this->getTabWidget(TextMessagesCom1);
            }
            else if (ownAircraft.getCom2System().isActiveFrequencySameFrequency(station.getFrequency()))
            {
                return this->getTabWidget(TextMessagesCom2);
            }
        }
        return nullptr;
    }

    QWidget *CTextMessageComponent::findTextMessageTabByName(const QString &name) const
    {
        if (name.isEmpty()) { return nullptr; }

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
        ui->fr_TextMessage->setVisible(topLevel);
    }

    void CTextMessageComponent::textMessageEntered()
    {
        if (!ui->fr_TextMessage->isVisible() || !ui->lep_TextMessages->isVisible()) { return; }
        if (!this->isVisible()) { return; }

        const QString cl(ui->lep_TextMessages->getLastEnteredLineFormatted());
        if (!cl.isEmpty()) { this->handleEnteredTextMessage(cl); }
    }

    bool CTextMessageComponent::isVisibleWidgetHack() const
    {
        return m_usedAsOverlayWidget ? true : this->isVisibleWidget();
    }

    CCallsign CTextMessageComponent::getCallsignPropertyForTab(int tabIndex, bool validated) const
    {
        if (tabIndex < 0 || tabIndex >= ui->tw_TextMessages->count()) { return {}; }
        QWidget *tab = ui->tw_TextMessages->widget(tabIndex);
        if (tab && !tab->property("callsign").toString().isEmpty())
        {
            const CCallsign cs(tab->property("callsign").toString());
            if (!validated) { return cs; }
            if (sGui && sGui->getIContextNetwork())
            {
                const CAtcStation atc = sGui->getIContextNetwork()->getOnlineStationForCallsign(cs);
                if (atc.hasCallsign())
                {
                    return atc.getCallsign();
                } // first hand callsign diretcly from network context

                const CSimulatedAircraft aircraft = sGui->getIContextNetwork()->getAircraftInRangeForCallsign(cs);
                if (aircraft.hasCallsign())
                {
                    return aircraft.getCallsign();
                } // first hand callsign diretcly from network context
            }
            return cs;
        }
        return {};
    }

    void CTextMessageComponent::emitDisplayInInfoWindow(const CVariant &message,
                                                        std::chrono::milliseconds displayDuration)
    {
        if (m_usedAsOverlayWidget) { return; }
        emit this->displayInInfoWindow(message, displayDuration);
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
        else if (ui->tw_TextMessages->tabText(index) == "SUP")
        {
            CLogMessage(this).validationError(
                u"Message cannot be send to SUP channel. To send another wallop message use .wallop instead");
            return {};
        }
        else
        {
            if (index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM1))
            {
                cmd.append(QString::number(this->getOwnAircraft().getCom1System().getFrequencyActive().valueRounded(
                    CFrequencyUnit::MHz(), 3)));
            }
            else if (index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesCOM2))
            {
                cmd.append(QString::number(this->getOwnAircraft().getCom2System().getFrequencyActive().valueRounded(
                    CFrequencyUnit::MHz(), 3)));
            }
            else if (index == ui->tw_TextMessages->indexOf(ui->tb_TextMessagesUnicom))
            {
                cmd.append(QString::number(
                    CPhysicalQuantitiesConstants::FrequencyUnicom().valueRounded(CFrequencyUnit::MHz(), 3)));
            }
            else
            {
                // not a standard channel
                bool isNumber;
                const QString selectedTabText = firstPartOfTabText(ui->tw_TextMessages->tabText(index).trimmed());
                const double frequency = selectedTabText.toDouble(&isNumber);
                if (isNumber)
                {
                    const CFrequency radioFrequency = CFrequency(frequency, CFrequencyUnit::MHz());
                    if (CComSystem::isValidCivilAviationFrequency(radioFrequency))
                    {
                        cmd.append(QString::number(radioFrequency.valueRounded(CFrequencyUnit::MHz(), 3)));
                    }
                    else
                    {
                        // selectedTabText expected to be the callsign
                        // with T664 we resolve against the callsigns in the context if possible
                        const CCallsign cs = this->getCallsignPropertyForTab(index, true);
                        if (cs.isEmpty()) { cmd.append(selectedTabText); }
                        else { cmd.append(cs.isAtcCallsign() ? cs.getStringAsSet() : cs.asString()); }
                    }
                }
                else { cmd.append(selectedTabText); }
            }
            return cmd % u" " % enteredLine;
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

        emit this->textMessageTabSelected();
    }

    void CTextMessageComponent::showCorrespondingTabForFrequency(const CFrequency &frequency)
    {
        const CSimulatedAircraft ownAircraft = this->getOwnAircraft();
        const CFrequency freq1 = ownAircraft.getCom1System().getFrequencyActive();
        const CFrequency freq2 = ownAircraft.getCom2System().getFrequencyActive();
        if (freq1 == frequency)
        {
            this->setTab(TextMessagesCom1);
            return;
        }
        if (freq2 == frequency)
        {
            this->setTab(TextMessagesCom2);
            return;
        }
        this->setTab(TextMessagesAll);
    }

    void CTextMessageComponent::fontSizeMinus() { ui->comp_SettingsStyle->fontSizeMinus(); }

    void CTextMessageComponent::fontSizePlus() { ui->comp_SettingsStyle->fontSizePlus(); }

    void CTextMessageComponent::setTab(TextMessageTab tab)
    {
        // set via widget, as ALL can be removed
        switch (tab)
        {
        case TextMessagesAll: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesAll); break;
        case TextMessagesCom1: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesCOM1); break;
        case TextMessagesCom2: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesCOM2); break;
        case TextMessagesUnicom: ui->tw_TextMessages->setCurrentWidget(ui->tb_TextMessagesUnicom); break;
        default: break;
        }

        emit this->textMessageTabSelected();
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
        if (!ui->gb_MessageTo->isChecked()) { return; }
        ui->comp_AtcStations->updateStations();
    }

    bool CTextMessageComponent::hasAllMessagesTab() const
    {
        return ui->tw_TextMessages->widget(0) == ui->tb_TextMessagesAll;
    }

    void CTextMessageComponent::showSettings(bool show) { ui->gb_Settings->setVisible(show); }

    void CTextMessageComponent::showTextMessageEntry(bool show) { ui->fr_TextMessage->setVisible(show); }

    void CTextMessageComponent::focusTextEntry()
    {
        if (!ui->lep_TextMessages->isVisible()) { return; }
        const CTextMessageSettings s = m_messageSettings.get();
        if (m_usedAsOverlayWidget && !s.focusOverlayWindow()) { return; }
        ui->lep_TextMessages->setFocus();
    }

    void CTextMessageComponent::removeAllMessagesTab() { ui->tw_TextMessages->removeTab(0); }

    void CTextMessageComponent::activate(bool send, bool receive)
    {
        m_activeSend = send;
        m_activeReceive = receive;
    }

    QString CTextMessageComponent::firstPartOfTabText(const QString &tabText)
    {
        if (tabText.isEmpty()) { return {}; }
        int index = tabText.indexOf(':');
        if (index < 0) { index = tabText.indexOf(' '); }
        if (index >= 0) { return tabText.left(index); }
        return tabText;
    }
} // namespace swift::gui::components
