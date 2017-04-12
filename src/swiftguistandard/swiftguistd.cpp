/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/network.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/infobarstatuscomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/icons.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/threadutils.h"
#include "ui_swiftguistd.h"

#include "swiftguistd.h"
#include <QAction>
#include <QDateTime>
#include <QIcon>
#include <QStackedWidget>
#include <QStyle>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

class QCloseEvent;
class QEvent;
class QMouseEvent;
class QWidget;

namespace BlackGui
{
    class CEnableForFramelessWindow;
    class IMainWindowAccess;
}
namespace BlackMisc { class CIdentifiable; }

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Input;
using namespace BlackMisc::Simulation;

// Constructor
SwiftGuiStd::SwiftGuiStd(BlackGui::CEnableForFramelessWindow::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, CEnableForFramelessWindow::modeToWindowFlags(windowMode)),
    CIdentifiable(this),
    CEnableForFramelessWindow(windowMode, true, "framelessMainWindow", this),
    ui(new Ui::SwiftGuiStd)
{
    // GUI
    ui->setupUi(this);
    this->setDynamicProperties(windowMode == CEnableForFramelessWindow::WindowFrameless);
    this->init();
}

SwiftGuiStd::~SwiftGuiStd()
{ }

void SwiftGuiStd::mouseMoveEvent(QMouseEvent *event)
{
    if (!handleMouseMoveEvent(event)) { QMainWindow::mouseMoveEvent(event); }
}

void SwiftGuiStd::mousePressEvent(QMouseEvent *event)
{
    if (!handleMousePressEvent(event)) { QMainWindow::mousePressEvent(event); }
}

void SwiftGuiStd::performGracefulShutdown()
{
    if (!this->m_init) { return; }
    this->m_init = false;

    Q_ASSERT_X(CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "Should shutdown in main thread");

    // shut down all timers
    this->stopAllTimers(true);

    // if we have a context, we shut some things down
    if (this->m_contextNetworkAvailable)
    {
        if (sGui && sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            if (this->m_contextAudioAvailable)
            {
                sGui->getIContextAudio()->leaveAllVoiceRooms();
                sGui->getIContextAudio()->disconnect(this); // break down signal / slots
            }
            sGui->getIContextNetwork()->disconnectFromNetwork();
            sGui->getIContextNetwork()->disconnect(this); // avoid any status update signals, etc.
        }
    }

    // clean up GUI
    ui->comp_MainInfoArea->dockAllWidgets();

    // allow some other parts to react
    if (!sGui) { return; } // overall shutdown
    sGui->processEventsToRefreshGui();

    // tell context GUI is going down
    if (sGui->getIContextApplication())
    {
        sGui->getIContextApplication()->unregisterApplication(identifier());
    }

    // allow some other parts to react
    sGui->processEventsToRefreshGui();
}

void SwiftGuiStd::closeEvent(QCloseEvent *event)
{
    if (sGui)
    {
        if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            // we do not just logoff, but give the user a chance to respond
            event->ignore();
            QTimer::singleShot(500, this, &SwiftGuiStd::ps_loginRequested);
            return;
        }

        // save settings
        if (sGui->showCloseDialog(this, event) == QDialog::Rejected) { return; }
    }
    this->performGracefulShutdown();
}

void SwiftGuiStd::changeEvent(QEvent *event)
{
    if (!CEnableForFramelessWindow::handleChangeEvent(event)) { QMainWindow::changeEvent(event); }
}

QAction *SwiftGuiStd::getWindowMinimizeAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarMinButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Window minimized", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_showMinimized);
    return a;
}

QAction *SwiftGuiStd::getWindowNormalAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarNormalButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Window normal", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_showNormal);
    return a;
}

QAction *SwiftGuiStd::getToggleWindowVisibilityAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarShadeButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Toogle main window visibility", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_toggleWindowVisibility);
    return a;
}

QAction *SwiftGuiStd::getToggleStayOnTopAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Toogle main window on top", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::ps_toogleWindowStayOnTop);
    return a;
}

void SwiftGuiStd::ps_setMainPage(SwiftGuiStd::MainPageIndex mainPage)
{
    ui->sw_MainMiddle->setCurrentIndex(mainPage);
}

void SwiftGuiStd::ps_setMainPageInfoArea(CMainInfoAreaComponent::InfoArea infoArea)
{
    this->ps_setMainPageToInfoArea();
    ui->comp_MainInfoArea->selectArea(infoArea);
}

bool SwiftGuiStd::isMainPageSelected(SwiftGuiStd::MainPageIndex mainPage) const
{
    return ui->sw_MainMiddle->currentIndex() == static_cast<int>(mainPage);
}

void SwiftGuiStd::ps_loginRequested()
{
    if (ui->sw_MainMiddle->currentIndex() == static_cast<int>(MainPageLogin))
    {
        // already main page, we fake a re-trigger here
        emit this->currentMainInfoAreaChanged(ui->sw_MainMiddle->currentWidget());
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
    // used with log subscriber
    if (statusMessage.wasHandledBy(this)) { return; }
    statusMessage.markAsHandledBy(this);
    this->m_statusBar.displayStatusMessage(statusMessage);

    // main info areas
    ui->comp_MainInfoArea->displayStatusMessage(statusMessage);

    // list
    ui->comp_MainInfoArea->getLogComponent()->appendStatusMessageToList(statusMessage);

    // display overlay for errors, but not for validation
    if (statusMessage.getSeverity() == CStatusMessage::SeverityError && ! statusMessage.getCategories().contains(CLogCategory::validation()))
    {
        ui->fr_CentralFrameInside->showOverlayMessage(statusMessage);
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
    const bool corePreviouslyAvailable = this->m_coreAvailable;
    if (sGui &&
            sGui->getIContextApplication() &&
            !sGui->isShuttingDown() &&
            !sGui->getIContextApplication()->isEmptyObject())
    {
        // ping to check if core is still alive
        this->m_coreAvailable = this->isMyIdentifier(sGui->getIContextApplication()->registerApplication(getCurrentTimestampIdentifier()));
    }
    else
    {
        this->m_coreAvailable = false;
    }

    this->m_contextNetworkAvailable = this->m_coreAvailable && sGui->getIContextNetwork() && !sGui->getIContextNetwork()->isEmptyObject();
    this->m_contextAudioAvailable = this->m_coreAvailable && sGui->getIContextAudio() && !sGui->getIContextAudio()->isEmptyObject();

    // react to a change in core's availability
    if (this->m_coreAvailable != corePreviouslyAvailable)
    {
        if (this->m_coreAvailable)
        {
            // core has just become available (startup)
            sGui->getIContextApplication()->synchronizeLogSubscriptions();
            sGui->getIContextApplication()->synchronizeLocalSettings();
        }
    }
}

void SwiftGuiStd::updateGuiStatusInformation()
{
    const QString now = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    QString network("unavailable");
    if (this->m_contextNetworkAvailable)
    {
        bool dbus = !sGui->getIContextNetwork()->isUsingImplementingObject();
        network = dbus ? now : "local";
        ui->comp_InfoBarStatus->setDBusStatus(dbus);
    }

    // update status fields
    const QString s = QString("network: %1").arg(network);
    ui->comp_InfoBarStatus->setDBusTooltip(s);
}

void SwiftGuiStd::ps_onChangedWindowOpacity(int opacity)
{
    qreal o = opacity / 100.0;
    o = o < 0.3 ? 0.3 : o;
    o = o > 1.0 ? 1.0 : o;
    QWidget::setWindowOpacity(o);
    ui->comp_MainInfoArea->getSettingsComponent()->setGuiOpacity(o * 100.0);
}

void SwiftGuiStd::ps_toogleWindowStayOnTop()
{
    CGuiUtility::toggleStayOnTop(this);
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
    emit currentMainInfoAreaChanged(ui->sw_MainMiddle->currentWidget());
    Q_UNUSED(currentIndex);
}

void SwiftGuiStd::ps_onChangedMainInfoAreaFloating(bool floating)
{
    // code for whole floating area goes here
    Q_UNUSED(floating);
}

void SwiftGuiStd::ps_showMinimized()
{
    this->showMinimizedModeChecked();
}

void SwiftGuiStd::ps_showNormal()
{
    this->showNormalModeChecked();
}

void SwiftGuiStd::ps_navigatorClosed()
{
    this->showNormal();
}

void SwiftGuiStd::ps_verifyDataAvailability()
{
    const CSimulatorInfo sims = sGui->getIContextSimulator()->simulatorsWithInitializedModelSet();
    if (sims.isNoSimulator())
    {
        CLogMessage(this).error("No model set so far, you need at least one model set");
    }
}

void SwiftGuiStd::ps_sharedFilesHeadersLoaded()
{
    Q_ASSERT_X(sGui && sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");
    Q_ASSERT_X(CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "Wrong thread");
    const CEntityFlags::Entity newEntities = sGui->getWebDataServices()->getEntitiesWithNewerHeaderTimestamp(CEntityFlags::AllDbEntities);
    if (newEntities == CEntityFlags::NoEntity) { return; }
    CStatusMessage sm = CStatusMessage(this).info("New data for shared files:");
    CStatusMessageList sms({sm});
    const QSet<CEntityFlags::Entity> newSingleEntities = CEntityFlags::asSingleEntities(newEntities);
    for (CEntityFlags::Entity newSingleEntity : newSingleEntities)
    {
        sm = CStatusMessage(this).info("Load data for '%1'?") << CEntityFlags::flagToString(newSingleEntity);
        sms.push_back(sm);
    }

    // allow to init GUI completely
    const int delay = 2500;
    QTimer::singleShot(delay, this, [ = ]
    {
        // delayed call
        auto lambda = [newEntities]()
        {
            sGui->getWebDataServices()->triggerLoadingDirectlyFromSharedFiles(newEntities, false);
        };
        ui->fr_CentralFrameInside->showOverlayMessagesWithConfirmation(sms, "Load data?", lambda);
    });
}

void SwiftGuiStd::playNotifcationSound(CNotificationSounds::Notification notification) const
{
    if (!this->m_contextAudioAvailable) { return; }
    if (!ui->comp_MainInfoArea->getSettingsComponent()->playNotificationSounds()) { return; }
    sGui->getIContextAudio()->playNotification(notification, true);
}

void SwiftGuiStd::displayConsole()
{
    ui->comp_MainInfoArea->displayConsole();
}

void SwiftGuiStd::displayLog()
{
    ui->comp_MainInfoArea->displayLog();
}
