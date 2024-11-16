// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef STDGUI_SWIFTGUISTD_H
#define STDGUI_SWIFTGUISTD_H

#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "core/actionbind.h"
#include "gui/components/aircraftmodelsetvalidationdialog.h"
#include "gui/components/maininfoareacomponent.h"
#include "gui/components/navigatordialog.h"
#include "gui/components/textmessagecomponenttab.h"
#include "gui/enableforframelesswindow.h"
#include "gui/guiactionbind.h"
#include "gui/mainwindowaccess.h"
#include "gui/managedstatusbar.h"
#include "misc/audio/audiosettings.h"
#include "misc/audio/notificationsounds.h"
#include "misc/icons.h"
#include "misc/identifiable.h"
#include "misc/input/actionhotkeydefs.h"
#include "misc/loghandler.h"
#include "misc/loghistory.h"
#include "misc/network/connectionstatus.h"
#include "misc/simulation/autopublishdata.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/statusmessage.h"
#include "misc/variant.h"

class QAction;
class QCloseEvent;
class QEvent;
class QMouseEvent;
class QTimer;

namespace swift::misc::aviation
{
    class CAltitude;
}
namespace swift::gui::components
{
    class CDbLoadDataDialog;
    class CAutoPublishDialog;
    class CModelBrowserDialog;
} // namespace swift::gui::components
namespace Ui
{
    class SwiftGuiStd;
}

//! swift GUI
class SwiftGuiStd :
    public QMainWindow,
    public swift::misc::CIdentifiable,
    public swift::gui::CEnableForFramelessWindow,
    public swift::gui::IMainWindowAccess
{
    Q_OBJECT
    Q_INTERFACES(swift::gui::IMainWindowAccess)

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
    QScopedPointer<swift::gui::components::CNavigatorDialog> m_navigator { new swift::gui::components::CNavigatorDialog() }; //!< navigator dialog bar, if I pass the parent, the dialog is always centered over the parent
    QScopedPointer<swift::gui::components::CDbLoadDataDialog> m_dbLoadDialog; //!< load DB data, lazy init UI component
    QScopedPointer<swift::gui::components::CAutoPublishDialog> m_autoPublishDialog; //!< auto publish dialog
    QScopedPointer<swift::gui::components::CModelBrowserDialog> m_modelBrower; //!< model browser
    QScopedPointer<swift::gui::components::CAircraftModelSetValidationDialog> m_validationDialog; //!< aircraft model validation dialog
    swift::misc::CData<swift::misc::simulation::data::TLastAutoPublish> m_lastAutoPublish { this };
    swift::core::CActionBind m_actionPtt { swift::misc::input::pttHotkeyAction(), swift::misc::CIcons::StandardIconRadio16, this, &SwiftGuiStd::onPttChanged };
    swift::core::CActionBindings m_menuHotkeyHandlers;
    swift::gui::CManagedStatusBar m_statusBar;
    swift::misc::CLogHistoryReplica m_logHistoryForLogButtons { this };
    swift::misc::CLogHistoryReplica m_logHistoryForOverlay { this };
    swift::misc::CSetting<swift::misc::audio::TSettings> m_audioSettings { this };

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
    swift::misc::simulation::CSimulatedAircraft m_ownAircraft; //!< own aircraft's state

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
    void setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const swift::misc::aviation::CAltitude &altitude, const swift::misc::aviation::CAltitude &pressureAltitude);

    //! Is given main page selected?
    //! \param mainPage index to be checked
    bool isMainPageSelected(MainPageIndex mainPage) const;

    //! Stop all timers
    //! \param disconnectSignalSlots also disconnect signal/slots
    void stopAllTimers(bool disconnectSignalSlots);

    //! Play notifcation sound
    void playNotifcationSound(swift::misc::audio::CNotificationSounds::NotificationFlag notification) const;

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
    void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);

    //
    // GUI related functions
    //

    //! Set \sa MainPageInfoArea
    void setMainPageToInfoArea() { this->setMainPage(MainPageInfoArea); }

    //! Set one of the main pages
    void setMainPage(MainPageIndex mainPage);

    //! Set the main info area
    void setMainPageInfoArea(swift::gui::components::CMainInfoAreaComponent::InfoArea infoArea);

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
    void onAudioClientFailure(const swift::misc::CStatusMessage &msg);

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
    void onValidatedModelSet(const swift::misc::simulation::CSimulatorInfo &simulator, const swift::misc::simulation::CAircraftModelList &valid, const swift::misc::simulation::CAircraftModelList &invalid, bool stopped, const swift::misc::CStatusMessageList &msgs);

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
    void onShowOverlayVariant(const swift::misc::CVariant &variant, int durationMs);
    void onShowOverlayInlineTextMessageTab(swift::gui::components::TextMessageTab tab);
    void onShowOverlayInlineTextMessageCallsign(const swift::misc::aviation::CCallsign &callsign);
    //! @}
};

// #pragma pop_macro("interface")

#endif // guard
