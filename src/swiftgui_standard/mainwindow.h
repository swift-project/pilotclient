/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef SAMPLE_MAINWINDOW_H
#define SAMPLE_MAINWINDOW_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "guimodeenums.h"
#include "blackcore/context_audio.h"
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_simulator.h"
#include "blackcore/context_runtime.h"
#include "blackcore/input_manager.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/infowindowcomponent.h"
#include "blackgui/transpondermodeselector.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/serverlistmodel.h"
#include "blackgui/models/aircraftlistmodel.h"
#include "blackgui/models/userlistmodel.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackgui/models/keyboardkeylistmodel.h"
#include "blackmisc/nwtextmessage.h"
#include "blackmisc/loghandler.h"
#include "blacksound/soundgenerator.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QTableView>
#include <QItemSelection>
#include <QLabel>
#include <QTimer>

namespace Ui { class MainWindow; }

//! swift GUI
class MainWindow : public QMainWindow, public BlackGui::Components::CEnableForRuntime
{
    Q_OBJECT

public:
    //! Constructor
    explicit MainWindow(GuiModes::WindowMode windowMode, QWidget *parent = nullptr);

    //! Destructor
    ~MainWindow();

    //! Init data
    void init(const BlackCore::CRuntimeConfig &runtimeConfig);

    //! Graceful shutdown
    void gracefulShutdown();

    //! Log message category
    static QString getMessageCategory() { return "swift.gui.component.mainwindow"; }

protected:
    //! Close event, e.g. when window is closed
    void closeEvent(QCloseEvent *event);

    //! Mouse moving, required for frameless window
    void mouseMoveEvent(QMouseEvent *event);

    //! Mouse press, required for frameless window
    void mousePressEvent(QMouseEvent *event);

    //! Main page indexes
    //! \remarks keep the values in sync with the real tab indexes
    enum MainPageIndex
    {
        MainPageCockpit  = 0,
        MainPageInfoArea = 1
    };

private:
    QScopedPointer<Ui::MainWindow> ui;
    BlackGui::Components::CInfoWindowComponent *m_compInfoWindow = nullptr; //!< the info window (popup
    bool m_init = false;
    GuiModes::WindowMode m_windowMode;
    BlackInput::IKeyboard *m_keyboard = nullptr; //!< hotkeys

    BlackMisc::CLogSubscriber m_logSubscriber { this, &MainWindow::ps_displayStatusMessageInGui };

    // contexts
    bool m_coreAvailable           = false;
    bool m_contextNetworkAvailable = false;
    bool m_contextAudioAvailable   = false;
    BlackMisc::Aviation::CAircraft m_ownAircraft; //!< own aircraft's state
    QTimer *m_timerContextWatchdog = nullptr;     //!< core available?
    QTimer *m_timerStatusBar       = nullptr;     //!< cleaning up status bar
    QTimer *m_timerSimulator       = nullptr;     //!< update simulator data

    // frameless window
    QPoint m_dragPosition; /*!< position, if moving is handled with frameless window */

    // context menus
    QMenu *m_contextMenuStatusMessageList = nullptr; /*!< context menu for status message list */

    // cockpit
    QString m_transponderResetValue;         //!< Temp. storage of XPdr mode to reset, req. until timer allows singleShoot with Lambdas
    QWidget *m_inputFocusedWidget = nullptr; //!< currently used widget for input, mainly used with cockpit

    // status bar
    QLabel *m_statusBarIcon  = nullptr; //!< status bar icon
    QLabel *m_statusBarLabel = nullptr; //!< status bar label

    //! GUI status update
    void updateGuiStatusInformation();

    //! 1st data reads
    void initialDataReads();

    //! Init GUI signals
    void initGuiSignals();

    //! Init dynamic menus
    void initDynamicMenus();

    //! Context network availability check, otherwise status message
    bool isContextNetworkAvailableCheck();

    //! Context voice availability check, otherwise status message
    bool isContextAudioAvailableCheck();

    //! Audio device lists
    void setAudioDeviceLists();

    //! Context availability, used by watchdog
    void setContextAvailability();

    /*!
     * \brief Position of own plane for testing
     * \param wgsLatitude   WGS latitude
     * \param wgsLongitude  WGS longitude
     * \param altitude
     */
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude);

    /*!
     * \brief Is given main page selected?
     * \param mainPage  index to be checked
     * \return
     */
    bool isMainPageSelected(MainPageIndex mainPage) const;

    //! Start all update timers
    void startUpdateTimersWhenConnected();

    //! Stop all update timers
    void stopUpdateTimersWhenDisconnected();

    /*!
     * \brief Stop all timers
     * \param disconnect also disconnect signal/slots
     */
    void stopAllTimers(bool disconnect);

    //! Play notifcation sound
    void playNotifcationSound(BlackSound::CNotificationSounds::Notification notification) const;

    //! Update simulator page with latest user aircraft data
    void updateSimulatorData();

    //! Originator for aircraft context
    static const QString &swiftGuiStandardOriginator()
    {
        // one time init, timestamp allows "multiple swift GUIs"
        static const QString o = QString("SWIFTGUISTANDARD:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
        return o;
    }

private slots:

    //
    // Data received related slots
    //

    //! Reload own aircraft
    bool ps_reloadOwnAircraft();

    //! Display status message
    void ps_displayStatusMessageInGui(const BlackMisc::CStatusMessage &);

    //! Settings have been changed
    void ps_onChangedSetttings(uint typeValue);

    /*!
     * \brief Connection status changed
     * \param from  old status, as uint so it is compliant with DBus
     * \param to    new status, as uint so it is compliant with DBus
     */
    void ps_onConnectionStatusChanged(uint from, uint to, const QString &message);

    //! Simulator available
    void ps_onSimulatorConnectionChanged(bool isAvailable);

    //
    // GUI related slots
    //

    /*!
     * \brief Set the main page
     * \param start Startup phase
     */
    void ps_setMainPage(bool start = false);

    /*!
     * \brief setMainPage
     * \param mainPage
     */
    void ps_setMainPage(MainPageIndex mainPage);

    //! Connect to network
    void ps_toggleNetworkConnection();

    //! Menu item clicked
    void ps_onMenuClicked();

    //! Terminated connection
    void ps_onConnectionTerminated();

    //! Update timer
    void ps_handleTimerBasedUpdates();

    //! Audio volume handling and mute
    void ps_setAudioVolumes();

    /*!
     * \brief changeOpacity
     * \param opacity 0-100
     */
    void ps_changeWindowOpacity(int opacity = -1);

    //! Toogle Windows stay on top
    void ps_toogleWindowStayOnTop();

    //! Set the hotkey functions
    void ps_registerHotkeyFunctions();

    //! Style sheet has been changed
    void ps_onStyleSheetsChanged();
};

#pragma pop_macro("interface")

#endif // guard
