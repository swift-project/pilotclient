#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network_impl.h"
#include "blackcore/context_network_proxy.h"
#include "blackcore/context_simulator_impl.h"
#include "blackcore/context_simulator_proxy.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/context_application_proxy.h"
#include "blackcore/context_settings_impl.h"
#include "blackcore/context_settings_proxy.h"
#include "blackcore/context_audio_impl.h"
#include "blackcore/context_audio_proxy.h"
#include "blackcore/context_runtime.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/keyboardkeylistmodel.h"
#include "blackmisc/iconsstandard.h"
#include "blackmisc/avselcal.h"
#include "blackmisc/project.h"
#include <QSortFilterProxyModel>
#include <QSizeGrip>
#include <QHBoxLayout>

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
        closeIcon->setIcon(CIconsStandard::close16());
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
    if (this->m_timerSimulator == nullptr) this->m_timerSimulator = new QTimer(this);

    // context
    this->createRuntime(runtimeConfig, this);
    CRuntimeBasedComponent::setRuntimeForComponents(this->getRuntime(), this);

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
    bool connect;
    this->connect(this->getIContextApplication(), &IContextApplication::statusMessage, this, &MainWindow::ps_displayStatusMessageInGui);
    this->connect(this->getIContextApplication(), &IContextApplication::statusMessages, this, &MainWindow::ps_displayStatusMessagesInGui);
    this->connect(this->getIContextApplication(), &IContextApplication::redirectedOutput, this, &MainWindow::displayRedirectedOutput);
    this->connect(this->getIContextNetwork(), &IContextNetwork::connectionTerminated, this, &MainWindow::ps_onConnectionTerminated);
    this->connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &MainWindow::ps_onConnectionStatusChanged);
    connect = this->connect(this->getIContextNetwork(), SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)), this->ui->comp_MainInfoArea->getTextMessageComponent(), SLOT(appendTextMessagesToGui(BlackMisc::Network::CTextMessageList)));
    Q_ASSERT(connect);
    this->connect(this->getIContextSimulator(), &IContextSimulator::connectionChanged, this, &MainWindow::ps_onSimulatorConnectionChanged);
    this->connect(this->m_timerContextWatchdog, &QTimer::timeout, this, &MainWindow::ps_handleTimerBasedUpdates);
    this->connect(this->m_timerSimulator, &QTimer::timeout, this, &MainWindow::ps_handleTimerBasedUpdates);
    this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &MainWindow::ps_onChangedSetttings);

    // sliders
    this->connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedUsersUpdateInterval, this->ui->comp_MainInfoArea->getUserComponent(), &CUserComponent::setUpdateIntervalSeconds);
    this->connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAircraftsUpdateInterval, this->ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::setUpdateIntervalSeconds);
    this->connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAtcStationsUpdateInterval, this->ui->comp_MainInfoArea->getAtcStationComponent(), &::CAtcStationComponent::setUpdateIntervalSeconds);

    Q_ASSERT(connect);
    Q_UNUSED(connect); // suppress GCC warning in release build

    // start timers, update timers will be started when network is connected
    this->m_timerContextWatchdog->start(2 * 1000);

    // init availability
    this->setContextAvailability();

    // cockpit external buttons
    this->ui->comp_Cockpit->setExternalIdentButton(this->ui->pb_CockpitIdent);

    // data
    this->initialDataReads();

    // start screen
    this->ps_setMainPage(true);

    // init context menus
    this->initContextMenus();

    // starting
    this->getIContextApplication()->notifyAboutComponentChange(IContextApplication::ComponentGui, IContextApplication::ActionStarts);

    // We don't receive signals from the past. So ask for it simulate an initial signal
    ps_onSimulatorConnectionChanged(this->getIContextSimulator()->isConnected());

    // info
    this->ui->te_StatusPageConsole->appendPlainText(CProject::systemNameAndVersion());
    this->ui->te_StatusPageConsole->appendPlainText(CProject::compiledInfo());

    // hotkeys
    this->ps_registerHotkeys();

    // update timers
    this->startUpdateTimers();

    // do this as last statement, so it can be used as flag
    // whether init has been completed
    this->m_init = true;
}

//
// GUI signals
//
void MainWindow::initGuiSignals()
{
    bool connected;

    // Remark: With new style, only methods of same signature can be connected
    // This is why we still have some "old" SIGNAL/SLOT connections here

    // MAIN buttons
    connected = this->connect(this->ui->pb_MainAircrafts, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainAtc, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainCockpit, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainConnect, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainConnect, SIGNAL(released()), this, SLOT(ps_toggleNetworkConnection()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainFlightplan, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainSettings, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainSimulator, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainStatus, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainUsers, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainTextMessages, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainWeather, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainMappings, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainKeypadOpacity050, SIGNAL(clicked()), this, SLOT(ps_changeWindowOpacity()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainKeypadOpacity100, SIGNAL(clicked()), this, SLOT(ps_changeWindowOpacity()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_Foo, SIGNAL(released()), this, SLOT(ps_setMainPage()));
    Q_ASSERT(connected);

    // Sound buttons
    this->connect(this->ui->pb_SoundMute, &QPushButton::clicked, this, &MainWindow::ps_setAudioVolumes);
    this->connect(this->ui->pb_SoundMaxVolume, &QPushButton::clicked, this, &MainWindow::ps_setAudioVolumes);
    this->connect(this->ui->comp_Cockpit, &CCockpitV1Component::audioVolumeChanged, this, &MainWindow::ps_setAudioVolumes);

    // menu
    this->connect(this->ui->menu_ReloadSettings, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_TestLocationsEDDF, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_TestLocationsEDDM, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_TestLocationsEDNX, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_TestLocationsEDRY, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_FileClose, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_FileSettingsDirectory, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_FileResetSettings, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_FileReloadStyleSheets, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);
    this->connect(this->ui->menu_FileFont, &QAction::triggered, this, &MainWindow::ps_onMenuClicked);

    // command line / text messages
    connected = this->connect(this->ui->le_CommandLineInput, SIGNAL(returnPressed()), this->ui->comp_MainInfoArea->getTextMessageComponent(), SLOT(commandEntered()));
    Q_ASSERT(connected);
    this->connect(this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::displayInInfoWindow, this->m_compInfoWindow, &CInfoWindowComponent::display);
    this->ui->comp_MainInfoArea->getTextMessageComponent()->setSelcalCallback(std::bind(&CCockpitV1Component::getSelcalCode, this->ui->comp_Cockpit));

    // settings (GUI component), styles
    this->connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedWindowsOpacity, this, &MainWindow::ps_changeWindowOpacity);
    this->connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &MainWindow::ps_onStyleSheetsChanged);

    // no warnings in release build
    Q_UNUSED(connected);
}

/*
 * Init data when started
 */
void MainWindow::initialDataReads()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = (this->getIContextNetwork()->usingLocalObjects() || (this->getIContextApplication()->ping(t) == t));
    if (!this->m_coreAvailable)
    {
        this->ps_displayStatusMessageInGui(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityError,
                                           "no initial data read as network context is not available"));
        return;
    }

    this->ui->comp_MainInfoArea->getSettingsComponent()->reloadSettings(); // init read
    this->ps_reloadOwnAircraft(); // init read, independent of traffic network
    this->ps_displayStatusMessageInGui(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityInfo, "initial data read"));
}

/*
 * Start update timers
 */
void MainWindow::startUpdateTimers()
{
    this->ui->comp_MainInfoArea->getAircraftComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAircraftUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getAtcStationComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAtcUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getUserComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getUsersUpdateIntervalSeconds());
}

/*
 * Stop udate timers
 */
void MainWindow::stopUpdateTimers()
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
    this->m_timerSimulator->stop();
    this->stopUpdateTimers();
    if (!disconnect) return;
    this->disconnect(this->m_timerStatusBar);
    this->disconnect(this->m_timerContextWatchdog);
    this->disconnect(this->m_timerSimulator);
}
