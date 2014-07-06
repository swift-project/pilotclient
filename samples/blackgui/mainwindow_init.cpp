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
#include "blackgui/textmessagecomponent.h"
#include "blackmisc/avselcal.h"
#include "blackmisc/project.h"
#include <QSortFilterProxyModel>
#include <QSizeGrip>
#include <QHBoxLayout>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Hardware;
using namespace BlackGui;


/*
 * Init data
 */
void MainWindow::init(const CRuntimeConfig &runtimeConfig)
{
    if (this->m_init) return;

    // with frameless window, we shift menu and statusbar into central widget
    // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    this->setWindowTitle(CProject::systemNameAndVersion());
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

    // timers
    if (this->m_timerContextWatchdog == nullptr) this->m_timerContextWatchdog = new QTimer(this);
    if (this->m_timerSimulator == nullptr) this->m_timerSimulator = new QTimer(this);

    // context
    this->createRuntime(runtimeConfig, this);
    CRuntimeBasedComponent::setRuntimeForComponents(this->getRuntime(), this);

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
    connect = this->connect(this->getIContextNetwork(), SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)), this->ui->comp_TextMessages, SLOT(appendTextMessagesToGui(BlackMisc::Network::CTextMessageList)));
    Q_ASSERT(connect);
    this->connect(this->getIContextSimulator(), &IContextSimulator::connectionChanged, this, &MainWindow::simulatorConnectionChanged);
    this->connect(this->m_timerContextWatchdog, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    this->connect(this->m_timerSimulator, &QTimer::timeout, this, &MainWindow::timerBasedUpdates);
    this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &MainWindow::changedSetttings);

    // sliders
    this->connect(this->ui->comp_Settings, &CSettingsComponent::changedUsersUpdateInterval, this->ui->comp_Users, &BlackGui::CUserComponent::setUpdateIntervalSeconds);
    this->connect(this->ui->comp_Settings, &CSettingsComponent::changedAircraftsUpdateInterval, this->ui->comp_Aircrafts, &BlackGui::CAircraftComponent::setUpdateIntervalSeconds);
    this->connect(this->ui->comp_Settings, &CSettingsComponent::changedAtcStationsUpdateInterval, this->ui->comp_AtcStations, &BlackGui::CAtcStationComponent::setUpdateIntervalSeconds);

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
    this->setMainPage(true);

    // init context menus
    this->initContextMenus();

    // starting
    this->getIContextApplication()->notifyAboutComponentChange(IContextApplication::ComponentGui, IContextApplication::ActionStarts);

    // We don't receive signals from the past. So ask for it simulate an initial signal
    simulatorConnectionChanged(this->getIContextSimulator()->isConnected());

    // info
    this->ui->te_StatusPageConsole->appendPlainText(CProject::systemNameAndVersion());
    this->ui->te_StatusPageConsole->appendPlainText(CProject::compiledInfo());

    // hotkeys
    this->registerHotkeys();

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
    this->connect(this->ui->comp_Cockpit, &CCockpitV1Component::audioVolumeChanged, this, &MainWindow::audioVolumes);

    // menu
    this->connect(this->ui->menu_ReloadSettings, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDDF, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDDM, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDNX, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_TestLocationsEDRY, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_FileClose, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_FileSettingsDirectory, &QAction::triggered, this, &MainWindow::menuClicked);
    this->connect(this->ui->menu_FileResetSettings, &QAction::triggered, this, &MainWindow::menuClicked);

    // command line / text messages
    connected = this->connect(this->ui->le_CommandLineInput, SIGNAL(returnPressed()), this->ui->comp_TextMessages, SLOT(commandEntered()));
    Q_ASSERT(connected);
    this->connect(this->ui->comp_TextMessages, &CTextMessageComponent::displayInInfoWindow, this->m_compInfoWindow, &CInfoWindowComponent::display);
    this->ui->comp_TextMessages->setSelcalCallback(std::bind(&CCockpitV1Component::getSelcalCode, this->ui->comp_Cockpit));

    // settings (GUI component)
    this->connect(this->ui->comp_Settings, &CSettingsComponent::changedWindowsOpacity, this, &MainWindow::changeWindowOpacity);

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

    this->ui->comp_Settings->reloadSettings(); // init read
    this->reloadOwnAircraft(); // init read, independent of traffic network
    this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityInfo, "initial data read"));
}

/*
 * Start update timers
 */
void MainWindow::startUpdateTimers()
{
    this->ui->comp_Aircrafts->setUpdateIntervalSeconds(this->ui->comp_Settings->getAircraftUpdateIntervalSeconds());
    this->ui->comp_AtcStations->setUpdateIntervalSeconds(this->ui->comp_Settings->getAtcUpdateIntervalSeconds());
    this->ui->comp_Users->setUpdateIntervalSeconds(this->ui->comp_Settings->getUsersUpdateIntervalSeconds());
}

/*
 * Stop udate timers
 */
void MainWindow::stopUpdateTimers()
{
    this->ui->comp_AtcStations->stopTimer();
    this->ui->comp_Aircrafts->stopTimer();
    this->ui->comp_Users->stopTimer();
}

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
