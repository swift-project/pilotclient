#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Voice;

/*
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    m_init(false), m_withDBus(true),
    m_coreAvailable(false), m_contextNetworkAvailable(false), m_contextVoiceAvailable(false),
    m_dBusConnection("dummy"), m_coreRuntime(nullptr),
    m_atcListOnline(nullptr), m_atcListBooked(nullptr),
    m_trafficServerList(nullptr), m_aircraftsInRange(nullptr),
    m_contextApplication(nullptr),
    m_contextNetwork(nullptr), m_contextVoice(nullptr),
    m_contextSettings(nullptr),
    m_ownAircraft(), m_voiceRoomCom1(), m_voiceRoomCom2(),
    m_timerUpdateAtcStationsOnline(nullptr), m_timerUpdateAircraftsInRange(nullptr),
    m_timerCollectedCockpitUpdates(nullptr), m_timerContextWatchdog(nullptr)
{
    ui->setupUi(this);
}

/*
 * Destructor
 */
MainWindow::~MainWindow()
{
    this->gracefulShutdown();
    delete ui;
}

/*
 * Graceful shutdown
 */
void MainWindow::gracefulShutdown()
{
    if (!this->m_init) return;
    this->m_init = false;

    // if we have a context, we shut some things down
    if (this->m_contextNetworkAvailable)
    {
        this->m_contextNetwork->disconnectFromNetwork();
    }

    if (this->m_contextVoiceAvailable)
    {
        this->m_contextVoice->leaveAllVoiceRooms();
    }

    if (this->m_timerUpdateAircraftsInRange)
    {
        this->m_timerUpdateAircraftsInRange->disconnect(this);
        this->m_timerUpdateAircraftsInRange->stop();
    }
    if (this->m_timerUpdateAtcStationsOnline)
    {
        this->m_timerUpdateAtcStationsOnline->disconnect(this);
        this->m_timerUpdateAtcStationsOnline->stop();
    }
    if (this->m_timerContextWatchdog)
    {
        this->m_timerContextWatchdog->disconnect(this);
        this->m_timerContextWatchdog->stop();
    }
    if (this->m_timerCollectedCockpitUpdates)
    {
        this->m_timerCollectedCockpitUpdates->disconnect(this);
        this->m_timerCollectedCockpitUpdates->stop();
    }
}

/*
 * Select correct main page
 */
void MainWindow::setMainPage(bool start)
{
    if (start)
    {
        this->ui->sw_MainMiddle->setCurrentIndex(0);
        return;
    }

    QObject *sender = QObject::sender();
    if (sender == this->ui->pb_MainConnect || sender == this->ui->pb_MainStatus)
        this->ui->sw_MainMiddle->setCurrentIndex(0);
    else if (sender == this->ui->pb_MainAtc)
        this->ui->sw_MainMiddle->setCurrentIndex(1);
    else if (sender == this->ui->pb_MainAircrafts)
        this->ui->sw_MainMiddle->setCurrentIndex(2);
    else if (sender == this->ui->pb_MainCockpit)
        this->ui->sw_MainMiddle->setCurrentIndex(3);
    else if (sender == this->ui->pb_MainTextMessages)
        this->ui->sw_MainMiddle->setCurrentIndex(4);
    else if (sender == this->ui->pb_MainFlightplan)
        this->ui->sw_MainMiddle->setCurrentIndex(5);
    else if (sender == this->ui->pb_MainSettings)
        this->ui->sw_MainMiddle->setCurrentIndex(6);
}

/*
 * Connect to Network
 */
void MainWindow::toggleNetworkConnection()
{
    CStatusMessageList msgs;
    if (!this->isContextNetworkAvailableCheck()) return;
    if (!this->m_contextNetwork->isConnected())
    {
        QString cs = this->ui->le_SettingsAircraftCallsign->text();
        if (cs.isEmpty())
        {
            this->displayStatusMessage(CStatusMessage::getValidationError("missing callsign"));
            return;
        }

        // Login is based on setting current server
        msgs = this->m_contextNetwork->connectToNetwork();
    }
    else
    {
        msgs = this->m_contextNetwork->disconnectFromNetwork();
        this->m_contextVoice->leaveAllVoiceRooms();
    }
    if (!msgs.isEmpty()) this->displayStatusMessages(msgs);
}

/*
 * Is the network context available?
 */
bool MainWindow::isContextNetworkAvailableCheck()
{
    if (this->m_contextNetworkAvailable) return true;
    this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError, "Network context not available, no updates this time"));
    return false;
}

/*
 * Is the voice context available?
 */
bool MainWindow::isContextVoiceAvailableCheck()
{
    if (this->m_contextVoiceAvailable) return true;
    this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError, "Voice context not available"));
    return false;
}

/*
 * Display a status message
 */
void MainWindow::displayStatusMessage(const CStatusMessage &message)
{
    this->ui->sb_MainStatusBar->showMessage(message.getMessage(), 3000);
    this->ui->te_StatusMessages->insertPlainText(message.toQString(true).append("\n"));
}

/*
 * Display a status message
 */
void MainWindow::displayStatusMessages(const CStatusMessageList &messages)
{
    if (messages.isEmpty()) return;
    foreach(CStatusMessage msg, messages)
    {
        this->displayStatusMessage(msg);
    }
}

/*
 * Menu clicked
 */
void MainWindow::menuClicked()
{
    QObject *sender = QObject::sender();
    CStatusMessageList msgs;

    if (sender == this->ui->menu_TestLocationsEDRY)
    {
        this->setTestPosition("N 049° 18' 17", "E 008° 27' 05", CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
    }
    else if (sender == this->ui->menu_TestLocationsEDNX)
    {
        this->setTestPosition("N 048° 14′ 22", "E 011° 33′ 41", CAltitude(486, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_TestLocationsEDDM)
    {
        this->setTestPosition("N 048° 21′ 14", "E 011° 47′ 10", CAltitude(448, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_TestLocationsEDDF)
    {
        this->setTestPosition("N 50° 2′ 0", "E 8° 34′ 14", CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()));
    }
    else if (sender == this->ui->menu_ReloadSettings)
    {
        this->reloadSettings();
        msgs.push_back(CStatusMessage::getInfoMessage("Settings reloaded"));
    }

    if (!msgs.isEmpty()) this->displayStatusMessages(msgs);
}

/*
* Connection terminated
*/
void MainWindow::connectionTerminated()
{
    this->updateGuiStatusInformation();
}

/*
* Connection status changed
*/
void MainWindow::connectionStatusChanged(uint /** from **/, uint to)
{
    // CContextNetwork::ConnectionStatus statusFrom = static_cast<CContextNetwork::ConnectionStatus>(from);
    CContextNetwork::ConnectionStatus statusTo = static_cast<CContextNetwork::ConnectionStatus>(to);

    // always
    this->updateGuiStatusInformation();

    if (statusTo == CContextNetwork::ConnectionStatusConnected)
    {
        QTimer::singleShot(5 * 1000, this, SLOT(reloadAircraftsInRange()));
        QTimer::singleShot(5 * 1000, this, SLOT(reloadAtcStationsOnline()));
    }
}

/*
* Timer event
*/
void MainWindow::updateTimer()
{
    QObject *sender = QObject::sender();
    if (sender == this->m_timerUpdateAtcStationsOnline)
    {
        int t = this->ui->hs_AtcStationsOnline->value() * 1000;
        this->m_timerUpdateAtcStationsOnline->start(t);
        if (ui->tv_AtcStationsOnline->isVisible())
            this->reloadAtcStationsOnline();
    }
    if (sender == this->m_timerUpdateAircraftsInRange)
    {
        int t = this->ui->hs_AtcStationsOnline->value() * 1000;
        this->m_timerUpdateAircraftsInRange->start(t);
        if (ui->tv_AircraftsInRange->isVisible())
            this->reloadAircraftsInRange();
    }
    else if (sender == this->m_timerContextWatchdog)
    {
        this->setContextAvailability();
        this->updateGuiStatusInformation();
    }

    // own aircraft
    if (sender == this->m_timerUpdateAircraftsInRange || sender == this->m_timerUpdateAtcStationsOnline)
    {
        this->reloadOwnAircraft(); // regular updates
    }
}

/*
* Context availability
*/
void MainWindow::setContextAvailability()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = this->m_contextApplication->ping(t) == t;
    this->m_contextNetworkAvailable = this->m_coreAvailable || this->m_contextNetwork->usingLocalObjects();
    this->m_contextVoiceAvailable = this->m_coreAvailable || this->m_contextVoice->usingLocalObjects();
}

/*
* Middle panel changed
*/
void MainWindow::middlePanelChanged(int /* index */)
{
    if (this->isContextNetworkAvailableCheck())
    {
        // remark, ATC stations is handled by tab changed

        if (this->ui->sw_MainMiddle->currentWidget() == this->ui->pg_AircraftsInRange)
        {
            if (this->m_aircraftsInRange->rowCount() < 1)
                this->reloadAircraftsInRange();
        }
    }
}

/*
* Update GUI
*/
void MainWindow::updateGuiStatusInformation()
{
    const QString now = QDateTime::currentDateTimeUtc().toString("yyyy - MM - dd HH: mm: ss");
    QString network("unavailable");
    if (this->m_contextNetworkAvailable)
    {
        network = this->m_contextNetwork->usingLocalObjects() ? "local" : now;
    }

    QString voice("unavailable");
    if (this->m_contextVoiceAvailable)
    {
        voice = this->m_contextVoice->usingLocalObjects() ? "local" : now;
    }

    this->ui->le_StatusNetworkContext->setText(network);
    this->ui->le_StatusVoiceContext->setText(voice);
    this->ui->cb_StatusWithDBus->setCheckState(this->m_withDBus ? Qt::Checked : Qt::Unchecked);

    // Connected button
    if (this->m_contextNetworkAvailable && this->m_contextNetwork->isConnected())
    {
        this->ui->pb_MainConnect->setText("Disconnect");
        this->ui->pb_MainConnect->setStyleSheet("background-color: green");
        if (!this->ui->le_StatusNetworkConnected->text().startsWith("2"))
            this->ui->le_StatusNetworkConnected->setText(now);
    }
    else
    {
        this->ui->pb_MainConnect->setText("Connect");
        this->ui->pb_MainConnect->setStyleSheet("background-color: ");
        this->ui->le_StatusNetworkConnected->setText("Disconnected");
    }
}
