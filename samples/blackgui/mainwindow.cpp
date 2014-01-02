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

/*
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    m_init(false), m_withDBus(true),
    m_dBusConnection("dummy"), m_coreRuntime(nullptr),
    m_atcListOnline(nullptr), m_atcListBooked(nullptr), m_trafficServerList(nullptr), m_aircraftsInRange(nullptr),
    m_contextNetwork(nullptr), m_contextSettings(nullptr),
    m_ownAircraft(),
    m_timerUpdateAtcStationsOnline(nullptr), m_timerUpdateAircraftsInRange(nullptr), m_timerContextWatchdog(nullptr), m_timerCollectedCockpitUpdates(nullptr)
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
 * Read stations
 */
void MainWindow::reloadAtcStationsBooked()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_atcListBooked->update(this->m_contextNetwork->getAtcStationsBooked());
    this->ui->tv_AtcStationsBooked->resizeColumnsToContents();
}

/*
 * Read stations
 */
void MainWindow::reloadAtcStationsOnline()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_atcListOnline->update(this->m_contextNetwork->getAtcStationsOnline());
    this->ui->tv_AtcStationsOnline->resizeColumnsToContents();
}

/*
 * Read aircrafts
 */
void MainWindow::reloadAircraftsInRange()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_aircraftsInRange->update(this->m_contextNetwork->getAircraftsInRange());
    this->ui->tv_AircraftsInRange->resizeColumnsToContents();
}

/*
 * Read own aircraft
 */
bool MainWindow::reloadOwnAircraft()
{
    if (!this->isContextNetworkAvailableCheck()) return false;
    if (this->isCockpitUpdatePending()) return false;
    CAircraft loadedAircraft = this->m_contextNetwork->getOwnAircraft();
    if (loadedAircraft == this->m_ownAircraft) return false;

    // changed aircraft
    this->m_ownAircraft = loadedAircraft;
    this->updateCockpitFromContext();
    return true;
}

/*
 * Connect to Network
 */
void MainWindow::toggleNetworkConnection()
{
    CStatusMessages msgs;
    if (!this->isContextNetworkAvailableCheck()) return;
    if (!this->m_contextNetwork->isConnected())
    {
        QString cs = this->ui->le_SettingsPlaneCallsign->text();
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
void MainWindow::displayStatusMessages(const CStatusMessages &messages)
{
    if (messages.isEmpty()) return;
    foreach(CStatusMessage msg, messages.getMessages())
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
    CStatusMessages msgs;
    if (sender == this->ui->menu_TestLocationsEDRY)
    {
        this->m_contextNetwork->updateOwnPosition(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CLength(0, CLengthUnit::m())),
            CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())
        );
    }
    else if (sender == this->ui->menu_ReloadSettings)
    {
        this->reloadSettings();
        msgs.append(CStatusMessage::getInfoMessage("Settings reloaded"));
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
 * Station selected
 */
void MainWindow::onlineAtcStationSelected(QModelIndex index)
{
    this->ui->te_AtcStationsOnlineInfo->setText(""); // reset
    const CAtcStation stationClicked = this->m_atcListOnline->at(index);
    QString infoMessage;

    if (stationClicked.hasAtis())
    {
        infoMessage.append(stationClicked.getAtis().getMessage());
    }
    if (stationClicked.hasMetar())
    {
        if (!infoMessage.isEmpty()) infoMessage.append("\n\n");
        infoMessage.append(stationClicked.getMetar().getMessage());
    }

    this->ui->te_AtcStationsOnlineInfo->setText(infoMessage);
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
        qint64 t = QDateTime::currentMSecsSinceEpoch();
        m_contextNetworkAvailable = (this->m_contextNetwork->usingLocalObjects() || (this->m_contextNetwork->ping(t) == t));
        this->updateGuiStatusInformation();
    }

    // own aircraft
    if (sender == this->m_timerUpdateAircraftsInRange || sender == this->m_timerUpdateAtcStationsOnline)
    {
        this->reloadOwnAircraft(); // regular updates
    }
}

/*
 * Get METAR
 */
void MainWindow::getMetar(const QString &airportIcaoCode)
{
    if (!this->isContextNetworkAvailableCheck()) return;
    if (!this->m_contextNetwork->isConnected()) return;
    QString icao = airportIcaoCode.isEmpty() ? this->ui->le_AtcStationsOnlineMetar->text().trimmed().toUpper() : airportIcaoCode.trimmed().toUpper();
    this->ui->le_AtcStationsOnlineMetar->setText(icao);
    if (icao.length() != 4) return;
    CInformationMessage metar = this->m_contextNetwork->getMetar(icao);
    if (metar.getType() != CInformationMessage::METAR) return;
    if (metar.isEmpty()) return;
    this->ui->te_AtcStationsOnlineInfo->setText(metar.getMessage());
}

/*
 * ATC station tab changed are changed
 */
void MainWindow::atcStationTabChanged(int /** tabIndex **/)
{
    if (this->isContextNetworkAvailableCheck())
    {
        if (this->ui->tw_AtcStations->currentWidget() == this->ui->tb_AtcStationsBooked)
        {
            if (this->m_atcListBooked->rowCount() < 1)
                this->reloadAtcStationsBooked();
        }
        else if (this->ui->tw_AtcStations->currentWidget() == this->ui->tb_AtcStationsOnline)
        {
            this->reloadAtcStationsOnline();
        }
    }
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
    if (this->m_contextNetworkAvailable)
    {
        const QString now = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
        this->ui->le_networkContextAvailable->setText(
            this->m_contextNetwork->usingLocalObjects() ? "local" :
            now);
        if (this->m_contextNetwork->isConnected())
        {
            this->ui->pb_MainConnect->setText("Disconnect");
            this->ui->pb_MainConnect->setStyleSheet("background-color: green");
            if (!this->ui->le_StatusNetworkConnected->text().startsWith("2"))
                this->ui->le_StatusNetworkConnected->setText(now);
        }
        else
        {
            this->ui->pb_MainConnect->setText("Connect");
            this->ui->pb_MainConnect->setStyleSheet("background-color:");
            this->ui->le_StatusNetworkConnected->setText("Disconnected");
        }
    }
    else
    {
        this->ui->le_networkContextAvailable->setText("Not available");
    }
}
