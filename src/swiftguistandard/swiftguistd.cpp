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
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/context_application.h"
#include "blackcore/context_ownaircraft.h"
#include "blackcore/network.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/audio/notificationsounds.h"
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
using namespace BlackMisc::Audio;
using namespace BlackMisc::Input;

/*
 * Constructor
 */
SwiftGuiStd::SwiftGuiStd(BlackGui::CEnableForFramelessWindow::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, CEnableForFramelessWindow::modeToWindowFlags(windowMode)),
    CIdentifiable(this),
    CEnableForFramelessWindow(windowMode, true, "framelessMainWindow", this),
    ui(new Ui::SwiftGuiStd)
{
    // GUI
    ui->setupUi(this);
    this->setDynamicProperties(windowMode == CEnableForFramelessWindow::WindowFrameless);
}

SwiftGuiStd::~SwiftGuiStd()
{ }

void SwiftGuiStd::mouseMoveEvent(QMouseEvent *event)
{
    if (!handleMouseMoveEvent(event))
    {
        QMainWindow::mouseMoveEvent(event);
    }
}

void SwiftGuiStd::mousePressEvent(QMouseEvent *event)
{
    if (!handleMousePressEvent(event))
    {
        QMainWindow::mousePressEvent(event);
    }
}

void SwiftGuiStd::performGracefulShutdown()
{
    if (!this->m_init) { return; }
    this->m_init = false;

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

    // clean up GUI
    this->ui->comp_MainInfoArea->dockAllWidgets();
    this->ui->comp_InvisibleInfoArea->dockAllWidgets();

    // allow some other parts to react
    QApplication::processEvents(QEventLoop::AllEvents, 100);

    // tell GUI components to shut down
    emit requestGracefulShutdown();

    // tell context GUI is going down
    if (this->getIContextApplication())
    {
        this->getIContextApplication()->unregisterApplication(identifier());
    }

    // allow some other parts to react
    QApplication::processEvents(QEventLoop::AllEvents, 100);
}

void SwiftGuiStd::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    this->performGracefulShutdown();
    QApplication::exit();
}

void SwiftGuiStd::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        // make sure a tool window is changed to Normal window so it is show in taskbar
        // here we are already in transition state, so isMinimized means will be minimize right now
        // this check here is needed if minimized is called from somewhere else than ps_showMinimized
        if (!this->isFrameless())
        {
            bool toolWindow(this->isToolWindow());
            if (isMinimized())
            {
                if (toolWindow)
                {
                    // still tool, force normal window
                    BlackMisc::singleShot(0, QThread::currentThread(), [ = ]()
                    {
                        this->ps_showMinimized();
                    });
                }
            }
            else
            {
                if (!toolWindow)
                {
                    // not tool, force tool window
                    BlackMisc::singleShot(0, QThread::currentThread(), [ = ]()
                    {
                        this->ps_showNormal();
                    });
                }
            }
        } // frameless?
    }
    QMainWindow::changeEvent(event);
}

QAction *SwiftGuiStd::getWindowMinimizeAction(QObject *parent)
{
    QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarMinButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Window minimized", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_showMinimized);
    return a;
}

QAction *SwiftGuiStd::getWindowNormalAction(QObject *parent)
{
    QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarNormalButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Window normal", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_showNormal);
    return a;
}

QAction *SwiftGuiStd::getToggleWindowVisibilityAction(QObject *parent)
{
    QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarShadeButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Toogle main window visibility", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_toggleWindowVisibility);
    return a;
}

QAction *SwiftGuiStd::getToggleStayOnTopAction(QObject *parent)
{
    QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Toogle main window on top", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_toogleWindowStayOnTop);
    return a;
}

void SwiftGuiStd::ps_setMainPage(SwiftGuiStd::MainPageIndex mainPage)
{
    this->ui->sw_MainMiddle->setCurrentIndex(mainPage);
}

void SwiftGuiStd::ps_setMainPageInfoArea(CMainInfoAreaComponent::InfoArea infoArea)
{
    this->ps_setMainPageToInfoArea();
    this->ui->comp_MainInfoArea->selectArea(infoArea);
}

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

bool SwiftGuiStd::isContextNetworkAvailableCheck()
{
    if (this->m_contextNetworkAvailable) return true;
    CLogMessage(this).error("Network context not available, no updates this time");
    return false;
}

bool SwiftGuiStd::isContextAudioAvailableCheck()
{
    if (this->m_contextAudioAvailable) return true;
    CLogMessage(this).error("Audio context not available");
    return false;
}

void SwiftGuiStd::ps_displayStatusMessageInGui(const CStatusMessage &statusMessage)
{
    if (!this->m_init) { return; }
    if (statusMessage.isRedundant()) { return; }
    if (statusMessage.wasHandledBy(this)) { return; }
    statusMessage.markAsHandledBy(this);
    this->m_statusBar.displayStatusMessage(statusMessage);

    // main info areas
    this->ui->comp_MainInfoArea->displayStatusMessage(statusMessage);
    this->ui->comp_InvisibleInfoArea->displayStatusMessage(statusMessage);

    // list
    this->ui->comp_MainInfoArea->getLogComponent()->appendStatusMessageToList(statusMessage);

    // display overlay for errors, but not for validation
    if (statusMessage.getSeverity() == CStatusMessage::SeverityError && ! statusMessage.getCategories().contains(CLogCategory::validation()))
    {
        this->ui->fr_CentralFrameInside->showMessage(statusMessage);
    }
}

void SwiftGuiStd::ps_onConnectionTerminated()
{
    this->updateGuiStatusInformation();
}

void SwiftGuiStd::ps_onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
{
    Q_UNUSED(from);
    this->updateGuiStatusInformation();

    // sounds
    switch (to)
    {
    case INetwork::Connected:
        this->playNotifcationSound(CNotificationSounds::NotificationLogin);
        break;
    case INetwork::Disconnected:
        this->playNotifcationSound(CNotificationSounds::NotificationLogoff);
        break;
    case INetwork::DisconnectedError:
        this->playNotifcationSound(CNotificationSounds::NotificationError);
        break;
    default:
        break;
    }
}

void SwiftGuiStd::ps_handleTimerBasedUpdates()
{
    this->setContextAvailability();
    this->updateGuiStatusInformation();

    // own aircraft
    this->ps_reloadOwnAircraft();
}

void SwiftGuiStd::setContextAvailability()
{
    bool corePreviouslyAvailable = this->m_coreAvailable;

    if (this->getIContextApplication()->isUsingImplementingObject())
    {
        this->m_coreAvailable = true;
    }
    else
    {
        this->m_coreAvailable = isMyIdentifier(this->getIContextApplication()->registerApplication(getCurrentTimestampIdentifier()));
    }
    this->m_contextNetworkAvailable = this->m_coreAvailable || this->getIContextNetwork()->isUsingImplementingObject();
    this->m_contextAudioAvailable = this->m_coreAvailable || this->getIContextAudio()->isUsingImplementingObject();

    // react to a change in core's availability
    if (this->m_coreAvailable != corePreviouslyAvailable)
    {
        if (this->m_coreAvailable)
        {
            // core has just become available
            this->getIContextApplication()->synchronizeLogSubscriptions();
            this->getIContextApplication()->synchronizeLocalSettings();
        }
        else
        {
            // core has just become unavailable...
        }
    }
}

void SwiftGuiStd::updateGuiStatusInformation()
{
    const QString now = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    QString network("unavailable");
    if (this->m_contextNetworkAvailable)
    {
        bool dbus = !this->getIContextNetwork()->isUsingImplementingObject();
        network = dbus ? now : "local";
        this->ui->comp_InfoBarStatus->setDBusStatus(dbus);
    }

    // update status fields
    QString s = QString("network: %1").arg(network);
    this->ui->comp_InfoBarStatus->setDBusTooltip(s);
}

void SwiftGuiStd::ps_onChangedWindowOpacity(int opacity)
{
    qreal o = opacity / 100.0;
    o = o < 0.3 ? 0.3 : o;
    o = o > 1.0 ? 1.0 : o;
    QWidget::setWindowOpacity(o);
    this->ui->comp_MainInfoArea->getSettingsComponent()->setGuiOpacity(o * 100.0);
}

void SwiftGuiStd::ps_toogleWindowStayOnTop()
{
    Qt::WindowFlags flags = this->windowFlags();
    if (Qt::WindowStaysOnTopHint & flags)
    {
        flags ^= Qt::WindowStaysOnTopHint;
        flags |= Qt::WindowStaysOnBottomHint;
        CLogMessage(this).info("Window on bottom");
    }
    else
    {
        flags ^= Qt::WindowStaysOnBottomHint;
        flags |= Qt::WindowStaysOnTopHint;
        CLogMessage(this).info("Window on top");
    }
    this->setWindowFlags(flags);
    this->show();
}

void SwiftGuiStd::ps_toggleWindowVisibility()
{
    if (this->isVisible())
    {
        this->hide();
    }
    else
    {
        this->show();
    }
}

void SwiftGuiStd::ps_onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void SwiftGuiStd::ps_onCurrentMainWidgetChanged(int currentIndex)
{
    emit currentMainInfoAreaChanged(this->ui->sw_MainMiddle->currentWidget());
    Q_UNUSED(currentIndex);
}

void SwiftGuiStd::ps_onChangedMainInfoAreaFloating(bool floating)
{
    // code for whole floating area goes here
    Q_UNUSED(floating);
}

void SwiftGuiStd::ps_showMinimized()
{
    if (m_windowMode == CEnableForFramelessWindow::WindowTool) { this->toolToNormalWindow(); }
    this->showMinimized();
}

void SwiftGuiStd::ps_showNormal()
{
    if (m_windowMode == CEnableForFramelessWindow::WindowTool) { this->normalToToolWindow(); }
    this->showNormal();
}

void SwiftGuiStd::playNotifcationSound(CNotificationSounds::Notification notification) const
{
    if (!this->m_contextAudioAvailable) { return; }
    if (!this->ui->comp_MainInfoArea->getSettingsComponent()->playNotificationSounds()) { return; }
    this->getIContextAudio()->playNotification(notification, true);
}

void SwiftGuiStd::displayConsole()
{
    this->ui->comp_MainInfoArea->displayConsole();
}

void SwiftGuiStd::displayLog()
{
    this->ui->comp_MainInfoArea->displayLog();
}

