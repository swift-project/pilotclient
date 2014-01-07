#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/coreruntime.h"
#include <QSortFilterProxyModel>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;


/*
 * Init data
 */
void MainWindow::init(bool withDBus)
{
    if (this->m_init) return;
    this->m_withDBus = withDBus;

    // init models, the delete allows to re-init
    if (this->m_atcListBooked != nullptr) this->m_atcListBooked->deleteLater();
    this->m_atcListBooked = new CAtcListModel(this);

    if (this->m_atcListOnline != nullptr) this->m_atcListOnline->deleteLater();
    this->m_atcListOnline = new CAtcListModel(this);

    if (this->m_trafficServerList != nullptr) this->m_trafficServerList->deleteLater();
    this->m_trafficServerList = new CServerListModel(this);

    if (this->m_aircraftsInRange != nullptr) this->m_aircraftsInRange->deleteLater();
    this->m_aircraftsInRange = new CAircraftListModel(this);

    // set sort order and models
    // enable first, otherwise order in the model will be reset
    this->ui->tv_AtcStationsOnline->setSortingEnabled(true);
    this->ui->tv_AtcStationsOnline->setModel(this->m_atcListOnline);
    if (this->m_atcListOnline->hasValidSortColumn())
        this->ui->tv_AtcStationsOnline->horizontalHeader()->setSortIndicator(this->m_atcListOnline->getSortColumn(), this->m_atcListOnline->getSortOrder());

    this->m_atcListBooked->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAtcStation::IndexBookedFrom);
    this->ui->tv_AtcStationsBooked->setSortingEnabled(true);
    this->ui->tv_AtcStationsBooked->setModel(this->m_atcListBooked);
    if (this->m_atcListBooked->hasValidSortColumn())
        this->ui->tv_AtcStationsBooked->horizontalHeader()->setSortIndicator(this->m_atcListBooked->getSortColumn(), this->m_atcListBooked->getSortOrder());

    this->ui->tv_AircraftsInRange->setSortingEnabled(true);
    this->ui->tv_AircraftsInRange->setModel(this->m_aircraftsInRange);
    if (this->m_aircraftsInRange->hasValidSortColumn())
        this->ui->tv_AircraftsInRange->horizontalHeader()->setSortIndicator(this->m_aircraftsInRange->getSortColumn(), this->m_aircraftsInRange->getSortOrder());

    this->ui->tv_SettingsTnServers->setModel(this->m_trafficServerList);

    if (this->m_timerUpdateAircraftsInRange == nullptr) this->m_timerUpdateAircraftsInRange = new QTimer(this);
    if (this->m_timerUpdateAtcStationsOnline == nullptr) this->m_timerUpdateAtcStationsOnline = new QTimer(this);
    if (this->m_timerContextWatchdog == nullptr) this->m_timerContextWatchdog = new QTimer(this);
    if (this->m_timerCollectedCockpitUpdates == nullptr) this->m_timerCollectedCockpitUpdates = new QTimer(this);

    // context
    if (this->m_withDBus)
    {
        this->m_dBusConnection = QDBusConnection::sessionBus();
        this->m_contextNetwork = new BlackCore::IContextNetwork(BlackCore::CDBusServer::ServiceName, this->m_dBusConnection, this);
        this->m_contextSettings = new BlackCore::IContextSettings(BlackCore::CDBusServer::ServiceName, this->m_dBusConnection, this);
    }
    else
    {
        this->m_coreRuntime = new CCoreRuntime(false, this);
        this->m_contextNetwork = this->m_coreRuntime->getIContextNetwork();
        this->m_contextSettings = this->m_coreRuntime->getIContextSettings();
    }

    // relay status messages
    bool connect;
    connect = this->connect(this->m_contextNetwork, SIGNAL(statusMessage(BlackMisc::CStatusMessage)),
                            this, SLOT(displayStatusMessage(BlackMisc::CStatusMessage)));
    Q_ASSERT_X(connect, "init", "cannot connect status message");

    connect = this->connect(this->m_contextNetwork, SIGNAL(connectionTerminated()),
                            this, SLOT(connectionTerminated()));
    Q_ASSERT_X(connect, "init", "cannot connect terminating");

    connect = this->connect(this->m_contextNetwork, SIGNAL(connectionStatusChanged(uint, uint)),
                            this, SLOT(connectionStatusChanged(uint, uint)));
    Q_ASSERT_X(connect, "init", "cannot connect change connection status");

    connect = this->connect(this->m_contextSettings, SIGNAL(changedNetworkSettings()),
                            this, SLOT(changedNetworkSettings()));
    Q_ASSERT_X(connect, "init", "cannot connect change network status");

    connect = this->connect(this->m_contextNetwork, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)),
                            this, SLOT(textMessageReceived(BlackMisc::Network::CTextMessageList)));
    Q_ASSERT_X(connect, "init", "cannot connect text message received");

    connect = this->connect(this->m_timerUpdateAircraftsInRange, SIGNAL(timeout()),
                            this, SLOT(updateTimer()));
    Q_ASSERT_X(connect, "init", "cannot connect timer");

    connect = this->connect(this->m_timerUpdateAtcStationsOnline, SIGNAL(timeout()),
                            this, SLOT(updateTimer()));
    Q_ASSERT_X(connect, "init", "cannot connect timer");

    connect = this->connect(this->m_timerContextWatchdog, SIGNAL(timeout()),
                            this, SLOT(updateTimer()));
    Q_ASSERT_X(connect, "init", "cannot connect timer (watchdog)");

    connect = this->connect(this->m_timerCollectedCockpitUpdates, SIGNAL(timeout()),
                            this, SLOT(sendCockpitUpdates()));
    Q_ASSERT_X(connect, "init", "cannot connect timer (cockpit updates)");

    //
    // GUI
    //
    connect = this->connect(this->ui->tw_AtcStations, SIGNAL(currentChanged(int)),
                            this, SLOT(atcStationTabChanged(int)));
    Q_ASSERT_X(connect, "init", "cannot connect stations tab changed");

    connect = this->connect(this->ui->sw_MainMiddle, SIGNAL(currentChanged(int)),
                            this, SLOT(middlePanelChanged(int)));
    Q_ASSERT_X(connect, "init", "cannot connect middle panle changed");
    Q_UNUSED(connect);


    // start timers
    this->m_timerUpdateAircraftsInRange->start(10 * 1000);
    this->m_timerUpdateAtcStationsOnline->start(10 * 1000);
    this->m_timerContextWatchdog->start(2 * 1000);

    // data
    this->initialDataReads();

    // start screen
    this->setMainPage(true);

    // do this as last statement, so it can be used as flag
    // whether init has been completed
    this->m_init = true;
}

/*
 * Init data when started
 */
void MainWindow::initialDataReads()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = (this->m_contextNetwork->usingLocalObjects() || (this->m_contextApplication->ping(t) == t));
    if (!this->m_coreAvailable)
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError,
                                   "No initial data read as network context is not available"));
        return;
    }

    this->reloadSettings(); // init read
    this->reloadAtcStationsBooked(); // init read, to do this no traffic network required
    this->reloadOwnAircraft(); // init read, independent of traffic network

    if (this->m_contextNetwork->isConnected())
    {
        // connection is already established
        this->reloadAircraftsInRange();
        this->updateGuiStatusInformation();
    }
}
