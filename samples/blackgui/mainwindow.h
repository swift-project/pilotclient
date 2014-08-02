/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef SAMPLE_MAINWINDOW_H
#define SAMPLE_MAINWINDOW_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "guimodeenums.h"
#include "blackinput/keyboard.h"
#include "blackcore/context_audio.h"
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_simulator.h"
#include "blackcore/context_runtime.h"
#include "blackgui/components/runtimebasedcomponent.h"
#include "blackgui/components/infowindowcomponent.h"
#include "blackgui/transpondermodeselector.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/serverlistmodel.h"
#include "blackgui/models/aircraftlistmodel.h"
#include "blackgui/models/userlistmodel.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackgui/models/keyboardkeylistmodel.h"
#include "blackmisc/nwtextmessage.h"
#include "blacksound/soundgenerator.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QTableView>
#include <QItemSelection>
#include <QLabel>
#include <QTimer>

namespace Ui { class MainWindow; }

//! swift GUI
class MainWindow : public QMainWindow, public BlackGui::Components::CRuntimeBasedComponent
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
        MainPageStatus = 0,
        MainPageCockpit = 4,
        MainPageFoo = 9
    };

private:
    QScopedPointer<Ui::MainWindow> ui;
    BlackGui::Components::CInfoWindowComponent *m_compInfoWindow;
    bool m_init;
    GuiModes::WindowMode m_windowMode;
    BlackInput::IKeyboard *m_keyboard; //!< hotkeys

    // contexts
    bool m_coreAvailable;
    bool m_contextNetworkAvailable;
    bool m_contextAudioAvailable;
    BlackMisc::Aviation::CAircraft m_ownAircraft; /*!< own aircraft's state */
    QTimer *m_timerContextWatchdog; /*!< core available? */
    QTimer *m_timerStatusBar; /*!< cleaning up status bar */
    QTimer *m_timerSimulator; /*!< update simulator data */

    // frameless window
    QPoint m_dragPosition; /*!< position, if moving is handled with frameless window */

    // context menus
    QMenu *m_contextMenuAudio; /*!< audio context menu */
    QMenu *m_contextMenuStatusMessageList; /*!< context menu for status message list */

    // cockpit
    QString m_transponderResetValue; /*!< Temp. storage of XPdr mode to reset, req. until timer allows singleShoot with Lambdas */
    QWidget *m_inputFocusedWidget; /*!< currently used widget for input, mainly used with cockpit */

    // status bar
    QLabel *m_statusBarIcon; /*!< status bar icon */
    QLabel *m_statusBarLabel; /*!< status bar label */

    //! GUI status update
    void updateGuiStatusInformation();

    //! 1st data reads
    void initialDataReads();

    //! Init GUI signals
    void initGuiSignals();

    //! Init the context menus
    void initContextMenus();

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
    void startUpdateTimers();

    //! Stop all update timers
    void stopUpdateTimers();

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
    static const QString &sampleBlackGuiOriginator()
    {
        static const QString o = QString("GUISAMPLE1:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
        return o;
    }

private slots:

    //
    // Data received related slots
    //

    //! Reload own aircraft
    bool ps_reloadOwnAircraft();

    //! Display status message
    void ps_displayStatusMessageInGui(const BlackMisc::CStatusMessage &sendStatusMessage);

    //! Display status messages
    void ps_displayStatusMessagesInGui(const BlackMisc::CStatusMessageList &messages);

    //! Redirected output
    void displayRedirectedOutput(const BlackMisc::CStatusMessage &sendStatusMessage, qint64 contextId);

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

    //! Context menu for audio
    void ps_displayAudioIconContextMenu(const QPoint &position);

    //! Toogle Windows stay on top
    void ps_toogleWindowStayOnTop();

    //! Set the hotkeys
    void ps_registerHotkeys();

    //! Style sheet has been changed
    void ps_onStyleSheetsChanged();
};

#pragma pop_macro("interface")

#endif // guard
