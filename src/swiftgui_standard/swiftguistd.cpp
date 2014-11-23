/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistd.h"
#include "ui_swiftguistd.h"
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
#include "blackmisc/notificationsounds.h"
#include <QMouseEvent>
#include <QMainWindow>

using namespace BlackCore;
using namespace BlackSound;
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
SwiftGuiStd::SwiftGuiStd(BlackGui::CEnableForFramelessWindow::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, CEnableForFramelessWindow::modeToWindowFlags(windowMode)),
    CEnableForFramelessWindow(windowMode, true, this),
    ui(new Ui::SwiftGuiStd)
{
    // GUI
    ui->setupUi(this);
    this->ui->wi_CentralWidgetOutside->setProperty("mainframeless", this->isFrameless());
    this->m_compInfoWindow = new CInfoWindowComponent(this); // setupUi has to be first!
}

/*
 * Destructor
 */
SwiftGuiStd::~SwiftGuiStd()
{ }

/*
 * Graceful shutdown
 */
void SwiftGuiStd::performGracefulShutdown()
{
    if (!this->m_init) { return; }
    this->m_init = false;

    // tell GUI components to shut down
    emit requestGracefulShutdown();

    // tell context GUI is going down
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
void SwiftGuiStd::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    this->performGracefulShutdown();
    // if (this->sender() != this) QMainWindow::closeEvent(event);
    QApplication::exit();
}

/*
 * Set main page
 */
void SwiftGuiStd::ps_setMainPage(SwiftGuiStd::MainPageIndex mainPage)
{
    this->ui->sw_MainMiddle->setCurrentIndex(mainPage);
}

void SwiftGuiStd::ps_setMainPageInfoArea(CMainInfoAreaComponent::InfoArea infoArea)
{
    this->ps_setMainPageToInfoArea();
    this->ui->comp_MainInfoArea->selectArea(infoArea);
}

/*
 * Given main page selected?
 */
bool SwiftGuiStd::isMainPageSelected(SwiftGuiStd::MainPageIndex mainPage) const
{
    return this->ui->sw_MainMiddle->currentIndex() == static_cast<int>(mainPage);
}

void SwiftGuiStd::ps_loginRequested()
{
    if (this->ui->sw_MainMiddle->currentIndex() == static_cast<int>(MainPageLogin))
    {
        // already main page, we fake a re-trigger here
        emit this->currentMainInfoAreaChanged(this->ui->sw_MainMiddle->currentWidget());
    }
    else
    {
        this->ps_setMainPage(MainPageLogin);
    }
}

/*
 * Is the network context available?
 */
bool SwiftGuiStd::isContextNetworkAvailableCheck()
{
    if (this->m_contextNetworkAvailable) return true;
    this->ps_displayStatusMessageInGui(CLogMessage(this).error("Network context not available, no updates this time"));
    return false;
}

/*
 * Is the audio context available?
 */
bool SwiftGuiStd::isContextAudioAvailableCheck()
{
    if (this->m_contextAudioAvailable) return true;
    this->ps_displayStatusMessageInGui(CLogMessage(this).error("Voice context not available"));
    return false;
}

/*
 * Display a status message
 */
void SwiftGuiStd::ps_displayStatusMessageInGui(const CStatusMessage &statusMessage)
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

void SwiftGuiStd::ps_onChangedSetttings(uint typeValue)
{
    IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
    if (type == IContextSettings::SettingsHotKeys) this->ps_registerHotkeyFunctions();
}

/*
* Connection terminated
*/
void SwiftGuiStd::ps_onConnectionTerminated()
{
    this->updateGuiStatusInformation();
}

/*
* Connection status changed
*/
void SwiftGuiStd::ps_onConnectionStatusChanged(uint /** from **/, uint to, const QString & /* message */)
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
void SwiftGuiStd::ps_handleTimerBasedUpdates()
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
void SwiftGuiStd::setContextAvailability()
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    this->m_coreAvailable = this->getIContextApplication()->ping(t) == t;
    this->m_contextNetworkAvailable = this->m_coreAvailable || this->getIContextNetwork()->isUsingImplementingObject();
    this->m_contextAudioAvailable = this->m_coreAvailable || this->getIContextAudio()->isUsingImplementingObject();
}

/*
* Update GUI
*/
void SwiftGuiStd::updateGuiStatusInformation()
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
void SwiftGuiStd::ps_changeWindowOpacity(int opacity)
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
void SwiftGuiStd::ps_toogleWindowStayOnTop()
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
void SwiftGuiStd::ps_registerHotkeyFunctions()
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
void SwiftGuiStd::ps_onStyleSheetsChanged()
{
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameMainWindow()
    }
    );
    this->setStyleSheet(s);
}

void SwiftGuiStd::ps_onCurrentMainWidgetChanged(int currentIndex)
{
    emit currentMainInfoAreaChanged(this->ui->sw_MainMiddle->currentWidget());
    Q_UNUSED(currentIndex);
}

/*
 * Notification
 */
void SwiftGuiStd::playNotifcationSound(CNotificationSounds::Notification notification) const
{
    if (!this->m_contextAudioAvailable) return;
    if (!this->ui->comp_MainInfoArea->getSettingsComponent()->playNotificationSounds()) return;
    this->getIContextAudio()->playNotification(static_cast<uint>(notification), true);
}
