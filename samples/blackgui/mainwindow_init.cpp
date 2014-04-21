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
#include "blackgui/atcstationlistmodel.h"
#include "blackgui/keyboardkeylistmodel.h"
#include "blackmisc/avselcal.h"
#include <QSortFilterProxyModel>
#include <QSizeGrip>
#include <QHBoxLayout>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;


/*
 * Init data
 */
void MainWindow::init(const CRuntimeConfig &runtimeConfig)
{
    if (this->m_init) return;

    // with frameless window, we shift menu and statusbar into central widget
    // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    if (this->m_windowMode == GuiModes::WindowFrameless)
    {

        this->ui->wi_CentralWidgetOutside->setStyleSheet("#wi_CentralWidgetOutside {border: 2px solid green; border-radius: 20px; }");
        this->ui->vl_CentralWidgetOutside->setContentsMargins(8, 8, 8, 8);

        QHBoxLayout *menuBarLayout = new QHBoxLayout();
        QPushButton *closeIcon = new QPushButton(this);
        closeIcon->setStyleSheet("margin: 0; padding: 0; background: transparent;");
        closeIcon->setIcon(QIcon(":/blackgui/icons/close.png"));
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

    // init encapsulated table views / models
    this->ui->tvp_AtcStationsBooked->setStationMode(CAtcStationListModel::StationsBooked);
    this->ui->tvp_CockpitVoiceRoom1->setUserMode(CUserListModel::UserShort);
    this->ui->tvp_CockpitVoiceRoom2->setUserMode(CUserListModel::UserShort);

    // SELCAL pairs in cockpit
    this->ui->cb_CockpitSelcal1->clear();
    this->ui->cb_CockpitSelcal2->clear();
    this->ui->cb_CockpitSelcal1->addItems(BlackMisc::Aviation::CSelcal::codePairs());
    this->ui->cb_CockpitSelcal2->addItems(BlackMisc::Aviation::CSelcal::codePairs());

    // timers
    if (this->m_timerUpdateAircraftsInRange == nullptr) this->m_timerUpdateAircraftsInRange = new QTimer(this);
    if (this->m_timerUpdateAtcStationsOnline == nullptr) this->m_timerUpdateAtcStationsOnline = new QTimer(this);
    if (this->m_timerUpdateUsers == nullptr) this->m_timerUpdateUsers = new QTimer(this);
    if (this->m_timerContextWatchdog == nullptr) this->m_timerContextWatchdog = new QTimer(this);
    if (this->m_timerCollectedCockpitUpdates == nullptr) this->m_timerCollectedCockpitUpdates = new QTimer(this);
    if (this->m_timerAudioTests == nullptr) this->m_timerAudioTests = new QTimer(this);
    if (this->m_timerSimulator == nullptr) this->m_timerSimulator = new QTimer(this);

    // context
    this->createRuntime(runtimeConfig, this);
    this->ui->comp_Flightplan->setRuntime(this->getRuntime());

    // wire GUI signals
    this->initGuiSignals();

    // images
    this->m_resPixmapConnectionConnected = QPixmap(":/blackgui/icons/logingreen.png");
    this->m_resPixmapConnectionDisconnected = QPixmap(":/blackgui/icons/loginred.png");
    this->m_resPixmapConnectionConnecting = QPixmap(":/blackgui/icons/loginyellow.png");
    this->m_resPixmapVoiceLow = QPixmap(":/blackgui/icons/audiovolumelow.png");
    this->m_resPixmapVoiceHigh = QPixmap(":/blackgui/icons/audiovolumehigh.png");
    this->m_resPixmapVoiceMuted = QPixmap(":/blackgui/icons/audiovolumemuted.png");

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
    this->connect(this->getIContextApplication(), &IContextApplication::statusMessage, this, &MainWindow::displayStatusMessage);
    this->connect(this->getIContextApplication(), &IContextApplication::statusMessages, this, &MainWindow::displayStatusMessages);
    this->connect(this->getIContextApplication(), &IContextApplication::redirectedOutput, this, &MainWindow::displayRedirectedOutput);
    this->connect(this->getIContextNetwork(), &IContextNetwork::connectionTerminated, this, &MainWindow::connectionTerminated);
    this->connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &MainWindow::connectionStatusChanged);
    this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &MainWindow::changedSettings);
    connect = this->connect(this->getIContextNetwork(), SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)), this, SLOT(appendTextMessagesToGui(BlackMisc::Network::CTextMessageList)));
    Q_ASSERT(connect);
    this->connect(this->getIContextSimulator(), &IContextSimulator::connectionChanged, this, &MainWindow::simulatorConnectionChanged);
    this->connect(this->m_timerUpdateAircraftsInRange, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    this->connect(this->m_timerUpdateAtcStationsOnline, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    this->connect(this->m_timerUpdateUsers, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    this->connect(this->m_timerContextWatchdog, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    this->connect(this->m_timerCollectedCockpitUpdates, &QTimer::timeout, this, &MainWindow::sendCockpitUpdates);
    this->connect(this->m_timerAudioTests, &QTimer::timeout, this, &MainWindow::audioTestUpdate);
    this->connect(this->m_timerSimulator, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    connect = this->connect(this->getIContextAudio(), &IContextAudio::audioTestCompleted, this, &MainWindow::audioTestUpdate);
    Q_ASSERT(connect);
    Q_UNUSED(connect); // suppress GCC warning in release build

    // start timers, update timers will be started when network is connected
    this->m_timerContextWatchdog->start(2 * 1000);

    // init availability
    this->setContextAvailability();

    // voice panel
    this->setAudioDeviceLists();
    this->ui->prb_SettingsAudioTestProgress->setVisible(false);

    // data
    this->initialDataReads();

    // start screen
    this->setMainPage(true);

    // init context menus
    this->initContextMenus();

    // starting
    this->getIContextApplication()->notifyAboutComponentChange(IContextApplication::ComponentGui, IContextApplication::ActionStarts);

    // We don't receive signals from the past. So ask for it simulate an initial signal
    simulatorConnectionChanged(this->getIContextSimulator()->isConnected());

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
    this->connect(this->ui->sw_MainMiddle, &QStackedWidget::currentChanged, this, &MainWindow::middlePanelChanged);
    connected = this->connect(this->ui->pb_MainAircrafts, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainAtc, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainCockpit, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainConnect, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainConnect, SIGNAL(released()), this, SLOT(toggleNetworkConnection()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainFlightplan, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainSettings, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainSimulator, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainStatus, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainUsers, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainTextMessages, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainWeather, SIGNAL(released()), this, SLOT(setMainPage()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainKeypadOpacity050, SIGNAL(clicked()), this, SLOT(changeWindowOpacity()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_MainKeypadOpacity100, SIGNAL(clicked()), this, SLOT(changeWindowOpacity()));
    Q_ASSERT(connected);

    // Sound buttons
    this->connect(this->ui->pb_SoundMute, &QPushButton::clicked, this, &MainWindow::audioVolumes);
    this->connect(this->ui->pb_SoundMaxVolume, &QPushButton::clicked, this, &MainWindow::audioVolumes);
    connected = this->connect(this->ui->di_CockpitCom1Volume, &QDial::valueChanged, this, &MainWindow::audioVolumes);
    connected = this->connect(this->ui->di_CockpitCom2Volume, &QDial::valueChanged, this, &MainWindow::audioVolumes);

    // menu
    this->connect(this->ui->menu_ReloadSettings, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDDF, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDDM, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDNX, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDRY, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_FileClose, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_FileSettingsDirectory, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_FileResetSettings, &QAction::triggered, this, &MainWindow::menuClicked);

    // command line
    this->connect(this->ui->le_CommandLineInput, &QLineEdit::returnPressed, this, &MainWindow::commandEntered);

    // cockpit
    connected = this->connect(this->ui->cbp_CockpitTransponderMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(cockpitValuesChanged()));
    Q_ASSERT(connected);
    this->connect(this->ui->ds_CockpitCom1Active, &QDoubleSpinBox::editingFinished, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->ds_CockpitCom2Active, &QDoubleSpinBox::editingFinished, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->ds_CockpitCom1Standby, &QDoubleSpinBox::editingFinished, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->ds_CockpitCom2Standby, &QDoubleSpinBox::editingFinished, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->ds_CockpitTransponder, &QDoubleSpinBox::editingFinished, this, &MainWindow::cockpitValuesChanged);

    this->connect(this->ui->cb_CockpitVoiceRoom1Override, &QCheckBox::clicked, this, &MainWindow::setAudioVoiceRooms);
    this->connect(this->ui->cb_CockpitVoiceRoom2Override, &QCheckBox::clicked, this, &MainWindow::setAudioVoiceRooms);
    this->connect(this->ui->le_CockpitVoiceRoomCom1, &QLineEdit::returnPressed, this, &MainWindow::setAudioVoiceRooms);
    this->connect(this->ui->le_CockpitVoiceRoomCom2, &QLineEdit::returnPressed, this, &MainWindow::setAudioVoiceRooms);
    this->connect(this->ui->pb_CockpitToggleCom1, &QPushButton::clicked, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->pb_CockpitToggleCom2, &QPushButton::clicked, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->pb_CockpitIdent, &QPushButton::clicked, this, &MainWindow::cockpitValuesChanged);
    this->connect(this->ui->pb_CockpitSelcalTest, &QPushButton::clicked, this, &MainWindow::testSelcal);
    this->connect(this->ui->cbp_CockpitTransponderMode, &CTransponderModeSelector::identEnded, this, &MainWindow::resetTransponderMode);
    this->connect(qApp, &QApplication::focusChanged, this, &MainWindow::inputFocusChanged);

    // voice
    connected = this->connect(this->ui->cb_SettingsAudioInputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(audioDeviceSelected(int)));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->cb_SettingsAudioOutputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(audioDeviceSelected(int)));
    Q_ASSERT(connected);
    this->connect(this->ui->pb_SettingsAudioMicrophoneTest, &QPushButton::clicked, this, &MainWindow::startAudioTest);
    this->connect(this->ui->pb_SettingsAudioSquelchTest, &QPushButton::clicked, this, &MainWindow::startAudioTest);

    // ATC
    connected = this->connect(this->ui->le_AtcStationsOnlineMetar, SIGNAL(returnPressed()), this, SLOT(getMetar()));
    Q_ASSERT(connected);
    connected = this->connect(this->ui->pb_AtcStationsLoadMetar, SIGNAL(clicked()), this, SLOT(getMetar()));
    Q_ASSERT(connected);
    this->connect(this->ui->tw_AtcStations, &QTabWidget::currentChanged, this, &MainWindow::atcStationTabChanged);
    this->connect(this->ui->pb_ReloadAtcStationsBooked, &QPushButton::clicked, this, &MainWindow::reloadAtcStationsBooked);
    this->connect(this->ui->tvp_AtcStationsOnline, &QTableView::clicked, this, &MainWindow::onlineAtcStationSelected);
    this->connect(this->ui->pb_AtcStationsAtisReload, &QPushButton::clicked, this, &MainWindow::requestAtis);

    // Settings server
    this->connect(this->ui->pb_SettingsTnCurrentServer, &QPushButton::released, this, &MainWindow::alterTrafficServer);
    this->connect(this->ui->pb_SettingsTnRemoveServer, &QPushButton::released, this, &MainWindow::alterTrafficServer);
    this->connect(this->ui->pb_SettingsTnSaveServer, &QPushButton::released, this, &MainWindow::alterTrafficServer);
    this->connect(this->ui->tvp_SettingsTnServers, &QTableView::clicked, this, &MainWindow::networkServerSelected);

    // Settings
    this->connect(this->ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &MainWindow::changeWindowOpacity);

    // Settings hotkeys
    this->connect(this->ui->pb_SettingsMiscCancel, &QPushButton::clicked, this, &MainWindow::reloadSettings);
    this->connect(this->ui->pb_SettingsMiscSave, &QPushButton::clicked, this, &MainWindow::saveHotkeys);
    this->connect(this->ui->pb_SettingsMiscRemove, &QPushButton::clicked, this, &MainWindow::clearHotkey);

    // Settings simulator
    this->connect(this->ui->pb_SettingsSimulatorFsxTestConnection, &QPushButton::clicked, this, &MainWindow::testSimConnectConnection);
    this->connect(this->ui->pb_SettingsSimulatorFsxSaveSimconnectCfg, &QPushButton::clicked, this, &MainWindow::saveSimConnectCfg);
    this->connect(this->ui->pb_SettingsSimulatorFsxOpenSimconnectCfg, &QPushButton::clicked, this, &MainWindow::simConnectCfgFile);
    this->connect(this->ui->pb_SettingsSimulatorFsxDeleteSimconnectCfg, &QPushButton::clicked, this, &MainWindow::simConnectCfgFile);
    this->connect(this->ui->pb_SettingsSimulatorFsxExistsSimconncetCfg, &QPushButton::clicked, this, &MainWindow::simConnectCfgFile);

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
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityError,
                                   "no initial data read as network context is not available"));
        return;
    }

    this->reloadSettings(); // init read
    this->reloadAtcStationsBooked(); // init read, to do this no traffic network required
    this->reloadOwnAircraft(); // init read, independent of traffic network

    if (this->getIContextNetwork()->isConnected())
    {
        // connection is already established
        this->reloadAircraftsInRange();
        this->reloadAllUsers();
        this->reloadAtcStationsOnline();
        this->updateGuiStatusInformation();
    }

    this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityInfo, "initial data read"));
}

/*
 * Start update timers
 */
void MainWindow::startUpdateTimers()
{
    this->m_timerUpdateAircraftsInRange->start(this->ui->hs_SettingsGuiAircraftRefreshTime->value() * 1000);
    this->m_timerUpdateAtcStationsOnline->start(this->ui->hs_SettingsGuiAtcRefreshTime->value() * 1000);
    this->m_timerUpdateUsers->start(this->ui->hs_SettingsGuiUserRefreshTime->value() * 1000);
}

/*
 * Stop udate timers
 */
void MainWindow::stopUpdateTimers(bool disconnect)
{
    this->m_timerUpdateAircraftsInRange->stop();
    this->m_timerUpdateAtcStationsOnline->stop();
    this->m_timerUpdateUsers->stop();
    this->m_timerAudioTests->stop();
    this->m_timerSimulator->stop();
    if (!disconnect) return;
    this->disconnect(this->m_timerUpdateAircraftsInRange);
    this->disconnect(this->m_timerUpdateAtcStationsOnline);
    this->disconnect(this->m_timerUpdateUsers);
    this->disconnect(this->m_timerAudioTests);
    this->disconnect(this->m_timerSimulator);
}
