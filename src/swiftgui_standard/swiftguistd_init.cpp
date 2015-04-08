/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "ui_swiftguistd.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_all_interfaces.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/components/cockpitcomponent.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/keyboardkeylistmodel.h"
#include "blackmisc/icons.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/project.h"
#include "blackmisc/hotkeyfunction.h"
#include "blackmisc/logmessage.h"
#include <QSizeGrip>
#include <QHBoxLayout>
#include <QPushButton>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Hardware;
using namespace BlackGui;
using namespace BlackGui::Components;


/*
 * Init data
 */
void SwiftGuiStd::init(const CRuntimeConfig &runtimeConfig)
{
    // POST(!) GUI init

    if (this->m_init) { return; }
    this->setVisible(false); // hide all, so no flashing windows during init

    // icon, initial position where intro was before
    this->setWindowIcon(CIcons::swift24());
    this->setWindowTitle(CProject::systemNameAndVersion());
    QPoint pos = CGuiUtility::introWindowPosition();
    this->move(pos);

    // with frameless window, we shift menu and statusbar into central widget
    // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    if (this->isFrameless())
    {
        // wrap menu in layout, add button to menu bar and insert on top
        QHBoxLayout *menuBarLayout = this->addFramelessCloseButton(this->ui->mb_MainMenuBar);
        this->ui->vl_CentralWidgetOutside->insertLayout(0, menuBarLayout, 0);

        // now insert the dock widget info bar into the widget
        this->ui->vl_CentralWidgetOutside->insertWidget(1, this->ui->dw_InfoBarStatus);

        // move the status bar into the frame
        // (otherwise it is dangling outside the frame as it belongs to the window)
        this->ui->sb_MainStatusBar->setParent(this->ui->wi_CentralWidgetOutside);
        this->ui->vl_CentralWidgetOutside->addWidget(this->ui->sb_MainStatusBar, 0);

        // grip
        this->addFramelessSizeGripToStatusBar(this->ui->sb_MainStatusBar);
    }

    // timers
    if (this->m_timerContextWatchdog == nullptr) { this->m_timerContextWatchdog = new QTimer(this) ; }

    // context
    this->createRuntime(runtimeConfig, this);
    CEnableForRuntime::setRuntimeForComponents(this->getRuntime(), this);

    // info bar and status bar
    this->m_statusBar.initStatusBar(this->ui->sb_MainStatusBar);
    this->ui->dw_InfoBarStatus->allowStatusBar(false);
    this->ui->dw_InfoBarStatus->setPreferredSizeWhenFloating(this->ui->dw_InfoBarStatus->size()); // set floating size

    // wire GUI signals
    this->initGuiSignals();

    // signal / slots contexts / timers
    connect(this->getIContextNetwork(), &IContextNetwork::connectionTerminated, this, &SwiftGuiStd::ps_onConnectionTerminated);
    connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &SwiftGuiStd::ps_onConnectionStatusChanged);
    connect(this->getIContextNetwork(), &IContextNetwork::textMessagesReceived, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::onTextMessageReceived);
    connect(this->getIContextNetwork(), &IContextNetwork::textMessageSent, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::onTextMessageSent);
    connect(this->m_timerContextWatchdog, &QTimer::timeout, this, &SwiftGuiStd::ps_handleTimerBasedUpdates);
    connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &SwiftGuiStd::ps_onChangedSetttings);

    // log messages
    m_logSubscriber.changeSubscription(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo));

    // start timers, update timers will be started when network is connected
    this->m_timerContextWatchdog->start(2 * 1000);

    // init availability
    this->setContextAvailability();

    // data
    this->initialDataReads();

    // start screen and complete menu
    this->ps_setMainPageToInfoArea();
    this->initDynamicMenus();

    // starting
    this->getIContextApplication()->notifyAboutComponentChange(IContextApplication::ApplicationGui, IContextApplication::ApplicationStarts);

    // info
    this->ui->comp_MainInfoArea->getLogComponent()->appendPlainTextToConsole(CProject::systemNameAndVersion());
    this->ui->comp_MainInfoArea->getLogComponent()->appendPlainTextToConsole(CProject::compiledInfo());

    // hotkeys
    this->ps_registerHotkeyFunctions();

    // update timers
    this->startUpdateTimersWhenConnected();

    // do this as last statement, so it can be used as flag
    // whether init has been completed
    this->setVisible(true);

    this->m_init = true;
}

/*
 * GUI signals
 */
void SwiftGuiStd::initGuiSignals()
{
    // Remark: With new style, only methods of same signature can be connected
    // This is why we still have some "old" SIGNAL/SLOT connections here

    // main window
    connect(this->ui->sw_MainMiddle, &QStackedWidget::currentChanged, this, &SwiftGuiStd::ps_onCurrentMainWidgetChanged);

    // main keypad
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::selectedMainInfoAreaDockWidget, this, &SwiftGuiStd::ps_setMainPageInfoArea);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::connectPressed, this, &SwiftGuiStd::ps_loginRequested);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::changedOpacity, this , &SwiftGuiStd::ps_onChangedWindowOpacity);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::identPressed, this->ui->comp_MainInfoArea->getCockpitComponent(), &CCockpitComponent::setSelectedTransponderModeStateIdent);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::commandEntered, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::handleGlobalCommandLine);
    connect(this->ui->comp_MainInfoArea, &CMainInfoAreaComponent::changedInfoAreaStatus, ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::onMainInfoAreaChanged);

    // menu
    connect(this->ui->menu_ReloadSettings, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDDF, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDDM, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDNX, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDRY, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestInternals, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_FileClose, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_FileSettingsDirectory, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_FileResetSettings, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_FileReloadStyleSheets, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowFont, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowMinimize, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowToggleOnTop, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowToggleNavigator, &QAction::triggered, this->ui->comp_InvisibleInfoArea, &CInvisibleInfoAreaComponent::toggleNavigator);
    connect(this->ui->menu_DebugMetaTypes, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);

    // command line / text messages
    connect(this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::displayInInfoWindow, this->m_compInfoWindow, &CInfoWindowComponent::display);

    // settings (GUI component), styles
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedWindowsOpacity, this, &SwiftGuiStd::ps_onChangedWindowOpacity);
    connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &SwiftGuiStd::ps_onStyleSheetsChanged);

    // sliders
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedUsersUpdateInterval, this->ui->comp_MainInfoArea->getUserComponent(), &CUserComponent::setUpdateIntervalSeconds);
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAircraftUpdateInterval, this->ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::setUpdateIntervalSeconds);
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAtcStationsUpdateInterval, this->ui->comp_MainInfoArea->getAtcStationComponent(), &::CAtcStationComponent::setUpdateIntervalSeconds);

    // login
    connect(this->ui->comp_Login, &CLoginComponent::loginOrLogoffCancelled, this, &SwiftGuiStd::ps_setMainPageToInfoArea);
    connect(this->ui->comp_Login, &CLoginComponent::loginOrLogoffSuccessful, this, &SwiftGuiStd::ps_setMainPageToInfoArea);
    connect(this->ui->comp_Login, &CLoginComponent::loginOrLogoffSuccessful, this->ui->comp_MainInfoArea->getFlightPlanComponent(), &CFlightPlanComponent::loginDataSet);
    connect(this, &SwiftGuiStd::currentMainInfoAreaChanged, this->ui->comp_Login, &CLoginComponent::mainInfoAreaChanged);
    connect(this->ui->comp_Login, &CLoginComponent::requestNetworkSettings, this->ui->comp_MainInfoArea->getFlightPlanComponent(), [ = ]()
    {
        this->ps_setMainPageInfoArea(CMainInfoAreaComponent::InfoAreaSettings);
        this->ui->comp_MainInfoArea->getSettingsComponent()->setSettingsTab(CSettingsComponent::SettingTabNetworkServers);
    });

    // text messages
    connect(this->ui->comp_MainInfoArea->getAtcStationComponent(), &CAtcStationComponent::requestTextMessageWidget, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab);
    connect(this->ui->comp_MainInfoArea->getMappingComponet(), &CMappingComponent::requestTextMessageWidget, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab);
    connect(this->ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::requestTextMessageWidget, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab);

    // main info area
    connect(this->ui->comp_MainInfoArea, &CMainInfoAreaComponent::changedWholeInfoAreaFloating, this, &SwiftGuiStd::ps_onChangedMainInfoAreaFloating);
}

/*
 * Init data when started
 */
void SwiftGuiStd::initialDataReads()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = (this->getIContextNetwork()->isUsingImplementingObject() || (this->getIContextApplication()->ping(t) == t));
    if (!this->m_coreAvailable)
    {
        CLogMessage(this).error("no initial data read as network context is not available");
        return;
    }

    this->ui->comp_MainInfoArea->getSettingsComponent()->reloadSettings(); // init read
    this->ps_reloadOwnAircraft(); // init read, independent of traffic network
    CLogMessage(this).info("initial data read");
}

/*
 * Start update timers
 */
void SwiftGuiStd::startUpdateTimersWhenConnected()
{
    this->ui->comp_MainInfoArea->getAtcStationComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAtcUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getAircraftComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAircraftUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getUserComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getUsersUpdateIntervalSeconds());
}

/*
 * Stop udate timers
 */
void SwiftGuiStd::stopUpdateTimersWhenDisconnected()
{
    this->ui->comp_MainInfoArea->getAtcStationComponent()->stopTimer();
    this->ui->comp_MainInfoArea->getAircraftComponent()->stopTimer();
    this->ui->comp_MainInfoArea->getUserComponent()->stopTimer();
}

/*
 * Stop all timers
 */
void SwiftGuiStd::stopAllTimers(bool disconnect)
{
    this->m_timerContextWatchdog->stop();
    this->stopUpdateTimersWhenDisconnected();
    if (!disconnect) return;
    this->disconnect(this->m_timerContextWatchdog);
}
