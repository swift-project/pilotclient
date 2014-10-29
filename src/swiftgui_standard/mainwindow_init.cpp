#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_all_interfaces.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/components/cockpitcomponent.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/keyboardkeylistmodel.h"
#include "blackmisc/icons.h"
#include "blackmisc/avselcal.h"
#include "blackmisc/project.h"
#include "blackmisc/hotkeyfunction.h"
#include "blackmisc/logmessage.h"
#include <QSortFilterProxyModel>
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
void MainWindow::init(const CRuntimeConfig &runtimeConfig)
{
    if (this->m_init) return;

    // icon, initial position where intro was before
    this->setWindowIcon(CIcons::swift24());
    this->setWindowTitle(CProject::systemNameAndVersion());
    QPoint pos = CGuiUtility::introWindowPosition();
    this->move(pos);

    // with frameless window, we shift menu and statusbar into central widget
    // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    if (this->m_windowMode == GuiModes::WindowFrameless)
    {
        this->ui->wi_CentralWidgetOutside->setStyleSheet("#wi_CentralWidgetOutside {border: 2px solid green; border-radius: 20px; }");
        this->ui->vl_CentralWidgetOutside->setContentsMargins(8, 8, 8, 8);

        QHBoxLayout *menuBarLayout = new QHBoxLayout();
        QPushButton *closeIcon = new QPushButton(this);
        closeIcon->setStyleSheet("margin: 0; padding: 0; background: transparent;");
        closeIcon->setIcon(CIcons::close16());
        QObject::connect(closeIcon, &QPushButton::clicked, this, &QMainWindow::close);
        menuBarLayout->addWidget(this->ui->mb_MainMenuBar, 0, Qt::AlignTop | Qt::AlignLeft);
        menuBarLayout->addWidget(closeIcon, 0, Qt::AlignTop | Qt::AlignRight);
        this->ui->vl_CentralWidgetOutside->insertLayout(0, menuBarLayout, 0);

        QSizeGrip *grip = new QSizeGrip(this);
        grip->setStyleSheet("margin-right: 25px; background-color: transparent;");
        this->ui->sb_MainStatusBar->setParent(this->ui->wi_CentralWidgetOutside);
        this->ui->vl_CentralWidgetOutside->addWidget(this->ui->sb_MainStatusBar, 0);
        this->ui->sb_MainStatusBar->addPermanentWidget(grip);
    }

    // timers
    if (this->m_timerContextWatchdog == nullptr) this->m_timerContextWatchdog = new QTimer(this);

    // context
    this->createRuntime(runtimeConfig, this);
    CEnableForRuntime::setRuntimeForComponents(this->getRuntime(), this);

    // wire GUI signals
    this->initGuiSignals();

    // status bar
    if (!this->m_statusBarLabel)
    {
        // also subject of style sheet
        this->m_statusBarIcon = new QLabel(this);
        this->m_statusBarLabel = new QLabel(this);
        this->m_timerStatusBar = new QTimer(this);
        this->m_statusBarLabel->setMinimumHeight(16);
        connect(this->m_timerStatusBar, &QTimer::timeout, this->m_statusBarIcon, &QLabel::clear);
        connect(this->m_timerStatusBar, &QTimer::timeout, this->m_statusBarLabel, &QLabel::clear);
        this->ui->sb_MainStatusBar->addWidget(this->m_statusBarIcon, 0);
        this->ui->sb_MainStatusBar->addWidget(this->m_statusBarLabel, 1);
    }

    // signal / slots contexts / timers
    connect(this->getIContextNetwork(), &IContextNetwork::connectionTerminated, this, &MainWindow::ps_onConnectionTerminated);
    connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &MainWindow::ps_onConnectionStatusChanged);
    connect(this->getIContextNetwork(), &IContextNetwork::textMessagesReceived, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::onTextMessageReceived);
    connect(this->m_timerContextWatchdog, &QTimer::timeout, this, &MainWindow::ps_handleTimerBasedUpdates);
    connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &MainWindow::ps_onChangedSetttings);

    // sliders
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedUsersUpdateInterval, this->ui->comp_MainInfoArea->getUserComponent(), &CUserComponent::setUpdateIntervalSeconds);
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAircraftsUpdateInterval, this->ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::setUpdateIntervalSeconds);
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAtcStationsUpdateInterval, this->ui->comp_MainInfoArea->getAtcStationComponent(), &::CAtcStationComponent::setUpdateIntervalSeconds);

    // log messages
    m_logSubscriber.changeSubscription(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo));

    // start timers, update timers will be started when network is connected
    this->m_timerContextWatchdog->start(2 * 1000);

    // init availability
    this->setContextAvailability();

    // data
    this->initialDataReads();

    // start screen and complete menu
    this->ps_setMainPage();
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
    this->m_init = true;
}

//
// GUI signals
//
void MainWindow::initGuiSignals()
{
    // Remark: With new style, only methods of same signature can be connected
    // This is why we still have some "old" SIGNAL/SLOT connections here

    // Main keypad
    connect(this->ui->comp_MainKeypadArea, SIGNAL(selectedMainInfoAreaDockWidget(CMainInfoAreaComponent::InfoArea)), this, SLOT(ps_setMainPage()));
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::connectPressed, this, &MainWindow::ps_toggleNetworkConnection);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::changedOpacity, this , &MainWindow::ps_changeWindowOpacity);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::identPressed, this->ui->comp_MainInfoArea->getCockpitComponent(), &CCockpitComponent::setSelectedTransponderModeStateIdent);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::selectedMainInfoAreaDockWidget, this->ui->comp_MainInfoArea, &CMainInfoAreaComponent::selectArea);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::commandEntered, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::parseCommandLine);

    // menu
    connect(this->ui->menu_ReloadSettings, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDDF, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDDM, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDNX, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDRY, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_FileClose, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_FileSettingsDirectory, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_FileResetSettings, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_FileReloadStyleSheets, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    connect(this->ui->menu_FileFont, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);

    // command line / text messages
    connect(this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::displayInInfoWindow, this->m_compInfoWindow, &CInfoWindowComponent::display);

    // settings (GUI component), styles
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedWindowsOpacity, this, &MainWindow::ps_changeWindowOpacity);
    connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &MainWindow::ps_onStyleSheetsChanged);
}

/*
 * Init data when started
 */
void MainWindow::initialDataReads()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = (this->getIContextNetwork()->isUsingImplementingObject() || (this->getIContextApplication()->ping(t) == t));
    if (!this->m_coreAvailable)
    {
        this->ps_displayStatusMessageInGui(CLogMessage(this).error("no initial data read as network context is not available"));
        return;
    }

    this->ui->comp_MainInfoArea->getSettingsComponent()->reloadSettings(); // init read
    this->ps_reloadOwnAircraft(); // init read, independent of traffic network
    this->ps_displayStatusMessageInGui(CLogMessage(this).info("initial data read"));
}

/*
 * Start update timers
 */
void MainWindow::startUpdateTimersWhenConnected()
{
    this->ui->comp_MainInfoArea->getAtcStationComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAtcUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getAircraftComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAircraftUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getUserComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getUsersUpdateIntervalSeconds());
}

/*
 * Stop udate timers
 */
void MainWindow::stopUpdateTimersWhenDisconnected()
{
    this->ui->comp_MainInfoArea->getAtcStationComponent()->stopTimer();
    this->ui->comp_MainInfoArea->getAircraftComponent()->stopTimer();
    this->ui->comp_MainInfoArea->getUserComponent()->stopTimer();
}

/*
 * Stop all timers
 */
void MainWindow::stopAllTimers(bool disconnect)
{
    this->m_timerStatusBar->stop();
    this->m_timerContextWatchdog->stop();
    this->stopUpdateTimersWhenDisconnected();
    if (!disconnect) return;
    this->disconnect(this->m_timerStatusBar);
    this->disconnect(this->m_timerContextWatchdog);
}
