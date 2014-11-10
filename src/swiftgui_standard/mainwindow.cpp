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
#include "blackcore/context_ownaircraft.h"
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
    QMainWindow(parent, windowMode == GuiModes::WindowFrameless ?
                (Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint) :
                (Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)),
    ui(new Ui::MainWindow),
    m_windowMode(windowMode)
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
    if (!this->m_init) { return; }
    this->m_init = false;

    if (this->getIContextApplication())
    {
        this->getIContextApplication()->notifyAboutComponentChange(IContextApplication::ApplicationGui, IContextApplication::ApplicationStops);
    }

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
        this->getIContextOwnAircraft()->updateOwnAircraft(this->m_ownAircraft, MainWindow::swiftGuiStandardOriginator());

        // flight plan
        this->ui->comp_MainInfoArea->getFlightPlanComponent()->prefillWithAircraftData(this->m_ownAircraft);

        // Login is based on setting current server
        INetwork::LoginMode mode = INetwork::LoginNormal;
        if (this->ui->comp_MainInfoArea->getSettingsComponent()->loginStealth())
        {
            mode = INetwork::LoginStealth;
            this->ps_displayStatusMessageInGui(CLogMessage(this).info("login in stealth mode"));
        }
        else if (this->ui->comp_MainInfoArea->getSettingsComponent()->loginAsObserver())
        {
            mode = INetwork::LoginAsObserver;
            this->ps_displayStatusMessageInGui(CLogMessage(this).info("login in observer mode"));
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

void MainWindow::ps_loginRequested()
{
    this->ps_setMainPage(MainPageLogin);
}

/*
 * Is the network context available?
 */
bool MainWindow::isContextNetworkAvailableCheck()
{
    if (this->m_contextNetworkAvailable) return true;
    this->ps_displayStatusMessageInGui(CLogMessage(this).error("Network context not available, no updates this time"));
    return false;
}

/*
 * Is the audio context available?
 */
bool MainWindow::isContextAudioAvailableCheck()
{
    if (this->m_contextAudioAvailable) return true;
    this->ps_displayStatusMessageInGui(CLogMessage(this).error("Voice context not available"));
    return false;
}

/*
 * Display a status message
 */
void MainWindow::ps_displayStatusMessageInGui(const CStatusMessage &statusMessage)
{
    if (!this->m_init) return;
    if (statusMessage.isRedundant()) return;
    if (statusMessage.wasHandledBy(this)) return;
    statusMessage.markAsHandledBy(this);
    this->m_statusBar.displayStatusMessage(statusMessage);
    this->ui->comp_MainInfoArea->displayStatusMessage(statusMessage);

    // list
    this->ui->comp_MainInfoArea->getLogComponent()->appendStatusMessageToList(statusMessage);

    // display overlay for errors, but not for validation
    if (statusMessage.getSeverity() == CStatusMessage::SeverityError && ! statusMessage.getCategories().contains(CLogCategory::validation()))
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
    this->m_contextNetworkAvailable = this->m_coreAvailable || this->getIContextNetwork()->isUsingImplementingObject();
    this->m_contextAudioAvailable = this->m_coreAvailable || this->getIContextAudio()->isUsingImplementingObject();
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
        bool dbus = !this->getIContextNetwork()->isUsingImplementingObject();
        network =  dbus ? now : "local";
        this->ui->comp_InfoBarStatus->setDBusStatus(dbus);
    }

    // update status fields
    QString s = QString("network: %1").arg(network);
    this->ui->comp_InfoBarStatus->setDBusTooltip(s);
}

/*
 * Opacity 0-100
 */
void MainWindow::ps_changeWindowOpacity(int opacity)
{
    qreal o = opacity / 100.0;
    o = o < 0.3 ? 0.3 : o;
    o = o > 1.0 ? 1.0 : o;
    QWidget::setWindowOpacity(o);
    this->ui->comp_MainInfoArea->getSettingsComponent()->setGuiOpacity(o * 100.0);
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
        this->ps_displayStatusMessageInGui(CLogMessage(this).info("Window on bottom"));
    }
    else
    {
        flags ^= Qt::WindowStaysOnBottomHint;
        flags |= Qt::WindowStaysOnTopHint;
        this->ps_displayStatusMessageInGui(CLogMessage(this).info("Window on top"));
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
