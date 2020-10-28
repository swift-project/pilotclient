/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_swiftguistd.h"

#include "blackgui/components/infobarstatuscomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/dbloaddatadialog.h"
#include "blackgui/components/autopublishdialog.h"
#include "blackgui/components/afvmapdialog.h"
#include "blackgui/components/logindialog.h"
#include "blackgui/components/modelbrowserdialog.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/webdataservices.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/icons.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/threadutils.h"
#include "blackconfig/buildconfig.h"

#include "swiftguistd.h"
#include <QAction>
#include <QDateTime>
#include <QIcon>
#include <QSize>
#include <QStackedWidget>
#include <QStyle>
#include <QWidget>
#include <Qt>
#include <QtGlobal>
#include <QMessageBox>
#include <QPointer>

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
using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Input;
using namespace BlackMisc::Simulation;
using namespace BlackConfig;

// Constructor
SwiftGuiStd::SwiftGuiStd(BlackGui::CEnableForFramelessWindow::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, CEnableForFramelessWindow::modeToWindowFlags(windowMode)),
    CIdentifiable(this),
    CEnableForFramelessWindow(windowMode, true, "framelessMainWindow", this),
    ui(new Ui::SwiftGuiStd)
{
    // GUI
    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    sGui->registerMainApplicationWidget(this);
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

void SwiftGuiStd::mouseReleaseEvent(QMouseEvent *event)
{
    m_framelessDragPosition = QPoint();
    QMainWindow::mouseReleaseEvent(event);
}

void SwiftGuiStd::performGracefulShutdown()
{
    if (!m_init) { return; }
    m_init = false;

    Q_ASSERT_X(CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Should shutdown in main thread");

    // shut down all timers
    this->stopAllTimers(true);

    // if we have a context, we shut some things down
    if (m_contextNetworkAvailable)
    {
        if (sGui && sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            if (m_contextAudioAvailable)
            {
                sGui->getIContextAudio()->disconnect(this); // break down signal / slots
            }
            sGui->getIContextNetwork()->disconnectFromNetwork();
            sGui->getIContextNetwork()->disconnect(this); // avoid any status update signals, etc.
        }
    }

    // clean up GUI
    ui->comp_MainInfoArea->dockAllWidgets();

    // allow some other parts to react
    const QPointer<SwiftGuiStd> myself(this);
    if (sGui) { sGui->processEventsToRefreshGui(); }
    if (!sGui || !myself) { return; } // killed in meantime?

    // tell context GUI is going down
    if (sGui->getIContextApplication())
    {
        sGui->getIContextApplication()->unregisterApplication(identifier());
    }

    // allow some other parts to react
    if (sGui) { sGui->processEventsToRefreshGui(); }
}

void SwiftGuiStd::closeEvent(QCloseEvent *event)
{
    if (sGui)
    {
        if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            // we do not just logoff, but give the user a chance to respond
            event->ignore();
            QPointer<SwiftGuiStd> myself(this);
            QTimer::singleShot(500, this, [ = ]
            {
                if (!myself) { return; }
                myself->loginRequested();
            });
            return;
        }

        if (this->triggerAutoPublishDialog())
        {
            event->ignore();
            return;
        }

        // save settings
        if (sGui->showCloseDialog(this, event) == QDialog::Rejected)
        {
            // already ignored
            return;
        }
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
    connect(a, &QAction::triggered, this, &SwiftGuiStd::showMinimized);
    return a;
}

QAction *SwiftGuiStd::getWindowNormalAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarNormalButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Window normal", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::showNormal);
    return a;
}

QAction *SwiftGuiStd::getToggleWindowVisibilityAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarShadeButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Toogle main window visibility", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::toggleWindowVisibility);
    return a;
}

QAction *SwiftGuiStd::getToggleStayOnTopAction(QObject *parent)
{
    const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton), Qt::white, QSize(16, 16)));
    QAction *a = new QAction(i, "Toogle main window on top", parent);
    connect(a, &QAction::triggered, this, &SwiftGuiStd::toggleWindowStayOnTop);
    return a;
}

void SwiftGuiStd::setMainPage(SwiftGuiStd::MainPageIndex mainPage)
{
    ui->sw_MainMiddle->setCurrentIndex(mainPage);
}

void SwiftGuiStd::setMainPageInfoArea(CMainInfoAreaComponent::InfoArea infoArea)
{
    this->setMainPageToInfoArea();
    ui->comp_MainInfoArea->selectArea(infoArea);
}

void SwiftGuiStd::setSettingsPage(int settingsTabIndex)
{
    this->setMainPageInfoArea(CMainInfoAreaComponent::InfoAreaSettings);
    if (settingsTabIndex < 0) { return; }
    ui->comp_MainInfoArea->getSettingsComponent()->setCurrentIndex(settingsTabIndex);
}

bool SwiftGuiStd::isMainPageSelected(SwiftGuiStd::MainPageIndex mainPage) const
{
    return ui->sw_MainMiddle->currentIndex() == static_cast<int>(mainPage);
}

void SwiftGuiStd::loginRequested()
{
    if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }
    if (ui->sw_MainMiddle->currentIndex() == static_cast<int>(MainPageLogin))
    {
        // already main page, we fake a re-trigger here
        // emit this->currentMainInfoAreaChanged(ui->sw_MainMiddle->currentWidget());
        ui->comp_Login->toggleNetworkConnection();
    }
    else
    {
        // const bool connected = sGui->getIContextNetwork()->isConnected();
        const bool shift = QApplication::keyboardModifiers() & Qt::ShiftModifier;
        if (shift)
        {
            if (!m_loginDialog) { m_loginDialog.reset(new CLoginDialog(this)); }
            connect(m_loginDialog.data(), &CLoginDialog::requestNetworkSettings, this, &SwiftGuiStd::displayNetworkSettings);
            if (!CBuildConfig::isLocalDeveloperDebugBuild())
            {
                m_loginDialog->setAutoLogoff(true);
            }
            m_loginDialog->show();
        }
        else
        {
            this->setMainPage(MainPageLogin);
        }
    }
}

void SwiftGuiStd::onKickedFromNetwork(const QString &kickMessage)
{
    this->updateGuiStatusInformation();

    const QString msgText = kickMessage.isEmpty() ?
                            QStringLiteral("You have been kicked from the network") :
                            QStringLiteral("You have been kicked: '%1'").arg(kickMessage);
    CLogMessage(this).error(msgText);
    // this->displayInOverlayWindow(CStatusMessage(this, CStatusMessage::SeverityError, msgText));
}

void SwiftGuiStd::onConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
{
    Q_UNUSED(from)
    this->updateGuiStatusInformation();

    // sounds
    switch (to.getConnectionStatus())
    {
    case CConnectionStatus::Connected:    this->playNotifcationSound(CNotificationSounds::NotificationLogin); break;
    case CConnectionStatus::Disconnected: this->playNotifcationSound(CNotificationSounds::NotificationLogoff); break;
    default: break;
    }
}

void SwiftGuiStd::handleTimerBasedUpdates()
{
    this->setContextAvailability();
    this->updateGuiStatusInformation();

    // own aircraft
    this->reloadOwnAircraft();
}

void SwiftGuiStd::setContextAvailability()
{
    const bool corePreviouslyAvailable = m_coreAvailable;
    const bool isShuttingDown = !sGui || sGui->isShuttingDown();
    if (!isShuttingDown && sGui->getIContextApplication() && !sGui->getIContextApplication()->isEmptyObject())
    {
        // ping to check if core is still alive
        m_coreAvailable = this->isMyIdentifier(sGui->getIContextApplication()->registerApplication(identifier()));
    }
    else
    {
        m_coreAvailable = false;
    }
    if (isShuttingDown) { return; }
    if (m_coreAvailable && m_coreFailures > 0)
    {
        m_coreFailures--;
    }
    else if (!m_coreAvailable && m_coreFailures < MaxCoreFailures)
    {
        m_coreFailures++;
    }
    else if (!m_coreAvailable && !m_displayingDBusReconnect)
    {
        this->displayDBusReconnectDialog();
    }
    m_contextNetworkAvailable = m_coreAvailable && sGui->getIContextNetwork() && !sGui->getIContextNetwork()->isEmptyObject();
    m_contextAudioAvailable   = m_coreAvailable && sGui->getIContextAudio()   && !sGui->getIContextAudio()->isEmptyObject();

    // react to a change in core's availability
    if (m_coreAvailable != corePreviouslyAvailable)
    {
        if (m_coreAvailable)
        {
            // core has just become available (startup)
            // this HERE is called with and without DBus
            sGui->getIContextApplication()->synchronizeLocalSettings();
        }
    }
}

void SwiftGuiStd::updateGuiStatusInformation()
{
    if (m_coreAvailable)
    {
        static const QString dBusTimestamp("%1 %2");
        static const QString local("local");
        const QString now = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
        const bool dBus = sGui->getCoreFacadeConfig().requiresDBusConnection();
        ui->comp_InfoBarStatus->setDBusStatus(dBus && m_coreAvailable);
        ui->comp_InfoBarStatus->setDBusTooltip(dBus ? dBusTimestamp.arg(now, sGui->getCoreFacadeConfig().getDBusAddress()) : local);
    }
    else
    {
        static const QString unavailable("unavailable");
        ui->comp_InfoBarStatus->setDBusStatus(false);
        ui->comp_InfoBarStatus->setDBusTooltip(unavailable);
    }
}

void SwiftGuiStd::onChangedWindowOpacity(int opacity)
{
    qreal o = opacity / 100.0;
    o = o < 0.3 ? 0.3 : o;
    o = o > 1.0 ? 1.0 : o;
    QWidget::setWindowOpacity(o);
    ui->comp_MainInfoArea->getSettingsComponent()->setGuiOpacity(o * 100.0);
}

void SwiftGuiStd::toggleWindowStayOnTop()
{
    if (sGui) { sGui->toggleStayOnTop(); }
}

void SwiftGuiStd::toggleWindowVisibility()
{
    if (this->isVisible()) { this->hide(); return; }
    this->show();
}

void SwiftGuiStd::onStyleSheetsChanged()
{
    this->initStyleSheet();
}

void SwiftGuiStd::onToggledWindowsOnTop(bool onTop)
{
    if (onTop)
    {
        // here we could automatically display the navigator
        // if (m_navigator) { m_navigator->showNavigator(true); }
        ui->comp_MainInfoArea->allFloatingOnTop();
    }
}

void SwiftGuiStd::onCurrentMainWidgetChanged(int currentIndex)
{
    emit this->currentMainInfoAreaChanged(ui->sw_MainMiddle->currentWidget());
    Q_UNUSED(currentIndex)
}

void SwiftGuiStd::onChangedMainInfoAreaFloating(bool floating)
{
    // code for whole floating area goes here
    Q_UNUSED(floating)
}

void SwiftGuiStd::onRequestedConsoleMessage(const QString &logMsg, bool clear)
{
    if (logMsg.isEmpty()) { return; }
    CLogComponent *log = ui->comp_MainInfoArea->getLogComponent();
    Q_ASSERT_X(log, Q_FUNC_INFO, "Missing log component");
    if (clear) { log->clearConsole(); }
    log->appendPlainTextToConsole(logMsg);
}

void SwiftGuiStd::onAudioClientFailure(const CStatusMessage &msg)
{
    if (msg.isEmpty()) { return; }
    if (!sGui || sGui->isShuttingDown()) { return; }

    ui->fr_CentralFrameInside->showOverlayHTMLMessage(msg);
}

void SwiftGuiStd::focusInMainEntryField()
{
    ui->comp_MainKeypadArea->focusInEntryField();
}

void SwiftGuiStd::focusInTextMessageEntryField()
{
    if (!ui->comp_MainInfoArea->getTextMessageComponent()) { return; }
    if (ui->comp_MainInfoArea->getTextMessageComponent()->isParentDockWidgetFloating())
    {
        ui->comp_MainInfoArea->getTextMessageComponent()->activateWindow();
        ui->comp_MainInfoArea->getTextMessageComponent()->focusTextEntry();
    }
    else
    {
        this->focusInMainEntryField();
    }
}

void SwiftGuiStd::showMinimized()
{
    this->showMinimizedModeChecked();
}

void SwiftGuiStd::showNormal()
{
    this->showNormalModeChecked();
}

void SwiftGuiStd::onNavigatorClosed()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    this->show();
}

void SwiftGuiStd::verifyPrerequisites()
{
    if (!sGui || sGui->isShuttingDown()) { return; }

    CStatusMessageList msgs;
    if (!sGui->supportsContexts() || !sGui->getIContextSimulator())
    {
        msgs.push_back(CStatusMessage(this).error(u"No simulator context"));
    }
    else
    {
        msgs.push_back(sGui->getIContextSimulator()->verifyPrerequisites());
    }

    if (msgs.hasWarningOrErrorMessages())
    {
        if (msgs.size() > 1) { this->displayInOverlayWindow(msgs); }
        else { this->displayInOverlayWindow(msgs.front()); }
    }

    this->copyXSwiftBusDialog(true);
}

void SwiftGuiStd::onValidatedModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &valid, const CAircraftModelList &invalid, bool stopped, const CStatusMessageList &msgs)
{
    // will NOT be called if no errors and setting is "only on errors"
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (QApplication::activeModalWidget())
    {
        // avoid too many "deadlocking" dialogs, display warning instead
        if (invalid.isEmpty()) { return; }
        const CStatusMessage m = CLogMessage(this).validationWarning(u"Model set validation has found %1 invalid models for '%2', check the model validation") << invalid.size() << simulator.toQString(true);
        this->displayInOverlayWindow(m, 5000);
        return;
    }

    this->displayValidationDialog();
    m_validationDialog->validatedModelSet(simulator, valid, invalid, stopped, msgs);
}

void SwiftGuiStd::displayValidationDialog()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (!m_validationDialog)
    {
        m_validationDialog.reset(new CAircraftModelSetValidationDialog(this));
    }
    m_validationDialog->show();
}

void SwiftGuiStd::checkDbDataLoaded()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");
    Q_ASSERT_X(CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Wrong thread, needs to run in main thread");
    const CEntityFlags::Entity loadEntities = sGui->getWebDataServices()->getSynchronizedEntitiesWithNewerSharedFileOrEmpty(!m_dbDataLoading);
    if (loadEntities == CEntityFlags::NoEntity)
    {
        m_dbDataLoading = false;
        return;
    }

    if (!m_dbLoadDialog) { m_dbLoadDialog.reset(new CDbLoadDataDialog(this)); }
    m_dbLoadDialog->newerOrEmptyEntitiesDetected(loadEntities);
}

void SwiftGuiStd::playNotifcationSound(CNotificationSounds::NotificationFlag notification) const
{
    if (!m_contextAudioAvailable) { return; }
    if (!ui->comp_MainInfoArea->getSettingsComponent()->playNotificationSounds()) { return; }
    if (!sGui || sGui->isShuttingDown()) { return; }
    sGui->getCContextAudioBase()->playNotification(notification, true);
}

void SwiftGuiStd::displayConsole()
{
    ui->comp_MainInfoArea->displayConsole();
}

void SwiftGuiStd::displayLog()
{
    ui->comp_MainInfoArea->displayLog();
}

void SwiftGuiStd::displayNetworkSettings()
{
    if (!sApp || sApp->isShuttingDown()) { return; }
    this->setMainPageInfoArea(CMainInfoAreaComponent::InfoAreaSettings);
    ui->comp_MainInfoArea->getSettingsComponent()->setTab(CSettingsComponent::SettingTabServers);
}

void SwiftGuiStd::onPttChanged(bool enabled)
{
    Q_UNUSED(enabled)
    if (!sGui || !sGui->getCContextAudioBase()) { return; }

    // based on user request still play with AFV
    sGui->getCContextAudioBase()->playNotification(
        enabled ? CNotificationSounds::PTTClickKeyDown : CNotificationSounds::PTTClickKeyUp,
        true);
}

void SwiftGuiStd::displayDBusReconnectDialog()
{
    if (m_displayingDBusReconnect) { return; }
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (!sGui->getCoreFacade()) { return; }
    if (!sGui->getCoreFacadeConfig().requiresDBusConnection()) { return; }
    m_displayingDBusReconnect = true;
    const QString dBusAddress = sGui->getCoreFacade()->getDBusAddress();
    static const QString informativeText("Do you want to try to reconnect? 'Abort' will close the GUI.\n\nDBus: '%1'");
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("swift core not reachable!");
    msgBox.setInformativeText(informativeText.arg(dBusAddress));
    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Abort);
    msgBox.setDefaultButton(QMessageBox::Retry);
    const int ret = msgBox.exec();
    if (ret == QMessageBox::Abort)
    {
        m_coreFailures = 0;
        sGui->gracefulShutdown();
        CGuiApplication::exit(EXIT_FAILURE);
        return;
    }

    m_displayingDBusReconnect = false;
    CStatusMessage msg = sGui->getCoreFacade()->tryToReconnectWithDBus();
    if (msg.isSuccess()) { m_coreFailures = 0; }
    msg.clampSeverity(CStatusMessage::SeverityWarning);
    CLogMessage::preformatted(msg);
}

void SwiftGuiStd::onShowOverlayVariant(const CVariant &variant, int durationMs)
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    ui->fr_CentralFrameInside->showOverlayVariant(variant, durationMs);
}

void SwiftGuiStd::onShowOverlayInlineTextMessageTab(Components::TextMessageTab tab)
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    ui->fr_CentralFrameInside->showOverlayInlineTextMessage(tab);
}

void SwiftGuiStd::onShowOverlayInlineTextMessageCallsign(const CCallsign &callsign)
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    ui->fr_CentralFrameInside->showOverlayInlineTextMessage(callsign);
}

bool SwiftGuiStd::triggerAutoPublishDialog()
{
    if (!CAutoPublishData::existAutoPublishFiles()) { return false; }

    constexpr qint64 deltaT = 48 * 60 * 60 * 1000;
    const qint64 lastDialogTs = m_lastAutoPublish.get();
    bool showAutoPublish = lastDialogTs < 0 || (QDateTime::currentMSecsSinceEpoch() - lastDialogTs) > deltaT;
    if (!showAutoPublish) { return false; }

    const QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                QStringLiteral("Upload data?"),
                QStringLiteral("Do you want to help improving swift by uploading anonymized data?"),
                QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
    {
        m_lastAutoPublish.set(QDateTime::currentMSecsSinceEpoch());
        return false;
    }

    this->autoPublishDialog(); // updates m_lastAutoPublish
    return true;
}

bool SwiftGuiStd::startModelBrowser()
{
    if (!m_modelBrower)
    {
        m_modelBrower.reset(new CModelBrowserDialog(this));
    }
    m_modelBrower->exec();
    return true;
}

bool SwiftGuiStd::startAFVMap()
{
    /**
    if (!m_mapDialog)
    {
        m_mapDialog.reset(new CAfvMapDialog(this));
        m_mapDialog->setWindowModality(Qt::NonModal);
    }
    m_mapDialog->exec();
    **/

    //! \todo KB 2019-11 AFV map workaround
    if (sGui && !sGui->isShuttingDown())
    {
        sGui->openUrl("https://afv-map.vatsim.net/");
    }


    return true;
}
