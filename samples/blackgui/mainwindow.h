/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_MAINWINDOW_H
#define SAMPLE_MAINWINDOW_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "infowindow.h"
#include "guimodeenums.h"
#include "blackcore/context_audio.h"
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_simulator.h"
#include "blackcore/context_runtime.h"
#include "blackgui/transpondermodeselector.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackgui/serverlistmodel.h"
#include "blackgui/aircraftlistmodel.h"
#include "blackgui/userlistmodel.h"
#include "blackgui/statusmessagelistmodel.h"
#include "blackgui/keyboardkeylistmodel.h"
#include "blackmisc/nwtextmessage.h"
#include "blacksound/soundgenerator.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QTableView>
#include <QItemSelection>
#include <QLabel>
#include <QTimer>

namespace Ui
{
    class MainWindow;
}

/*!
 * \brief GUI
 */
class MainWindow : public QMainWindow
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
        MainPageAtc = 1,
        MainPageAircrafts = 2,
        MainPageUsers = 3,
        MainPageCockpit = 4,
        MainPageTextMessages = 5,
        MainPageFlightplan = 6,
        MainPageSettings = 7,
        MainPageSimulator = 8
    };

    //! Audio test modes
    enum AudioTest
    {
        NoAudioTest,
        SquelchTest,
        MicrophoneTest
    };

private:
    QScopedPointer<Ui::MainWindow> ui;
    CInfoWindow *m_infoWindow;
    bool m_init;
    GuiModes::WindowMode m_windowMode;
    AudioTest m_audioTestRunning;

    // contexts
    bool m_coreAvailable;
    bool m_contextNetworkAvailable;
    bool m_contextAudioAvailable;
    QScopedPointer<BlackCore::CRuntime> m_rt; /*!< runtime */
    BlackMisc::Aviation::CAircraft m_ownAircraft; /*!< own aircraft's state */
    QTimer *m_timerUpdateAtcStationsOnline; /*!< timer for update of stations */
    QTimer *m_timerUpdateAircraftsInRange; /*!< timer for update of aircrafts */
    QTimer *m_timerUpdateUsers; /*!< timer for update of users */
    QTimer *m_timerCollectedCockpitUpdates; /*!< collect cockpit updates over a short period before sending */
    QTimer *m_timerContextWatchdog; /*!< core available? */
    QTimer *m_timerStatusBar; /*!< cleaning up status bar */
    QTimer *m_timerAudioTests; /*!< cleaning up status bar */
    QTimer *m_timerSimulator; /*!< update simulator data */

    // pixmaps
    QPixmap m_resPixmapConnectionConnected;
    QPixmap m_resPixmapConnectionDisconnected;
    QPixmap m_resPixmapConnectionConnecting;
    QPixmap m_resPixmapVoiceHigh;
    QPixmap m_resPixmapVoiceLow;
    QPixmap m_resPixmapVoiceMuted;

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

    /*!
     * \brief Update the selected server textboxes
     * \param server to be displayed
     */
    void updateGuiSelectedServerTextboxes(const BlackMisc::Network::CServer &server);

    //! Selected server from textboxes
    BlackMisc::Network::CServer selectedServerFromTextboxes() const;

    //! 1st data reads
    void initialDataReads();

    //! Init GUI signals
    void initGuiSignals();

    //! Context network availability check, otherwise status message
    bool isContextNetworkAvailableCheck();

    //! Context voice availability check, otherwise status message
    bool isContextAudioAvailableCheck();

    //! Own cockpit, update from context
    void updateCockpitFromContext();

    //! Pending cockpit update operation
    bool isCockpitUpdatePending() const;

    //! Update the COM frequency displays
    void updateComFrequencyDisplays(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

    /*!
     * \brief Add new text message tab
     * \param tabName   name of the new tab, usually the channel name
     * \return
     */
    QWidget *addNewTextMessageTab(const QString &tabName);

    //! Find text message tab by its name
    QWidget *findTextMessageTabByName(const QString &name) const;

    /*!
     * \brief Private channel text message
     * \param textMessage
     * \param sending   sending or receiving
     */
    void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage, bool sending = false);

    /*!
     * Stub for sending a text message (eihter radio or private message).
     * Sets sender / receiver depending on frequency / channel situation.
     */
    BlackMisc::Network::CTextMessage getTextMessageStubForChannel();

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

    //! Display the overlay window
    void displayOverlayInfo(const QString &message = "");

    //! Overlay info displaying status message
    void displayOverlayInfo(const BlackMisc::CStatusMessage &message);

    /*!
     * \brief Is given main page selected?
     * \param mainPage  index to be checked
     * \return
     */
    bool isMainPageSelected(MainPageIndex mainPage) const;

    //! For this text message's recepient, is the current tab selected?
    bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

    //! Init the context menus
    void initContextMenus();

    //! Start all update timers
    void startUpdateTimers();

    /*!
     * \brief Stop all update timers
     * \param disconnect also disconnect signal/slots
     */
    void stopUpdateTimers(bool disconnect = false);

    //! Currently selected SELCAL code
    QString getSelcalCode() const;

    //! Audio test updates (timer) for progressbar and fetching results
    void audioTestUpdate();

    //! Play notifcation sound
    void playNotifcationSound(BlackSound::CSoundGenerator::Notification notification) const;

    //! Update simulator page with latest user aircraft data
    void updateSimulatorData();

private slots:

    //
    // Data received related slots
    //

    //! Reload booked stations
    void reloadAtcStationsBooked();

    //! Reload online stations
    void reloadAtcStationsOnline();

    //! Reload aircrafts in range
    void reloadAircraftsInRange();

    //! Reload all (online) users
    void reloadAllUsers();

    //! Reload own aircraft
    bool reloadOwnAircraft();

    //! Display status message
    void displayStatusMessage(const BlackMisc::CStatusMessage &statusMessage);

    //! Display status messages
    void displayStatusMessages(const BlackMisc::CStatusMessageList &messages);

    //! Redirected output
    void displayRedirectedOutput(const BlackMisc::CStatusMessage &statusMessage, qint64 contextId);

    /*!
     * \brief Connection status changed
     * \param from  old status, as uint so it is compliant with DBus
     * \param to    new status, as uint so it is compliant with DBus
     */
    void connectionStatusChanged(uint from, uint to, const QString &message);

    /*!
     * \brief Append text messages (received, to be sent) to GUI
     * \param messages
     * \param sending
     */
    void appendTextMessagesToGui(const BlackMisc::Network::CTextMessageList &messages, bool sending = false);

    //! Reload settings
    void reloadSettings();

    //! Send cockpit updates
    void sendCockpitUpdates();

    //! Simulator available
    void simulatorConnectionChanged(bool isAvailable);

    //
    // GUI related slots
    //

    /*!
     * \brief Set the main page
     * \param start Startup phase
     */
    void setMainPage(bool start = false);

    /*!
     * \brief setMainPage
     * \param mainPage
     */
    void setMainPage(MainPageIndex mainPage);

    //! Connect to network
    void toggleNetworkConnection();

    //! Menu item clicked
    void menuClicked();

    //! Terminated connection
    void connectionTerminated();

    /*!
     * \brief Network server selected
     * \param index
     */
    void networkServerSelected(QModelIndex index);

    /*!
     * \brief Online ATC station selected
     * \param index
     */
    void onlineAtcStationSelected(QModelIndex index);

    //! Alter traffic server
    void alterTrafficServer();

    //! Network settings have been changed
    void changedSettings(uint typeValue);

    //! Update timer
    void timerBasedUpdates();

    /*!
     * \brief ATC station, tab changed, reload data
     * \param tabIndex
     */
    void atcStationTabChanged(int tabIndex);

    /*!
     * \brief Middle panel has changed, reload data
     * \param index
     */
    void middlePanelChanged(int index);

    //! Command entered
    void commandEntered();

    //! Get METAR for given ICAO airport code
    void getMetar(const QString &airportIcaoCode = "");

    //! Request new ATIS
    void requestAtis();

    //! Close text message tab
    void closeTextMessageTab();

    //! Cockpit values changed
    void cockpitValuesChanged();

    /*!
     * \brief Audio device selected
     * \param index audio device index (COM1, COM2)
     */
    void audioDeviceSelected(int index);

    //! Reset transponder to Standby / Charly
    void resetTransponderMode();

    /*!
     * Set voice rooms, also
     * allows to set an arbitrary voice room for testing purposes
     */
    void setAudioVoiceRooms();

    //! Audio volume handling and mute
    void audioVolumes();

    /*!
     * \brief changeOpacity
     * \param opacity 0-100
     */
    void changeWindowOpacity(int opacity = -1);

    //! Context menu for audio
    void audioIconContextMenu(const QPoint &position);

    //! Test SELCAL (code valid? play tone)
    void testSelcal();

    //! start the MIC tests (Squelch)
    void startAudioTest();

    //! \brief Input focus changed, used to detect whether an input control has focus
    //! \sa QApplication::focusChanged
    void inputFocusChanged(QWidget *oldWidget, QWidget *newWidget);

    //! Save the Hotkeys
    void saveHotkeys();

    //! Clear single hotkey
    void clearHotkey();

    //! Test the SIM connect connectivity
    void testSimConnectConnection();

    //! Save a simconnect.cfg file for FSX
    void saveSimConnectCfg();

    //! simConnect.cfg: open, exists? delete
    void simConnectCfgFile();
};

#pragma pop_macro("interface")

#endif // guard
