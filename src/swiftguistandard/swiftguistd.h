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

#include "guimodeenums.h"
#include "blackcore/contextallinterfaces.h"
#include "blackcore/actionbind.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/navigatordialog.h"
#include "blackgui/transpondermodeselector.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/serverlistmodel.h"
#include "blackgui/models/userlistmodel.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/identifiable.h"
#include "blacksound/soundgenerator.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QTableView>
#include <QItemSelection>
#include <QLabel>
#include <QTimer>

namespace Ui { class SwiftGuiStd; }

//! swift GUI
class SwiftGuiStd :
    public QMainWindow,
    public BlackMisc::CIdentifiable,
    public BlackGui::CEnableForFramelessWindow,
    public BlackGui::Components::CEnableForRuntime
{
    Q_OBJECT

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
    SwiftGuiStd(BlackGui::CEnableForFramelessWindow::WindowMode windowMode, QWidget *parent = nullptr);

    //! Destructor
    ~SwiftGuiStd();

    //! Init data
    void init(const BlackCore::CCoreFacadeConfig &runtimeConfig);

    //! Log message category
    static QString getMessageCategory() { return "swift.gui.stdgui"; }

signals:
    //! GUI is shutting down, request graceful shutdown
    void requestGracefulShutdown();

    //! Main info area changed
    //! \remarks using widget pointer allows the component itself to identify if it is current
    void currentMainInfoAreaChanged(const QWidget *currentWidget);

protected:
    //! \copydoc QMainWindow::mouseMoveEvent
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    //! \copydoc QMainWindow::mousePressEvent
    virtual void mousePressEvent(QMouseEvent *event) override;

    //! \copydoc QMainWindow::closeEvent
    virtual void closeEvent(QCloseEvent *event) override;

    //! \copydoc QMainWindow::changeEvent
    virtual void changeEvent(QEvent *event) override;

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
    // if I pass the parent, the dialog is always center over the parent
    QScopedPointer<BlackGui::Components::CNavigatorDialog> m_navigator{new BlackGui::Components::CNavigatorDialog()};
    bool                                           m_init = false;
    BlackGui::CManagedStatusBar                    m_statusBar;
    BlackMisc::CLogSubscriber                      m_logSubscriber { this, &SwiftGuiStd::ps_displayStatusMessageInGui };
    BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this};  //!< setup cache

    // contexts
    bool m_coreAvailable           = false;
    bool m_contextNetworkAvailable = false;
    bool m_contextAudioAvailable   = false;
    QTimer *m_timerContextWatchdog = nullptr;                //!< core available?
    BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft; //!< own aircraft's state
    QSize m_windowMinSizeWithMainPageShown;
    QSize m_windowMinSizeWithMainPageHidden;

    // cockpit
    QString m_transponderResetValue;         //!< Temp. storage of XPdr mode to reset, req. until timer allows singleShoot with Lambdas
    QWidget *m_inputFocusedWidget = nullptr; //!< currently used widget for input, mainly used with cockpit

    // actions
    BlackCore::CActionBind m_action50 { "/swiftGui/Change opacity to 50%", this, &SwiftGuiStd::ps_onChangedWindowOpacityTo50 };
    BlackCore::CActionBind m_action100 { "/swiftGui/Change opacity to 100%", this, &SwiftGuiStd::ps_onChangedWindowOpacityTo100 };

    //! GUI status update
    void updateGuiStatusInformation();

    //! Set style sheet
    void initStyleSheet();

    //! 1st data reads
    void initialDataReads();

    //! Init GUI signals
    void initGuiSignals();

    //! Init dynamic menus
    void initDynamicMenus();

    //! Menu icons where required
    void initMenuIcons();

    //! Graceful shutdown
    void performGracefulShutdown();

    //! Context network availability check, otherwise status message
    bool isContextNetworkAvailableCheck();

    //! Context voice availability check, otherwise status message
    bool isContextAudioAvailableCheck();

    //! Audio device lists
    void setAudioDeviceLists();

    //! Context availability, used by watchdog
    void setContextAvailability();

    //! Position of own plane for testing
    //! \param wgsLatitude   WGS latitude
    //! \param wgsLongitude  WGS longitude
    //! \param altitude
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude);

    //! Is given main page selected?
    //! \param mainPage index to be checked
    bool isMainPageSelected(MainPageIndex mainPage) const;

    //! Start all update timers
    void startUpdateTimersWhenConnected();

    //! Stop all update timers
    void stopUpdateTimersWhenDisconnected();

    //! Stop all timers
    //! \param disconnect also disconnect signal/slots
    void stopAllTimers(bool disconnectSignalSlots);

    //! Play notifcation sound
    void playNotifcationSound(BlackMisc::Audio::CNotificationSounds::Notification notification) const;

    //! Display console
    void displayConsole();

    //! Display log
    void displayLog();

private slots:
    //
    // Data received related slots
    //

    //! Reload own aircraft
    bool ps_reloadOwnAircraft();

    //! Display status message
    void ps_displayStatusMessageInGui(const BlackMisc::CStatusMessage &);

    //! Connection status changed
    //! \param from  old status, as int so it is compliant with DBus
    //! \param to    new status, as int so it is compliant with DBus
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
};

#pragma pop_macro("interface")

#endif // guard
