/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef STDGUI_SWIFTGUISTD_H
#define STDGUI_SWIFTGUISTD_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "blackcore/actionbind.h"
#include "blackcore/network.h"
#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/navigatordialog.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/guiactionbind.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"

#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>
#include <QSize>
#include <QString>

class QAction;
class QCloseEvent;
class QEvent;
class QMouseEvent;
class QTimer;
class QWidget;

namespace BlackMisc { namespace Aviation { class CAltitude; } }
namespace Ui { class SwiftGuiStd; }

//! swift GUI
class SwiftGuiStd :
    public QMainWindow,
    public BlackMisc::CIdentifiable,
    public BlackGui::CEnableForFramelessWindow,
    public BlackGui::IMainWindowAccess
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

public:
    //! Main page indexes
    //! \remarks keep the values in sync with the real tab indexes
    enum MainPageIndex
    {
        MainPageInfoArea  = 0,
        MainPageLogin     = 1,
        MainPageInternals = 2,
        MainPageInvisible = 3
    };

    //! Constructor
    SwiftGuiStd(WindowMode windowMode, QWidget *parent = nullptr);

    //! Destructor
    virtual ~SwiftGuiStd();

    //! Log message category
    static QString getLogCategory() { return "swift.gui.stdgui"; }

signals:
    //! Main info area has changed
    //! \remarks using widget pointer allows the component itself to identify if it is current
    void currentMainInfoAreaChanged(const QWidget *currentWidget);

protected:
    //! \name QMainWindow events
    //! @{
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void changeEvent(QEvent *event) override;
    //! @}

    //! Get a minimize action which minimizes the window
    QAction *getWindowMinimizeAction(QObject *parent);

    //! Get a normal window action which minimizes the window
    QAction *getWindowNormalAction(QObject *parent);

    //! Toggle window visibility action
    QAction *getToggleWindowVisibilityAction(QObject *parent);

    //! Toggle window stay on top action
    QAction *getToggleStayOnTopAction(QObject *parent);

private:
    QScopedPointer<Ui::SwiftGuiStd> ui;
    QScopedPointer<BlackGui::Components::CNavigatorDialog> m_navigator{new BlackGui::Components::CNavigatorDialog()}; // if I pass the parent, the dialog is always centered over the parent
    BlackCore::CActionBindings  m_menuHotkeyHandlers;
    BlackGui::CManagedStatusBar m_statusBar;
    BlackMisc::CLogSubscriber   m_logSubscriber { this, &SwiftGuiStd::ps_displayStatusMessageInGui };
    bool                        m_init = false;

    // contexts
    static constexpr int MaxCoreFailures  = 5; //!< Failures counted before reconnecting
    int  m_coreFailures            = 0;        //!< failed access to core
    bool m_coreAvailable           = false;    //!< core already available?
    bool m_contextNetworkAvailable = false;    //!< network context available?
    bool m_contextAudioAvailable   = false;    //!< audio context available?
    bool m_displayingDBusReconnect = false;    //!< currently displaying reconnect dialog
    QTimer *m_timerContextWatchdog = nullptr;  //!< core available?
    BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft; //!< own aircraft's state

    //! GUI status update
    void updateGuiStatusInformation();

    //! Set style sheet
    void initStyleSheet();

    //! 1st data reads
    void initialDataReads();

    //! Init data (post GUI init)
    void init();

    //! Init GUI signals
    void initGuiSignals();

    //! Init dynamic menus
    void initMenus();

    //! Graceful shutdown
    void performGracefulShutdown();

    //! Audio device lists
    void setAudioDeviceLists();

    //! Context and DBus availability, used by watchdog
    void setContextAvailability();

    //! Position of own plane for testing
    //! \param wgsLatitude  WGS latitude
    //! \param wgsLongitude WGS longitude
    //! \param altitude
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude);

    //! Is given main page selected?
    //! \param mainPage index to be checked
    bool isMainPageSelected(MainPageIndex mainPage) const;

    //! Stop all timers
    //! \param disconnect also disconnect signal/slots
    void stopAllTimers(bool disconnectSignalSlots);

    //! Play notifcation sound
    void playNotifcationSound(BlackMisc::Audio::CNotificationSounds::Notification notification) const;

    //! Display console
    void displayConsole();

    //! Display log
    void displayLog();

    //! Display a reconnect dialog
    void displayDBusReconnectDialog();

private slots:
    //
    // Data received related slots
    //

    //! Reload own aircraft
    bool ps_reloadOwnAircraft();

    //! Display status message
    void ps_displayStatusMessageInGui(const BlackMisc::CStatusMessage &statusMessage);

    //! Connection status changed
    //! \param from old status
    //! \param to   new status
    void ps_onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

    //
    // GUI related slots
    //

    //! Set \sa MainPageInfoArea
    void ps_setMainPageToInfoArea() { this->ps_setMainPage(MainPageInfoArea); }

    //! Set one of the main pages
    void ps_setMainPage(MainPageIndex mainPage);

    //! Set the main info area
    void ps_setMainPageInfoArea(BlackGui::Components::CMainInfoAreaComponent::InfoArea infoArea);

    //! Login requested
    void ps_loginRequested();

    //! Menu item clicked
    void ps_onMenuClicked();

    //! Terminated connection
    void ps_onConnectionTerminated();

    //! Update timer
    void ps_handleTimerBasedUpdates();

    //! Change opacity 0-100
    void ps_onChangedWindowOpacityTo50(bool) { ps_onChangedWindowOpacity(50); }

    //! Change opacity 0-100
    void ps_onChangedWindowOpacityTo100(bool) { ps_onChangedWindowOpacity(100); }

    //! Change opacity 0-100
    void ps_onChangedWindowOpacity(int opacity = -1);

    //! Toogle if windows stays on top
    void ps_toogleWindowStayOnTop();

    //! Toggle window visibility
    void ps_toggleWindowVisibility();

    //! Style sheet has been changed
    void ps_onStyleSheetsChanged();

    //! Main info area current widget changed
    void ps_onCurrentMainWidgetChanged(int currentIndex);

    //! Whole main info area floating
    void ps_onChangedMainInfoAreaFloating(bool floating);

    //! Show window minimized
    void ps_showMinimized();

    //! Show window normal
    void ps_showNormal();

    //! Navigator dialog has been closed
    void ps_navigatorClosed();

    //! Checks if data such as model set etc. are available
    void ps_verifyDataAvailability();

    //! The shared headers have been loaded
    void ps_sharedInfoObjectsLoaded();
};

#pragma pop_macro("interface")

#endif // guard
