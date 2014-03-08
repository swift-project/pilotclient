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
#include "blackcore/coreruntime.h"
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
    /*!
     * \brief Constructor
     */
    explicit MainWindow(GuiModes::WindowMode windowMode, QWidget *parent = nullptr);

    /*!
     * Destructor
     */
    ~MainWindow();

    /*!
     * \brief Init data
     */
    void init(GuiModes::CoreMode coreMode);

    /*!
     * \brief Graceful shutdown
     */
    void gracefulShutdown();

protected:
    /*!
     * \brief Close event, e.g. when window is closed
     */
    void closeEvent(QCloseEvent *event);

    /*!
     * \brief Mouse moving, required for frameless window
     */
    void mouseMoveEvent(QMouseEvent *event);

    /*!
     * \brief Mouse press, required for frameless window
     */
    void mousePressEvent(QMouseEvent *event);

    /*!
     * \brief Main page indexes
     * \remarks keep the values in sync with the real tab indexes
     */
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
    QDBusConnection m_dBusConnection;

    // the table view models
    // normal pointers, as these will be deleted by parent
    BlackGui::CStatusMessageListModel *m_statusMessageList;
    BlackGui::CAtcListModel *m_modelAtcListOnline;
    BlackGui::CAtcListModel *m_modelAtcListBooked;
    BlackGui::CServerListModel *m_modelTrafficServerList;
    BlackGui::CAircraftListModel *m_modelAircraftsInRange;
    BlackGui::CUserListModel *m_modelAllUsers;
    BlackGui::CUserListModel *m_modelUsersVoiceCom1;
    BlackGui::CUserListModel *m_modelUsersVoiceCom2;
    BlackGui::CKeyboardKeyListModel *m_modelSettingsHotKeys;

    // contexts
    GuiModes::CoreMode m_coreMode;
    bool m_coreAvailable;
    bool m_contextNetworkAvailable;
    bool m_contextAudioAvailable;
    QScopedPointer<BlackCore::CCoreRuntime> m_coreRuntime; /*!< runtime, if working with local core */
    BlackCore::IContextApplication *m_contextApplication; /*!< overall application state */
    BlackCore::IContextNetwork *m_contextNetwork;
    BlackCore::IContextAudio *m_contextAudio;
    BlackCore::IContextSettings *m_contextSettings;
    BlackCore::IContextSimulator *m_contextSimulator;
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

    /*!
     * \brief GUI status update
     */
    void updateGuiStatusInformation();

    /*!
     * \brief Update the selected server textboxes
     * \param server to be displayed
     */
    void updateGuiSelectedServerTextboxes(const BlackMisc::Network::CServer &server);

    /*!
     * \brief Selected server from textboxes
     */
    BlackMisc::Network::CServer selectedServerFromTextboxes() const;

    /*!
     * \brief 1st data reads
     */
    void initialDataReads();

    /*!
     * \brief Init GUI signals
     */
    void initGuiSignals();

    /*!
     * \brief Context network availability check, otherwise status message
     */
    bool isContextNetworkAvailableCheck();

    /*!
     * \brief Context voice availability check, otherwise status message
     */
    bool isContextAudioAvailableCheck();

    /*!
     * \brief Own cockpit, update from context
     */
    void updateCockpitFromContext();

    /*!
     * \brief Pending cockpit update operation
     */
    bool isCockpitUpdatePending() const;

    //! \brief Update the COM frequency displays
    void updateComFrequencyDisplays(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

    /*!
     * \brief Add new text message tab
     * \param tabName   name of the new tab, usually the channel name
     * \return
     */
    QWidget *addNewTextMessageTab(const QString &tabName);

    /*!
     * \brief Find text message tab by its name
     * \param name
     * \return
     */
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

    /*!
     * \brief Audio device lists
     */
    void setAudioDeviceLists();

    /*!
     * \brief Context availability, used by watchdog
     */
    void setContextAvailability();

    /*!
     * \brief Position of own plane for testing
     * \param wgsLatitude   WGS latitude
     * \param wgsLongitude  WGS longitude
     * \param altitude
     */
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude);

    /*!
     * \brief Display the overlay window
     */
    void displayOverlayInfo(const QString &message = "");

    /*!
    * \brief Overlay info displaying status message
    */
    void displayOverlayInfo(const BlackMisc::CStatusMessage &message);

    /*!
     * \brief Is given main page selected?
     * \param mainPage  index to be checked
     * \return
     */
    bool isMainPageSelected(MainPageIndex mainPage) const;

    /*!
     * \brief For this text message's receipient, is the current tab selected?
     * \param textMessage   to be checked
     * \return
     */
    bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

    /*!
     * \brief Init the context menus
     */
    void initContextMenus();

    /*!
     * \brief Start all update timers
     */
    void startUpdateTimers();

    /*!
     * \brief Stop all update timers
     * \param disconnect also disconnect signal/slots
     */
    void stopUpdateTimers(bool disconnect = false);

    /*!
     * \brief Currently selected SELCAL code
     */
    QString getSelcalCode() const;

    /*!
     * \brief Audio test updates (timer) for progressbar and fetching results
     */
    void audioTestUpdate();

    /*!
     * \brief Play notifcation sound
     */
    void playNotifcationSound(BlackSound::CSoundGenerator::Notification notification) const;
	
	//! \brief Update simulator page with latest user aircraft data
    void updateSimulatorData();

private slots:

    //
    // Data received related slots
    //

    /*!
     * \brief Reload booked stations
     */
    void reloadAtcStationsBooked();

    /*!
     * \brief Reload online stations
     */
    void reloadAtcStationsOnline();

    /*!
     * \brief Reload aircrafts in range
     */
    void reloadAircraftsInRange();

    /*!
     * \brief Reload all (online) users
     */
    void reloadAllUsers();

    /*!
     * \brief Reload own aircraft
     * \return
     */
    bool reloadOwnAircraft();

    //! \brief Display status message
    void displayStatusMessage(const BlackMisc::CStatusMessage &statusMessage);

    //! \brief Display status messages
    void displayStatusMessages(const BlackMisc::CStatusMessageList &messages);

    /*!
     * \brief Connection status changed
     * \param from  old status, as uint so it is compliant with DBus
     * \param to    new status, as uint so it is compliant with DBus
     */
    void connectionStatusChanged(uint from, uint to);

    /*!
     * \brief Append text messages (received, to be sent) to GUI
     * \param messages
     * \param sending
     */
    void appendTextMessagesToGui(const BlackMisc::Network::CTextMessageList &messages, bool sending = false);

    //!\brief Reload settings
    void reloadSettings();

    //! \brief Send cockpit updates
    void sendCockpitUpdates();

    void simulatorAvailable();

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

    /*!
     * \brief Connect to network
     * \param userId
     * \param password
     */
    void toggleNetworkConnection();

    /*!
     * \brief Menu item clicked
     */
    void menuClicked();

    /*!
     * \brief Terminated connection
     */
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

    /*!
     * \brief Alter traffic server
     */
    void alterTrafficServer();

    /*!
     * \brief Network settings have been changed
     */
    void changedSettings();

    /*!
     * \brief Update timer
     */
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

    /*!
     * \brief Command entered
     */
    void commandEntered();

    /*!
     * \brief Get METAR
     * \param airportIcaoCode
     */
    void getMetar(const QString &airportIcaoCode = "");

    //! \brief Request new ATIS
    void requestAtis();

    /*!
     * \brief Close text message tab
     */
    void closeTextMessageTab();

    /*!
     * \brief Cockpit values changed
     */
    void cockpitValuesChanged();

    /*!
     * \brief Audio device selected
     * \param index audio device index (COM1, COM2)
     */
    void audioDeviceSelected(int index);

    /*!
     * \brief Reset transponder to Standby / Charly
     */
    void resetTransponderMode();

    /*!
     * Set voice rooms, also
     * allows to set an arbitrary voice room for testing purposes
     */
    void setAudioVoiceRooms();

    //! \brief Audio volume handling and mute
    void audioVolumes();

    /*!
     * \brief changeOpacity
     * \param opacity 0-100
     */
    void changeWindowOpacity(int opacity = -1);

    //! \brief Context menu for audio
    void audioIconContextMenu(const QPoint &position);

    //! \brief Context menu for message list
    void messageListContextMenu(const QPoint &position);

    //! \brief Test SELCAL (code valid? play tone)
    void testSelcal();

    //! \brief start the MIC tests (Squelch)
    void startAudioTest();

    //! \brief Input focus changed, used to detect whether an input control has focus
    //! \sa QApplication::focusChanged
    void inputFocusChanged(QWidget *oldWidget, QWidget *newWidget);

    //! Save the Hotkeys
    void saveHotkeys();

    //! Clear single hotkey
    void clearHotkey();
};

#pragma pop_macro("interface")

#endif // guard
