// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftguistd.h"
#include "ui_swiftguistd.h"
#include "blackgui/components/aircraftcomponent.h"
#include "blackgui/components/atcstationcomponent.h"
#include "blackgui/components/cockpitcomponent.h"
#include "blackgui/components/flightplancomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/logincomponent.h"
#include "blackgui/components/interpolationcomponent.h"
#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/mainkeypadareacomponent.h"
#include "blackgui/components/mappingcomponent.h"
#include "blackgui/components/navigatordialog.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/components/usercomponent.h"
#include "blackgui/dockwidgetinfobar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/stylesheetutility.h"
#include "core/webdataservices.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "core/context/contextaudio.h"
#include "sound/audioutilities.h"
#include "misc/network/networkutils.h"
#include "misc/sharedstate/datalinkdbus.h"
#include "misc/loghandler.h"
#include "misc/logmessage.h"
#include "misc/logpattern.h"
#include "misc/slot.h"
#include "misc/statusmessage.h"
#include "config/buildconfig.h"

#include <QAction>
#include <QHBoxLayout>
#include <QScopedPointer>
#include <QStackedWidget>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace swift::config;
using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::input;
using namespace BlackGui;
using namespace BlackGui::Components;

void SwiftGuiStd::init()
{
    // POST(!) GUI init
    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
    Q_ASSERT_X(sGui->getWebDataServices(), Q_FUNC_INFO, "Missing web services");
    Q_ASSERT_X(sGui->supportsContexts(), Q_FUNC_INFO, "Missing contexts");

    if (m_init) { return; }

    ui->dw_InfoBarStatus->initialFloating();

    this->setVisible(false); // hide all, so no flashing windows during init
    m_mwaStatusBar = &m_statusBar;
    m_mwaOverlayFrame = ui->fr_CentralFrameInside;
    m_mwaLogComponent = ui->comp_MainInfoArea->getLogComponent();
    sGui->initMainApplicationWidget(this);

    // log messages
    m_logHistoryForOverlay.setFilter(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityError));
    m_logHistoryForLogButtons.setFilter(CLogPattern().withSeverityAtOrAbove(SeverityWarning));
    connect(&m_logHistoryForOverlay, &CLogHistoryReplica::elementAdded, this, [this](const CStatusMessage &message) {
        //! \todo filter out validation messages at CLogPattern level
        if (!message.getCategories().contains(CLogCategories::validation())) { ui->fr_CentralFrameInside->showOverlayMessage(message); }
    });
    connect(&m_logHistoryForLogButtons, &CLogHistoryReplica::elementAdded, this, [this](const CStatusMessage &message) {
        if (message.getSeverity() == CStatusMessage::SeverityError)
        {
            m_statusBar.showErrorButton();
        }
        else if (message.getSeverity() == CStatusMessage::SeverityWarning)
        {
            m_statusBar.showWarningButton();
        }
    });
    m_logHistoryForOverlay.initialize(sApp->getDataLinkDBus());
    m_logHistoryForLogButtons.initialize(sApp->getDataLinkDBus());

    // style
    this->initStyleSheet();

    // with frameless window, we shift menu and statusbar into central widget
    // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    if (this->isFrameless())
    {
        // wrap menu in layout, add button to menu bar and insert on top
        QHBoxLayout *menuBarLayout = this->addFramelessCloseButton(ui->mb_MainMenuBar);
        ui->vl_CentralWidgetOutside->insertLayout(0, menuBarLayout, 0);

        // now insert the dock widget info bar into the widget
        ui->vl_CentralWidgetOutside->insertWidget(1, ui->dw_InfoBarStatus);

        // move the status bar into the frame
        // (otherwise it is dangling outside the frame as it belongs to the window)
        ui->sb_MainStatusBar->setParent(ui->wi_CentralWidgetOutside);
        ui->vl_CentralWidgetOutside->addWidget(ui->sb_MainStatusBar, 0);

        // grip
        this->addFramelessSizeGripToStatusBar(ui->sb_MainStatusBar);
    }

    // timers
    m_timerContextWatchdog.setObjectName(this->objectName().append(":m_timerContextWatchdog"));

    // info bar and status bar
    m_statusBar.initStatusBar(ui->sb_MainStatusBar);
    connect(&m_statusBar, &CManagedStatusBar::requestLogPage, ui->comp_MainInfoArea, &CMainInfoAreaComponent::displayLog);
    ui->dw_InfoBarStatus->allowStatusBar(false);
    ui->dw_InfoBarStatus->setPreferredSizeWhenFloating(ui->dw_InfoBarStatus->size()); // set floating size

    // navigator
    m_navigator->addAction(this->getToggleWindowVisibilityAction(m_navigator.data()));
    m_navigator->addActions(ui->comp_MainInfoArea->getInfoAreaToggleFloatingActions(m_navigator.data())); // here we add the actions for the main windows
    m_navigator->addAction(this->getWindowNormalAction(m_navigator.data()));
    m_navigator->addAction(this->getWindowMinimizeAction(m_navigator.data()));
    m_navigator->addAction(this->getToggleStayOnTopAction(m_navigator.data()));
    m_navigator->buildNavigator(1);

    // wire GUI signals
    this->initGuiSignals();

    // signal / slots contexts / timers
    Q_ASSERT_X(sGui->getIContextNetwork(), Q_FUNC_INFO, "Missing network context");
    Q_ASSERT_X(sGui->getIContextSimulator(), Q_FUNC_INFO, "Missing simulator context");

    bool s = connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &SwiftGuiStd::onConnectionStatusChanged, Qt::QueuedConnection);
    Q_ASSERT(s);
    s = connect(sGui->getIContextNetwork(), &IContextNetwork::kicked, this, &SwiftGuiStd::onKickedFromNetwork, Qt::QueuedConnection);
    Q_ASSERT(s);
    s = connect(sGui->getIContextSimulator(), &IContextSimulator::validatedModelSet, this, &SwiftGuiStd::onValidatedModelSet, Qt::QueuedConnection);
    Q_ASSERT(s);
    s = connect(&m_timerContextWatchdog, &QTimer::timeout, this, &SwiftGuiStd::handleTimerBasedUpdates);
    Q_ASSERT(s);

    if (sGui->getIContextAudio())
    {
        s = connect(sGui->getIContextAudio(), &IContextAudio::voiceClientFailure, this, &SwiftGuiStd::onAudioClientFailure, Qt::QueuedConnection);
        Q_ASSERT(s);
    }
    Q_UNUSED(s)

    // check if DB data have been loaded
    // only check once, so data can be loaded and
    connectOnce(sGui->getWebDataServices(), &CWebDataServices::sharedInfoObjectsRead, this, &SwiftGuiStd::checkDbDataLoaded, Qt::QueuedConnection);

    // start timers, update timers will be started when network is connected
    m_timerContextWatchdog.start(2500);

    // init availability
    this->setContextAvailability();

    // data
    this->initialContextDataReads();

    // start screen and complete menu
    this->setMainPageToInfoArea();
    this->initMenus();

    // info
    connect(ui->comp_InfoBarStatus, &CInfoBarStatusComponent::transponderModeChanged, ui->dw_InfoBarStatus, &CDockWidgetInfoBar::reloadStyleSheet, Qt::QueuedConnection);

    // Show kill button
    ui->fr_CentralFrameInside->showKillButton(true);

    // do this as last statement, so it can be used as flag
    // whether init has been completed
    this->setVisible(true);

    // more checks
    QPointer<SwiftGuiStd> myself(this);
    QTimer::singleShot(5000, this, [=] {
        if (!myself) { return; }
        this->verifyPrerequisites();
    });

    // trigger version check
    sGui->triggerNewVersionCheck(10 * 1000);

    // done
    m_init = true;
}

void SwiftGuiStd::initStyleSheet()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    const QString s = sGui->getStyleSheetUtility().styles(
        { CStyleSheetUtility::fileNameFonts(),
          CStyleSheetUtility::fileNameStandardWidget(),
          CStyleSheetUtility::fileNameSwiftStandardGui() });
    this->setStyleSheet(""); //! \todo KB 2018-07 without clearing the stylesheet I see a crash here for the 2nd update
    this->setStyleSheet(s);
}

void SwiftGuiStd::initGuiSignals()
{
    // Remark: With new style, only methods of same signature can be connected
    // This is why we still have some "old" SIGNAL/SLOT connections here

    // main window
    connect(ui->sw_MainMiddle, &QStackedWidget::currentChanged, this, &SwiftGuiStd::onCurrentMainWidgetChanged);

    // main keypad
    connect(ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::selectedMainInfoAreaDockWidget, this, &SwiftGuiStd::setMainPageInfoArea);
    connect(ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::connectPressed, this, &SwiftGuiStd::loginRequested);
    connect(ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::changedOpacity, this, &SwiftGuiStd::onChangedWindowOpacity);
    connect(ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::identPressed, ui->comp_MainInfoArea->getCockpitComponent(), &CCockpitComponent::setSelectedTransponderModeStateIdent);
    connect(ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::textEntered, ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::handleGlobalCommandLineText);
    connect(ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::audioPressed, ui->comp_MainInfoArea, &CMainInfoAreaComponent::selectAudioTab);
    connect(ui->comp_MainInfoArea, &CMainInfoAreaComponent::changedInfoAreaStatus, ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::onMainInfoAreaChanged);

    // text component
    connect(ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::textMessageTabSelected, this, &SwiftGuiStd::focusInTextMessageEntryField, Qt::QueuedConnection);

    // audio
    connect(ui->comp_MainInfoArea->getAtcStationComponent(), &CAtcStationComponent::requestAudioWidget, ui->comp_MainInfoArea, &CMainInfoAreaComponent::selectAudioTab);

    // menu
    connect(ui->menu_TestLocationsEDDF, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_TestLocationsEDDM, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_TestLocationsEDNX, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_TestLocationsEDRY, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_TestLocationsLOWW, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);

    connect(ui->menu_WindowToggleNavigator, &QAction::triggered, m_navigator.data(), &CNavigatorDialog::toggleNavigatorVisibility);
    connect(ui->menu_WindowFont, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_WindowMinimize, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_WindowToggleOnTop, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_InternalsPage, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_AutoPublish, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_ToggleIncognito, &QAction::triggered, this, &SwiftGuiStd::onMenuClicked);
    connect(ui->menu_ModelBrowser, &QAction::triggered, this, &SwiftGuiStd::startModelBrowser, Qt::QueuedConnection);
    connect(ui->menu_AfvMap, &QAction::triggered, this, &SwiftGuiStd::startAFVMap, Qt::QueuedConnection);

    connect(m_navigator.data(), &CNavigatorDialog::navigatorClosed, this, &SwiftGuiStd::onNavigatorClosed, Qt::QueuedConnection);
    m_navigator->setMainWindow(this);

    // settings (GUI component), styles
    connect(ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedWindowsOpacity, this, &SwiftGuiStd::onChangedWindowOpacity);
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &SwiftGuiStd::onStyleSheetsChanged, Qt::QueuedConnection);

    // login
    connect(ui->comp_Login, &CLoginComponent::loginOrLogoffCancelled, this, &SwiftGuiStd::setMainPageToInfoArea);
    connect(ui->comp_Login, &CLoginComponent::loginOrLogoffSuccessful, this, &SwiftGuiStd::setMainPageToInfoArea);
    connect(ui->comp_Login, &CLoginComponent::loginOrLogoffSuccessful, ui->comp_MainInfoArea->getFlightPlanComponent(), &CFlightPlanComponent::loginDataSet);
    connect(ui->comp_Login, &CLoginComponent::loginDataChangedDigest, ui->comp_MainInfoArea->getFlightPlanComponent(), &CFlightPlanComponent::loginDataSet);
    connect(ui->comp_Login, &CLoginComponent::requestNetworkSettings, this, &SwiftGuiStd::displayNetworkSettings);
    connect(ui->comp_Login, &CLoginComponent::requestLoginPage, [this]() {
        if (!sApp || sApp->isShuttingDown()) { return; }
        ui->sw_MainMiddle->setCurrentIndex(MainPageLogin);
    });
    connect(this, &SwiftGuiStd::currentMainInfoAreaChanged, ui->comp_Login, &CLoginComponent::mainInfoAreaChanged, Qt::QueuedConnection);

    // text messages
    connect(ui->comp_MainInfoArea->getAtcStationComponent(), &CAtcStationComponent::requestTextMessageWidget, ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getMappingComponent(), &CMappingComponent::requestTextMessageWidget, ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::requestTextMessageWidget, ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getUserComponent(), &CUserComponent::requestTextMessageWidget, ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab, Qt::QueuedConnection);

    // command line / text messages
    // here we display SUP messages and such in a central window
    ui->fr_CentralFrameInside->activateTextMessages(true);
    connect(ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::displayInInfoWindow, this, &SwiftGuiStd::onShowOverlayVariant, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getAtcStationComponent(), &CAtcStationComponent::requestTextMessageEntryTab, this, &SwiftGuiStd::onShowOverlayInlineTextMessageTab, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getAtcStationComponent(), &CAtcStationComponent::requestTextMessageEntryCallsign, this, &SwiftGuiStd::onShowOverlayInlineTextMessageCallsign, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getCockpitComponent(), &CCockpitComponent::requestTextMessageEntryTab, this, &SwiftGuiStd::onShowOverlayInlineTextMessageTab, Qt::QueuedConnection);
    connect(ui->comp_MainInfoArea->getCockpitComponent(), &CCockpitComponent::requestTextMessageEntryCallsign, this, &SwiftGuiStd::onShowOverlayInlineTextMessageCallsign, Qt::QueuedConnection);

    // interpolation and validation
    connect(ui->comp_MainInfoArea->getMappingComponent(), &CMappingComponent::requestValidationDialog, this, &SwiftGuiStd::displayValidationDialog);
    connect(ui->comp_MainInfoArea->getInterpolationComponent(), &CInterpolationComponent::requestRenderingRestrictionsWidget, [=] {
        this->setSettingsPage(CSettingsComponent::SettingTabSimulator);
    });

    // on top
    connect(sGui, &CGuiApplication::alwaysOnTop, this, &SwiftGuiStd::onToggledWindowsOnTop, Qt::QueuedConnection);

    // main info area
    connect(ui->comp_MainInfoArea, &CMainInfoAreaComponent::changedWholeInfoAreaFloating, this, &SwiftGuiStd::onChangedMainInfoAreaFloating, Qt::QueuedConnection);
}

void SwiftGuiStd::initialContextDataReads()
{
    this->setContextAvailability();
    if (!m_coreAvailable)
    {
        CLogMessage(this).error(u"No initial data read as network context is not available");
        return;
    }

    this->reloadOwnAircraft(); // init read, independent of traffic network
    CLogMessage(this).info(u"Initial data read");
}

void SwiftGuiStd::stopAllTimers(bool disconnectSignalSlots)
{
    m_timerContextWatchdog.stop();
    if (!disconnectSignalSlots) { return; }
    this->disconnect(&m_timerContextWatchdog);
}
