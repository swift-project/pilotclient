#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/context_application.h"
#include "blackcore/network.h"
#include "blackmisc/avaircraft.h"
#include <QMouseEvent>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Audio;

/*
 * Constructor
 */
MainWindow::MainWindow(GuiModes::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, windowMode == GuiModes::WindowFrameless ? (Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint) : (Qt::Tool | Qt::WindowStaysOnTopHint)),
    ui(new Ui::MainWindow),
    m_compInfoWindow(nullptr),
    m_init(false), m_windowMode(windowMode), m_audioTestRunning(NoAudioTest),
    // contexts and runtime
    m_coreAvailable(false), m_contextNetworkAvailable(false), m_contextAudioAvailable(false),

    // timers
    m_timerContextWatchdog(nullptr),
    m_timerStatusBar(nullptr), m_timerAudioTests(nullptr), m_timerSimulator(nullptr),
    // context menus
    m_contextMenuAudio(nullptr), m_contextMenuStatusMessageList(nullptr),
    // cockpit
    m_inputFocusedWidget(nullptr),
    // status bar
    m_statusBarIcon(nullptr), m_statusBarLabel(nullptr),
    // keyboard /hotkeys
    m_keyboard(nullptr)
{
    if (windowMode == GuiModes::WindowFrameless)
    {
        // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
        this->setAttribute(Qt::WA_NoSystemBackground, true);
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        // this->setAttribute(Qt::WA_PaintOnScreen);
    }

    // GUI
    ui->setupUi(this);
    this->m_compInfoWindow = new CInfoWindowComponent(this); // setupUi has to be first!

}

/*
 * Destructor
 */
MainWindow::~MainWindow()
{
    this->disconnect(this->getIContextSimulator(), &IContextSimulator::connectionChanged, this, &MainWindow::simulatorConnectionChanged);
}

/*
 * Graceful shutdown
 */
void MainWindow::gracefulShutdown()
{
    if (!this->m_init) return;
    this->m_init = false;

    if (this->m_keyboard) this->m_keyboard->unregisterAllHotkeys();

    if (this->getIContextApplication())
        this->getIContextApplication()->notifyAboutComponentChange(IContextApplication::ComponentGui, IContextApplication::ActionStops);

    // close info window
    if (this->m_compInfoWindow)
    {
        this->m_compInfoWindow->close();
        this->m_compInfoWindow = nullptr;
    }

    // shut down all timers
    this->stopAllTimers(true);

    // if we have a context, we shut some things down
    if (this->m_contextNetworkAvailable)
    {
        if (this->getIContextNetwork()->isConnected())
        {
            if (this->m_contextAudioAvailable)
            {
                this->getIContextAudio()->leaveAllVoiceRooms();
                this->getIContextAudio()->disconnect(this); // break down signal / slots
            }
            this->getIContextNetwork()->disconnectFromNetwork();
            this->getIContextNetwork()->disconnect(this); // avoid any status update signals, etc.
        }
    }
}

/*
 * Close event, window closes
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    this->gracefulShutdown();
    // if (this->sender() != this) QMainWindow::closeEvent(event);
    QApplication::exit();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // this->ui->fr_PseudoWindowBar->geometry().contains(event->pos())
    if (this->m_windowMode == GuiModes::WindowFrameless && event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - this->m_dragPosition);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (this->m_windowMode == GuiModes::WindowFrameless && event->button() == Qt::LeftButton)
    {
        this->m_dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
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
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageStatus);
    else if (sender == this->ui->pb_MainAtc)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageAtc);
    else if (sender == this->ui->pb_MainAircrafts)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageAircrafts);
    else if (sender == this->ui->pb_MainCockpit)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageCockpit);
    else if (sender == this->ui->pb_MainUsers)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageUsers);
    else if (sender == this->ui->pb_MainTextMessages)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageTextMessages);
    else if (sender == this->ui->pb_MainFlightplan)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageFlightplan);
    else if (sender == this->ui->pb_MainSettings)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageSettings);
    else if (sender == this->ui->pb_MainSimulator)
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageSimulator);
}

/*
 * Set main page
 */
void MainWindow::setMainPage(MainWindow::MainPageIndex mainPage)
{
    this->ui->sw_MainMiddle->setCurrentIndex(mainPage);
}

/*
 * Given main page selected?
 */
bool MainWindow::isMainPageSelected(MainWindow::MainPageIndex mainPage) const
{
    return this->ui->sw_MainMiddle->currentIndex() == static_cast<int>(mainPage);
}

/*
 * Connect to Network
 */
void MainWindow::toggleNetworkConnection()
{
    CStatusMessageList msgs;
    if (!this->isContextNetworkAvailableCheck()) return;

    this->ui->lbl_StatusNetworkConnectedIcon->setPixmap(this->m_resPixmapConnectionConnecting);
    if (!this->getIContextNetwork()->isConnected())
    {
        // validation of data here is not required, network context does this
        // in prephase of login
        this->m_ownAircraft.setCallsign(this->ui->le_SettingsAircraftCallsign->text());
        CAircraftIcao icao = this->m_ownAircraft.getIcaoInfo();
        icao.setAirlineDesignator(this->ui->le_SettingsIcaoAirlineDesignator->text());
        icao.setAircraftDesignator(this->ui->le_SettingsIcaoAircraftDesignator->text());
        icao.setAircraftCombinedType(this->ui->le_SettingsIcaoCombinedType->text());
        this->m_ownAircraft.setIcaoInfo(icao);

        // set latest aircraft
        this->getIContextOwnAircraft()->updateOwnAircraft(this->m_ownAircraft, MainWindow::sampleBlackGuiOriginator());

        // flight plan
        this->ui->comp_Flightplan->prefillWithAircraftData(this->m_ownAircraft);

        // Login is based on setting current server
        INetwork::LoginMode mode = INetwork::LoginNormal;
        if (this->ui->rb_SettingsLoginStealthMode->isChecked())
        {
            mode = INetwork::LoginStealth;
            this->displayStatusMessage(CStatusMessage::getInfoMessage("login in stealth mode"));
        }
        else if (this->ui->rb_SettingsLoginObserver->isChecked())
        {
            mode = INetwork::LoginAsObserver;
            this->displayStatusMessage(CStatusMessage::getInfoMessage("login in observer mode"));
        }
        msgs = this->getIContextNetwork()->connectToNetwork(static_cast<uint>(mode));
    }
    else
    {
        // disconnect from network
        this->stopUpdateTimers(); // stop update timers, to avoid updates during disconnecting (a short time frame)
        if (this->m_contextAudioAvailable) this->getIContextAudio()->leaveAllVoiceRooms();
        msgs = this->getIContextNetwork()->disconnectFromNetwork();
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
 * Is the audio context available?
 */
bool MainWindow::isContextAudioAvailableCheck()
{
    if (this->m_contextAudioAvailable) return true;
    this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError, "Voice context not available"));
    return false;
}

/*
 * Display a status message
 */
void MainWindow::displayStatusMessage(const CStatusMessage &statusMessage)
{
    if (!this->m_init) return;
    this->ui->sb_MainStatusBar->show();
    this->m_timerStatusBar->start(3000);
    this->m_statusBarIcon->setPixmap(statusMessage.toIcon());
    this->m_statusBarLabel->setText(statusMessage.getMessage());

    // list
    this->ui->tvp_StatusMessages->insert(statusMessage);

    // display overlay for errors, but not for validation
    if (statusMessage.getSeverity() == CStatusMessage::SeverityError && statusMessage.getType() != CStatusMessage::TypeValidation)
        this->m_compInfoWindow->displayStatusMessage(statusMessage);
}

/*
 * Display a status message
 */
void MainWindow::displayStatusMessages(const CStatusMessageList &messages)
{
    if (!this->m_init || messages.isEmpty()) return;
    foreach(CStatusMessage msg, messages)
    {
        this->displayStatusMessage(msg);
    }
}

void MainWindow::displayRedirectedOutput(const CStatusMessage &statusMessage, qint64 contextId)
{
    if (!this->getIContextApplication()) return;
    if (this->getIContextApplication()->getUniqueId() == contextId) return; //self triggered
    this->ui->te_StatusPageConsole->appendHtml(statusMessage.toHtml());
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
void MainWindow::connectionStatusChanged(uint /** from **/, uint to, const QString & /* message */)
{
    this->updateGuiStatusInformation();
    INetwork::ConnectionStatus newStatus = static_cast<INetwork::ConnectionStatus>(to);
    if (newStatus == INetwork::Connected)
        this->startUpdateTimers();
    else if (newStatus == INetwork::Disconnecting || newStatus == INetwork::Disconnected || newStatus == INetwork::DisconnectedError)
        this->stopUpdateTimers();

    // sounds
    switch (newStatus)
    {
    case INetwork::Connected:
        this->playNotifcationSound(BlackSound::CNotificationSounds::NotificationLogin);
        break;
    case INetwork::Disconnected:
        this->playNotifcationSound(BlackSound::CNotificationSounds::NotificationLogoff);
        break;
    case INetwork::DisconnectedError:
        this->playNotifcationSound(BlackSound::CNotificationSounds::NotificationError);
        break;
    default:
        break;
    }
}

/*
* Timer event
*/
void MainWindow::timerBasedUpdates()
{
    QObject *sender = QObject::sender();
    if (sender == this->m_timerContextWatchdog)
    {
        this->setContextAvailability();
        this->updateGuiStatusInformation();
    }
    else if (sender == this->m_timerSimulator)
    {
        this->updateSimulatorData();
    }

    // own aircraft
    this->reloadOwnAircraft(); // regular updates
}

/*
* Context availability
*/
void MainWindow::setContextAvailability()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = this->getIContextApplication()->ping(t) == t;
    this->m_contextNetworkAvailable = this->m_coreAvailable || this->getIContextNetwork()->usingLocalObjects();
    this->m_contextAudioAvailable = this->m_coreAvailable || this->getIContextAudio()->usingLocalObjects();
}

/*
* Update GUI
*/
void MainWindow::updateGuiStatusInformation()
{
    const QString now = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    QString network("unavailable");
    if (this->m_contextNetworkAvailable)
    {
        bool dbus = !this->getIContextNetwork()->usingLocalObjects();
        network =  dbus ? now : "local";
        this->ui->cb_StatusWithDBus->setChecked(dbus);
    }

    // handle voice, mute
    QString voice("unavailable");
    if (this->m_contextAudioAvailable)
    {
        voice = this->getIContextAudio()->usingLocalObjects() ? "local" : now;
        this->ui->pb_SoundMute->setEnabled(true);
    }
    else
    {
        // voice not available
        this->ui->pb_SoundMute->setEnabled(false);
    }

    // update status fields
    this->ui->le_StatusNetworkContext->setText(network);
    this->ui->le_StatusAudioContext->setText(voice);

    // Connected button
    if (this->m_contextNetworkAvailable && this->getIContextNetwork()->isConnected())
    {
        if (this->ui->lbl_StatusNetworkConnectedIcon->toolTip().startsWith("dis", Qt::CaseInsensitive))
            this->ui->lbl_StatusNetworkConnectedIcon->setToolTip(now);
        this->ui->pb_MainConnect->setText("Disconnect");
        this->ui->pb_MainConnect->setStyleSheet("background-color: green");
        this->ui->lbl_StatusNetworkConnectedIcon->setPixmap(this->m_resPixmapConnectionConnected);
    }
    else
    {
        this->ui->lbl_StatusNetworkConnectedIcon->setToolTip("disconnected");
        this->ui->pb_MainConnect->setText("Connect");
        this->ui->pb_MainConnect->setStyleSheet("background-color: ");
        this->ui->lbl_StatusNetworkConnectedIcon->setPixmap(this->m_resPixmapConnectionDisconnected);
    }
}

/*
 * Opacity 0-100
 */
void MainWindow::changeWindowOpacity(int opacity)
{
    if (opacity < 0)
    {
        QObject *sender = QObject::sender();
        if (sender == this->ui->pb_MainKeypadOpacity050)
            opacity = 50;
        else if (sender == this->ui->pb_MainKeypadOpacity100)
            opacity = 100;
        else
            return;
    }
    qreal o = opacity / 100.0;
    o = o < 0.3 ? 0.3 : o;
    QWidget::setWindowOpacity(o);
    this->ui->hs_SettingsGuiOpacity->setValue(o * 100.0);
}

void MainWindow::updateSimulatorData()
{
    if (this->getIContextSimulator()->isConnected())
        ui->le_SimulatorStatus->setText("Connected");
    else
        ui->le_SimulatorStatus->setText("Not connected");

    CAircraft ownAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
    ui->le_SimulatorLatitude->setText(ownAircraft.getSituation().latitude().toFormattedQString());
    ui->le_SimulatorLongitude->setText(ownAircraft.getSituation().longitude().toFormattedQString());
    ui->le_SimulatorAltitude->setText(ownAircraft.getSituation().getAltitude().toFormattedQString());
    ui->le_SimulatorPitch->setText(ownAircraft.getSituation().getPitch().toFormattedQString());
    ui->le_SimulatorBank->setText(ownAircraft.getSituation().getBank().toFormattedQString());
    ui->le_SimulatorHeading->setText(ownAircraft.getSituation().getHeading().toFormattedQString());
    ui->le_SimulatorGroundSpeed->setText(ownAircraft.getSituation().getGroundSpeed().toFormattedQString());

    ui->le_SimulatorCom1Active->setText(ownAircraft.getCom1System().getFrequencyActive().toFormattedQString());
    ui->le_SimulatorCom1Standby->setText(ownAircraft.getCom1System().getFrequencyStandby().toFormattedQString());
    ui->le_SimulatorCom2Active->setText(ownAircraft.getCom2System().getFrequencyActive().toFormattedQString());
    ui->le_SimulatorCom2Standby->setText(ownAircraft.getCom2System().getFrequencyStandby().toFormattedQString());
    ui->le_SimulatorTransponder->setText(ownAircraft.getTransponderCodeFormatted());
}

void MainWindow::simulatorConnectionChanged(bool isAvailable)
{
    // Simulator timer, TODO remove later
    if (isAvailable)
        this->m_timerSimulator->start(500);
    else
        this->m_timerSimulator->stop();
}

/*
 * Stay on top
 */
void MainWindow::toogleWindowStayOnTop()
{
    Qt::WindowFlags flags = this->windowFlags();
    if (Qt::WindowStaysOnTopHint & flags)
    {
        flags ^= Qt::WindowStaysOnTopHint;
        flags |= Qt::WindowStaysOnBottomHint;
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityInfo, "Window on bottom"));
    }
    else
    {
        flags ^= Qt::WindowStaysOnBottomHint;
        flags |= Qt::WindowStaysOnTopHint;
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeGui, CStatusMessage::SeverityInfo, "Window on top"));
    }
    this->setWindowFlags(flags);
    this->show();
}
