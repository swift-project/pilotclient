/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackmisc/icon.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/context_application.h"
#include "blackcore/network.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/logmessage.h"
#include <QMouseEvent>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Hardware;

/*
 * Constructor
 */
MainWindow::MainWindow(GuiModes::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, windowMode == GuiModes::WindowFrameless ? (Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint) : (Qt::Tool | Qt::WindowStaysOnTopHint)),
    ui(new Ui::MainWindow),
    m_compInfoWindow(nullptr),
    m_init(false), m_windowMode(windowMode), m_keyboard(nullptr),
    // contexts and runtime
    m_coreAvailable(false), m_contextNetworkAvailable(false), m_contextAudioAvailable(false),

    // timers
    m_timerContextWatchdog(nullptr),
    m_timerStatusBar(nullptr), m_timerSimulator(nullptr),
    // context menus
    m_contextMenuStatusMessageList(nullptr),
    // cockpit
    m_inputFocusedWidget(nullptr),
    // status bar
    m_statusBarIcon(nullptr), m_statusBarLabel(nullptr)
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
{ }

/*
 * Graceful shutdown
 */
void MainWindow::gracefulShutdown()
{
    if (!this->m_init) return;
    this->m_init = false;

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
        if (this->getIContextNetwork() && this->getIContextNetwork()->isConnected())
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

    if (this->getIContextSimulator())
        this->disconnect(this->getIContextSimulator(), &IContextSimulator::connectionChanged, this, &MainWindow::ps_onSimulatorConnectionChanged);
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
void MainWindow::ps_setMainPage(bool start)
{
    if (start)
    {
        this->ui->sw_MainMiddle->setCurrentIndex(0);
        return;
    }

    QObject *sender = QObject::sender();
    if (sender == this->ui->pb_MainCockpit)
    {
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageCockpit);
    }
    else
    {
        this->ui->sw_MainMiddle->setCurrentIndex(MainPageFoo);

        if (sender == this->ui->pb_MainAircrafts)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaAircrafts);
        }
        if (sender == this->ui->pb_MainAtc)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaAtc);
        }
        else if (sender == this->ui->pb_MainUsers)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaUsers);
        }
        else if (sender == this->ui->pb_MainTextMessages)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaTextMessages);
        }
        else if (sender == this->ui->pb_MainFlightplan)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaFlightPlan);
        }
        else if (sender == this->ui->pb_MainSettings)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaSettings);
        }
        else if (sender == this->ui->pb_MainSimulator)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaSimulator);
        }
        else if (sender == this->ui->pb_MainWeather)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaWeather);
        }
        else if (sender == this->ui->pb_MainLog)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaLog);
        }
        else if (sender == this->ui->pb_MainMappings)
        {
            this->ui->comp_MainInfoArea->selectArea(CMainInfoAreaComponent::InfoAreaMappings);
        }
    }
}

/*
 * Set main page
 */
void MainWindow::ps_setMainPage(MainWindow::MainPageIndex mainPage)
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
void MainWindow::ps_toggleNetworkConnection()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    if (!this->getIContextNetwork()->isConnected())
    {
        // validation of data here is not required, network context does this
        // in prephase of login
        this->m_ownAircraft.setCallsign(this->ui->comp_MainInfoArea->getSettingsComponent()->getOwnCallsignFromGui());
        CAircraftIcao icao = this->m_ownAircraft.getIcaoInfo();
        this->ui->comp_MainInfoArea->getSettingsComponent()->setOwnAircraftIcaoDataFromGui(icao);
        this->m_ownAircraft.setIcaoInfo(icao);

        // set latest aircraft
        this->getIContextOwnAircraft()->updateOwnAircraft(this->m_ownAircraft, MainWindow::sampleBlackGuiOriginator());

        // flight plan
        this->ui->comp_MainInfoArea->getFlightPlanComponent()->prefillWithAircraftData(this->m_ownAircraft);

        // Login is based on setting current server
        INetwork::LoginMode mode = INetwork::LoginNormal;
        if (this->ui->comp_MainInfoArea->getSettingsComponent()->loginStealth())
        {
            mode = INetwork::LoginStealth;
            this->ps_displayStatusMessageInGui(CLogMessage().info(this, "login in stealth mode"));
        }
        else if (this->ui->comp_MainInfoArea->getSettingsComponent()->loginAsObserver())
        {
            mode = INetwork::LoginAsObserver;
            this->ps_displayStatusMessageInGui(CLogMessage().info(this, "login in observer mode"));
        }
        CStatusMessage msg = this->getIContextNetwork()->connectToNetwork(static_cast<uint>(mode));
        this->ps_displayStatusMessageInGui(msg);
        this->startUpdateTimersWhenConnected();
    }
    else
    {
        // disconnect from network
        this->stopUpdateTimersWhenDisconnected(); // stop update timers, to avoid updates during disconnecting (a short time frame)
        if (this->m_contextAudioAvailable) this->getIContextAudio()->leaveAllVoiceRooms();
        CStatusMessage msg = this->getIContextNetwork()->disconnectFromNetwork();
        this->ps_displayStatusMessageInGui(msg);
    }
}

/*
 * Is the network context available?
 */
bool MainWindow::isContextNetworkAvailableCheck()
{
    if (this->m_contextNetworkAvailable) return true;
    this->ps_displayStatusMessageInGui(CLogMessage().error(this, "Network context not available, no updates this time"));
    return false;
}

/*
 * Is the audio context available?
 */
bool MainWindow::isContextAudioAvailableCheck()
{
    if (this->m_contextAudioAvailable) return true;
    this->ps_displayStatusMessageInGui(CLogMessage().error(this, "Voice context not available"));
    return false;
}

/*
 * Display a status message
 */
void MainWindow::ps_displayStatusMessageInGui(const CStatusMessage &statusMessage)
{
    if (statusMessage.isRedundant()) return;
    if (statusMessage.getSeverity() == CStatusMessage::SeverityDebug) return;

    if (!this->m_init) return;
    this->ui->sb_MainStatusBar->show();
    this->m_timerStatusBar->start(3000);
    this->m_statusBarIcon->setPixmap(statusMessage.toPixmap());
    this->m_statusBarLabel->setText(statusMessage.getMessage());

    // list
    this->ui->comp_MainInfoArea->getLogComponent()->appendStatusMessageToList(statusMessage);

    // display overlay for errors, but not for validation
    // TODO smarter use of CLogCategoryHandler to dispatch different categories of message to different MainWindow slots
    if (statusMessage.getSeverity() == CStatusMessage::SeverityError && ! statusMessage.getCategory().endsWith(".validation"))
    {
        this->m_compInfoWindow->displayStatusMessage(statusMessage);
    }
}

void MainWindow::ps_onChangedSetttings(uint typeValue)
{
    IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
    if (type == IContextSettings::SettingsHotKeys) this->ps_registerHotkeyFunctions();
}

/*
* Connection terminated
*/
void MainWindow::ps_onConnectionTerminated()
{
    this->updateGuiStatusInformation();
}

/*
* Connection status changed
*/
void MainWindow::ps_onConnectionStatusChanged(uint /** from **/, uint to, const QString & /* message */)
{
    this->updateGuiStatusInformation();
    INetwork::ConnectionStatus newStatus = static_cast<INetwork::ConnectionStatus>(to);

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
void MainWindow::ps_handleTimerBasedUpdates()
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
    this->ps_reloadOwnAircraft(); // regular updates
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
        this->ui->comp_InfoBarStatus->setDBusStatus(dbus);
    }

    // update status fields
    QString s = QString("network: %1").arg(network);
    this->ui->comp_InfoBarStatus->setDBusTooltip(s);

    // Connected button
    if (this->m_contextNetworkAvailable && this->getIContextNetwork()->isConnected())
    {
        this->ui->pb_MainConnect->setText("Disconnect");
        this->ui->pb_MainConnect->setStyleSheet("background-color: green");
    }
    else
    {
        this->ui->pb_MainConnect->setText("Connect");
        this->ui->pb_MainConnect->setStyleSheet("background-color: ");
    }
}

/*
 * Opacity 0-100
 */
void MainWindow::ps_changeWindowOpacity(int opacity)
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
    this->ui->comp_MainInfoArea->getSettingsComponent()->setGuiOpacity(o * 100.0);
}

void MainWindow::updateSimulatorData()
{
    CSimulatorComponent *simComp = this->ui->comp_MainInfoArea->getSimulatorComponent();
    Q_ASSERT(simComp);
    if (!this->getIContextSimulator()->isConnected())
    {
        simComp->clear();
        simComp->addOrUpdateByName("info", "sim not connected", CIcons::StandardIconWarning16);
        return;
    }

    // clear old warnings / information
    if (simComp->rowCount() < 5)
    {
        simComp->clear();
    }

    CAircraft ownAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
    CAircraftSituation s = ownAircraft.getSituation();
    CComSystem c1 = ownAircraft.getCom1System();
    CComSystem c2 = ownAircraft.getCom2System();

    simComp->addOrUpdateByName("latitude", s.latitude().toFormattedQString(), s.latitude().toIcon());
    simComp->addOrUpdateByName("longitude", s.longitude().toFormattedQString(), s.longitude().toIcon());
    simComp->addOrUpdateByName("altitude", s.getAltitude().toFormattedQString(), s.getAltitude().toIcon());
    simComp->addOrUpdateByName("pitch", s.getPitch().toFormattedQString(), CIcons::AviationAttitudeIndicator);
    simComp->addOrUpdateByName("bank", s.getBank().toFormattedQString(), CIcons::AviationAttitudeIndicator);
    simComp->addOrUpdateByName("heading", s.getHeading().toFormattedQString(), s.getHeading().toIcon());
    simComp->addOrUpdateByName("ground speed", s.getGroundSpeed().toFormattedQString(), s.getGroundSpeed().toIcon());

    simComp->addOrUpdateByName("COM1 active", c1.getFrequencyActive().toFormattedQString(), c1.getFrequencyActive().toIcon());
    simComp->addOrUpdateByName("COM2 active", c2.getFrequencyActive().toFormattedQString(), c2.getFrequencyActive().toIcon());
    simComp->addOrUpdateByName("COM1 standby", c1.getFrequencyStandby().toFormattedQString(), c1.getFrequencyStandby().toIcon());
    simComp->addOrUpdateByName("COM2 standby", c2.getFrequencyStandby().toFormattedQString(), c2.getFrequencyStandby().toIcon());
    simComp->addOrUpdateByName("Transponder", ownAircraft.getTransponderCodeFormatted(), ownAircraft.getTransponder().toIcon());
}

void MainWindow::ps_onSimulatorConnectionChanged(bool isAvailable)
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
void MainWindow::ps_toogleWindowStayOnTop()
{
    Qt::WindowFlags flags = this->windowFlags();
    if (Qt::WindowStaysOnTopHint & flags)
    {
        flags ^= Qt::WindowStaysOnTopHint;
        flags |= Qt::WindowStaysOnBottomHint;
        this->ps_displayStatusMessageInGui(CLogMessage().info(this, "Window on bottom"));
    }
    else
    {
        flags ^= Qt::WindowStaysOnBottomHint;
        flags |= Qt::WindowStaysOnTopHint;
        this->ps_displayStatusMessageInGui(CLogMessage().info(this, "Window on top"));
    }
    this->setWindowFlags(flags);
    this->show();
}

/*
 * Hotkey functions
 */
void MainWindow::ps_registerHotkeyFunctions()
{
    CInputManager *m_inputManager = BlackCore::CInputManager::getInstance();

    m_inputManager->registerHotkeyFunc(CHotkeyFunction::Opacity50(), this, [ this ](bool isPressed)
    {
        if (isPressed) this->ps_changeWindowOpacity(50);
    });

    m_inputManager->registerHotkeyFunc(CHotkeyFunction::Opacity100(), this, [ this ](bool isPressed)
    {
        if (isPressed) this->ps_changeWindowOpacity(100);
    });

    m_inputManager->registerHotkeyFunc(CHotkeyFunction::ToogleWindowsStayOnTop(), this, [ this ](bool isPressed)
    {
        if (isPressed) this->ps_toogleWindowStayOnTop();
    });
}

/*
 * Styles
 */
void MainWindow::ps_onStyleSheetsChanged()
{
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameMainWindow()
    }
    );
    this->setStyleSheet(s);
}
