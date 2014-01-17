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
#include "blackcore/context_voice.h"
#include "blackcore/context_network_interface.h"
#include "blackcore/context_settings_interface.h"
#include "blackcore/context_application_interface.h"
#include "blackcore/coreruntime.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackgui/serverlistmodel.h"
#include "blackgui/aircraftlistmodel.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/nwtextmessage.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QItemSelection>
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
     */
    enum MainPageIndex
    {
        MainPageStatus = 0,
        MainPageAtc = 1,
        MainPageAircrafts = 2,
        MainPageCockpit = 3,
        MainPageTextMessages = 4,
        MainPageFlightplan = 5,
        MainPageSettings = 6
    };

private:
    QScopedPointer<Ui::MainWindow> ui;
    CInfoWindow *m_infoWindow;
    bool m_init;
    GuiModes::WindowMode m_windowMode;
    GuiModes::CoreMode m_coreMode;
    bool m_coreAvailable;
    bool m_contextNetworkAvailable;
    bool m_contextVoiceAvailable;
    QDBusConnection m_dBusConnection;
    QScopedPointer<BlackCore::CCoreRuntime> m_coreRuntime; /*!< runtime, if working with local core */
    BlackGui::CAtcListModel *m_atcListOnline;
    BlackGui::CAtcListModel *m_atcListBooked;
    BlackGui::CServerListModel *m_trafficServerList;
    BlackGui::CAircraftListModel *m_aircraftsInRange;
    BlackCore::IContextApplication *m_contextApplication; /*!< overall application state */
    BlackCore::IContextNetwork *m_contextNetwork;
    BlackCore::IContextVoice *m_contextVoice;
    BlackCore::IContextSettings *m_contextSettings;
    BlackMisc::Aviation::CAircraft m_ownAircraft; /*!< own aircraft's state */
    BlackMisc::Voice::CVoiceRoom m_voiceRoomCom1;
    BlackMisc::Voice::CVoiceRoom m_voiceRoomCom2;
    QTimer *m_timerUpdateAtcStationsOnline; /*!< timer for update of stations */
    QTimer *m_timerUpdateAircraftsInRange; /*!< timer for update of aircrafts */
    QTimer *m_timerCollectedCockpitUpdates; /*!< collect cockpit updates over a short period before sending */
    QTimer *m_timerContextWatchdog; /*!< core available? */
    QPixmap m_resPixmapConnectionConnected;
    QPixmap m_resPixmapConnectionDisconnected;
    QPixmap m_resPixmapConnectionConnecting;
    QPixmap m_resPixmapVoiceHigh;
    QPixmap m_resPixmapVoiceLow;
    QPixmap m_resPixmapVoiceMuted;
    QPoint m_dragPosition; /*!< position, if moving is handled with frameless window */
    QMenu *m_contextMenuAudio; /*! Audio context menu */
    QString m_transponderResetValue; /*! Temp. storage of XPdr mode to reset, req. until timer allows singleShoot with Lambdas */

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
     * \return
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
     * \return
     */
    bool isContextNetworkAvailableCheck();

    /*!
     * \brief Context voice availability check, otherwise status message
     * \return
     */
    bool isContextVoiceAvailableCheck();

    /*!
     * \brief Own cockpit, update from context
     */
    void updateCockpitFromContext();

    /*!
     * \brief Pending cockpit update operation
     * \return
     */
    bool isCockpitUpdatePending() const;

    /*!
     * \brief Add new text message tab
     * \param tabName
     * \return
     */
    QWidget *addNewTextMessageTab(const QString &tabName);

    /*!
     * \brief Find text message tab by name
     * \param name
     * \return
     */
    QWidget *findTextMessageTabByName(const QString &name) const;

    /*!
     * \brief Private channel text message
     * \param textMessage
     * \param sending
     */
    void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage, bool sending = false);

    /*!
     * Stub for sending a text message (eihter radio or private message).
     * Sets sender / receiver depending on frequency / channel situation.
     * \return
     */
    BlackMisc::Network::CTextMessage getTextMessageStubForChannel();

    /*!
     * \brief Audio device lists
     * \return
     */
    void setAudioDeviceLists();

    /*!
     * \brief Context availability, used by watchdog
     */
    void setContextAvailability();

    /*!
     * \brief Position of own plane for testing
     * \param wgsLatitude
     * \param wgsLongitude
     * \param altitude
     */
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude);

    /*!
     * \brief Display the overlay window
     */
    void displayOverlayInfo(const QString &message = "");

    /*!
    * \brief Overlay info by status message
    * \param message
    */
    void displayOverlayInfo(const BlackMisc::CStatusMessage &message);

    /*!
     * \brief Is given main page selected?
     * \param mainPage
     * \return
     */
    bool isMainPageSelected(MainPageIndex mainPage) const;

    /*!
     * \brief For this text message's receiver, is the current tab selected
     * \param textMessage
     * \return
     */
    bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

    /*!
     * \brief Init the context menus
     */
    void initContextMenus();

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
     * \brief Reload own aircraft
     * \return
     */
    bool reloadOwnAircraft();

    /*!
     * \brief Display status message
     * \param message
     */
    void displayStatusMessage(const BlackMisc::CStatusMessage &message);

    /*!
     * \brief Display status messages
     * \param messages
     */
    void displayStatusMessages(const BlackMisc::CStatusMessageList &messages);

    /*!
     * \brief Connection status changed
     * \param from
     * \param to
     */
    void connectionStatusChanged(uint from, uint to);

    /*!
     * \brief Append text messages (received, to be sent) to GUI
     * \param messages
     * \param sending
     */
    void appendTextMessagesToGui(const BlackMisc::Network::CTextMessageList &messages, bool sending = false);

    /*!
     * \brief Reload settings
     */
    void reloadSettings();

    /*!
     * \brief Send cockpit updates
     */
    void sendCockpitUpdates();

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
    void changedNetworkSettings();

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
     * \brief Override voice room (allows to set an arbitrary voice room for testing purposes)
     */
    void voiceRoomOverride();

    /*!
     * \brief Audio volume handling and mute
     */
    void audioVolumes();

    /*!
     * \brief changeOpacity
     * \param opacity 0-100
     */
    void changeWindowOpacity(int opacity = -1);

    /*!
     * \brief Context menu for audio
     */
    void audioIconContextMenu(const QPoint &position);

};

#pragma pop_macro("interface")

#endif // guard
