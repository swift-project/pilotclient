#ifndef BLACKMISC_MAINWINDOW_H
#define BLACKMISC_MAINWINDOW_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "blackgui/atcstationlistmodel.h"
#include "blackgui/serverlistmodel.h"
#include "blackgui/aircraftlistmodel.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/nwtextmessage.h"
#include "blackcore/context_network_interface.h"
#include "blackcore/context_settings_interface.h"
#include "blackcore/coreruntime.h"
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
     * \param parent
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /*!
     * Destructor
     */
    ~MainWindow();

    /*!
     * \brief Init data
     * \param withDBus
     */
    void init(bool withDBus);

    /*!
     * \brief Graceful shutdown
     */
    void gracefulShutdown();

private:
    Ui::MainWindow *ui;
    bool m_init;
    bool m_withDBus;
    bool m_contextNetworkAvailable;
    QDBusConnection m_dBusConnection;
    BlackCore::CCoreRuntime *m_coreRuntime; /*!< runtime, if working with local core */
    BlackGui::CAtcListModel *m_atcListOnline;
    BlackGui::CAtcListModel *m_atcListBooked;
    BlackGui::CServerListModel *m_trafficServerList;
    BlackGui::CAircraftListModel *m_aircraftsInRange;
    BlackCore::IContextNetwork *m_contextNetwork;
    BlackCore::IContextSettings *m_contextSettings;
    BlackMisc::Aviation::CAircraft m_ownAircraft;
    QTimer *m_timerUpdateAtcStationsOnline;
    QTimer *m_timerUpdateAircraftsInRange;
    QTimer *m_timerCollectedCockpitUpdates;
    QTimer *m_timerContextWatchdog;

    /*!
     * \brief GUI status update
     */
    void updateGuiStatusInformation();

    /*!
     * \brief Update the selected server textboxs
     * \param server
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
     * \brief Context network available check, otherwise status message
     * \return
     */
    bool isContextNetworkAvailableCheck();

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
    QWidget *findTextMessageTabByName(const QString &name);

    /*!
     * \brief Private channel text message
     * \param textMessage
     * \param sending
     */
    void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage, bool sending = false);

    /*!
     * \brief stub for sending a text message (eihter radio or private message)
     * \return
     */
    BlackMisc::Network::CTextMessage getTextMessageStubForChannel();

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
    void displayStatusMessages(const BlackMisc::CStatusMessages &messages);

    /*!
     * \brief Connection status changed
     * \param from
     * \param to
     */
    void connectionStatusChanged(uint from, uint to);

    /*!
     * \brief Text messages received
     * \param messages
     * \param sending
     */
    void textMessageReceived(const BlackMisc::Network::CTextMessageList &messages, bool sending = false);

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
     */
    void setMainPage(bool start = false);

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
    void updateTimer();

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

};

#pragma pop_macro("interface")

#endif // guard
