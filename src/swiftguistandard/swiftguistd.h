// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef STDGUI_SWIFTGUISTD_H
#define STDGUI_SWIFTGUISTD_H

#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/navigatordialog.h"
#include "blackgui/components/aircraftmodelsetvalidationdialog.h"
#include "blackgui/components/textmessagecomponenttab.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/guiactionbind.h"
#include "blackcore/actionbind.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/autopublishdata.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/variant.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/loghistory.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/icons.h"

#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QAction;
class QCloseEvent;
class QEvent;
class QMouseEvent;
class QTimer;

namespace BlackMisc::Aviation
{
    class CAltitude;
}
namespace BlackGui::Components
{
    class CDbLoadDataDialog;
    class CAutoPublishDialog;
    class CLoginDialog;
    class CModelBrowserDialog;
    class CAfvMapDialog;
}
namespace Ui
{
    class SwiftGuiStd;
}

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
        MainPageInfoArea = 0,
        MainPageLogin = 1,
        MainPageInternals = 2,
        MainPageInvisible = 3
    };

    //! Constructor
    SwiftGuiStd(WindowMode windowMode, QWidget *parent = nullptr);

    //! Destructor
    virtual ~SwiftGuiStd() override;

signals:
    //! Main info area has changed
    //! \remarks using widget pointer allows the component itself to identify if it is current
    void currentMainInfoAreaChanged(const QWidget *currentWidget);

protected:
    //! \name QMainWindow events
    //! @{

    //! \copydoc QMainWindow::mouseMoveEvent
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    //! \copydoc QMainWindow::mousePressEvent
    virtual void mousePressEvent(QMouseEvent *event) override;

    //! \copydoc QMainWindow::mouseReleaseEvent
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    //! \copydoc QMainWindow::closeEvent
    virtual void closeEvent(QCloseEvent *event) override;

    //! \copydoc QMainWindow::changeEvent
    virtual void changeEvent(QEvent *event) override;
    //! @}

    //! Get a minimize action which minimizes the window

    //! @{
    //! Actions for navigator
    QAction *getWindowMinimizeAction(QObject *parent);
    QAction *getWindowNormalAction(QObject *parent);
    QAction *getToggleWindowVisibilityAction(QObject *parent);
    QAction *getToggleStayOnTopAction(QObject *parent);
    //! @}

private:
    QScopedPointer<Ui::SwiftGuiStd> ui;
    QScopedPointer<BlackGui::Components::CNavigatorDialog> m_navigator { new BlackGui::Components::CNavigatorDialog() }; //!< navigator dialog bar, if I pass the parent, the dialog is always centered over the parent
    QScopedPointer<BlackGui::Components::CDbLoadDataDialog> m_dbLoadDialog; //!< load DB data, lazy init UI component
    QScopedPointer<BlackGui::Components::CAutoPublishDialog> m_autoPublishDialog; //!< auto publish dialog
    QScopedPointer<BlackGui::Components::CLoginDialog> m_loginDialog; //!< login dialog
    QScopedPointer<BlackGui::Components::CModelBrowserDialog> m_modelBrower; //!< model browser
    QScopedPointer<BlackGui::Components::CAfvMapDialog> m_mapDialog; //!< map dialog
    QScopedPointer<BlackGui::Components::CAircraftModelSetValidationDialog> m_validationDialog; //!< aircraft model validation dialog
    BlackMisc::CData<BlackMisc::Simulation::Data::TLastAutoPublish> m_lastAutoPublish { this };
    BlackCore::CActionBind m_actionPtt { BlackMisc::Input::pttHotkeyAction(), BlackMisc::CIcons::StandardIconRadio16, this, &SwiftGuiStd::onPttChanged };
    BlackCore::CActionBindings m_menuHotkeyHandlers;
    BlackGui::CManagedStatusBar m_statusBar;
    BlackMisc::CLogHistoryReplica m_logHistoryForLogButtons { this };
    BlackMisc::CLogHistoryReplica m_logHistoryForOverlay { this };
    BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this };

    // contexts
    static constexpr int MaxCoreFailures = 5; //!< Failures counted before reconnecting
    int m_coreFailures = 0; //!< failed access to core
    bool m_init = false;
    bool m_coreAvailable = false; //!< core already available?
    bool m_contextNetworkAvailable = false; //!< network context available?
    bool m_contextAudioAvailable = false; //!< audio context available?
    bool m_displayingDBusReconnect = false; //!< currently displaying reconnect dialog
    bool m_dbDataLoading = false; //!< DB or shared data loading in progress
    QTimer m_timerContextWatchdog; //!< core available?
    BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft; //!< own aircraft's state

    //! GUI status update
    void updateGuiStatusInformation();

    //! Set style sheet
    void initStyleSheet();

    //! 1st data reads
    void initialContextDataReads();

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
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude, const BlackMisc::Aviation::CAltitude &pressureAltitude);

    //! Is given main page selected?
    //! \param mainPage index to be checked
    bool isMainPageSelected(MainPageIndex mainPage) const;

    //! Stop all timers
    //! \param disconnectSignalSlots also disconnect signal/slots
    void stopAllTimers(bool disconnectSignalSlots);

    //! Play notifcation sound
    void playNotifcationSound(BlackMisc::Audio::CNotificationSounds::NotificationFlag notification) const;

    //! Display log
    void displayLog();

    //! Display network settings
    void displayNetworkSettings();

    //! Display a reconnect dialog
    void displayDBusReconnectDialog();

    //! PTT changed
    void onPttChanged(bool enabled);

    //
    // Data receiving related funtions
    //

    //! Reload own aircraft
    bool reloadOwnAircraft();

    //! Connection status changed
    //! \param from old status
    //! \param to   new status
    void onConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

    //
    // GUI related functions
    //

    //! Set \sa MainPageInfoArea
    void setMainPageToInfoArea() { this->setMainPage(MainPageInfoArea); }

    //! Set one of the main pages
    void setMainPage(MainPageIndex mainPage);

    //! Set the main info area
    void setMainPageInfoArea(BlackGui::Components::CMainInfoAreaComponent::InfoArea infoArea);

    //! Display the settings page
    void setSettingsPage(int settingsTabIndex = -1);

    //! Login requested
    void loginRequested();

    //! Menu item clicked
    void onMenuClicked();

    //! Attach the simulator window
    void attachSimulatorWindow();

    //! Detach simulator window
    void detachSimulatorWindow();

    //! Kicked from network
    void onKickedFromNetwork(const QString &kickMessage);

    //! Update timer
    void handleTimerBasedUpdates();

    //! Change opacity 0-100
    void onChangedWindowOpacityTo50(bool) { onChangedWindowOpacity(50); }

    //! Change opacity 0-100
    void onChangedWindowOpacityTo100(bool) { onChangedWindowOpacity(100); }

    //! Change opacity 0-100
    void onChangedWindowOpacity(int opacity = -1);

    //! Toggle if windows stays on top
    //! \remark mostly used with navigator
    void toggleWindowStayOnTop();

    //! Toggle window visibility
    //! \remark mostly used with navigator
    void toggleWindowVisibility();

    //! Style sheet has been changed
    void onStyleSheetsChanged();

    //! Toggle window on top
    void onToggledWindowsOnTop(bool onTop);

    //! Main info area current widget changed
    void onCurrentMainWidgetChanged(int currentIndex);

    //! Whole main info area floating
    void onChangedMainInfoAreaFloating(bool floating);

    //! Reported issue with the client
    void onAudioClientFailure(const BlackMisc::CStatusMessage &msg);

    //! Focus in main entry window
    void focusInMainEntryField();

    //! Focus in the text message entry field
    void focusInTextMessageEntryField();

    //! Show window minimized
    void showMinimized();

    //! Show window normal
    void showNormal();

    //! Navigator dialog has been closed
    void onNavigatorClosed();

    //! Checks if model set is available
    void verifyPrerequisites();

    //! Model set haas been verfied
    void onValidatedModelSet(const BlackMisc::Simulation::CSimulatorInfo &simulator, const BlackMisc::Simulation::CAircraftModelList &valid, const BlackMisc::Simulation::CAircraftModelList &invalid, bool stopped, const BlackMisc::CStatusMessageList &msgs);

    //! Display validation dialog
    void displayValidationDialog();

    //! Ckeck if the DB data have been loaded
    void checkDbDataLoaded();

    //! Copy the xswiftbus files from build directory
    void copyXSwiftBusDialog(bool checkFileTimestamp);

    //! Auto publish diloag
    int autoPublishDialog();

    //! Show auto publish dialog if appropriate
    bool triggerAutoPublishDialog();

    //! Start the model browser
    bool startModelBrowser();

    //! Start AFV map
    bool startAFVMap();

    //! @{
    //! Request overlay inline text message
    void onShowOverlayVariant(const BlackMisc::CVariant &variant, int durationMs);
    void onShowOverlayInlineTextMessageTab(BlackGui::Components::TextMessageTab tab);
    void onShowOverlayInlineTextMessageCallsign(const BlackMisc::Aviation::CCallsign &callsign);
    //! @}
};

// #pragma pop_macro("interface")

#endif // guard
