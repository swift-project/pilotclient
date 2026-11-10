// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINIGUIWINDOW_H
#define SWIFTGUIMINI_MINIGUIWINDOW_H

#include <QMainWindow>

#include "gui/settings/minisettings.h"
#include "misc/network/connectionstatus.h"
#include "misc/settingscache.h"

class QTabWidget;
class QCloseEvent;

namespace swift::gui::components
{
    class CMiniClientsWidget;
    class CMiniRadarWidget;
    class CMiniCockpitWidget;
} // namespace swift::gui::components

class CMiniFlightPlanDialog;
class CMiniSimulatorSettingsDialog;
class CMiniStatusBarWidget;
class CMiniLoginWidget;
class CMiniMiscSettingsDialog;
class CMiniLoginSettingsDialog;

//! Simplified main window of the Mini-GUI with tabs (Connect, Clients, Radar, Cockpit) and menu
class CMiniGuiWindow : public QMainWindow
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniGuiWindow(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniGuiWindow() override;

protected:
    //! \copydoc QMainWindow::closeEvent
    //! Prevents closing while an active FSD connection exists, and instead makes the
    //! disconnect button blink red several times to draw the user's attention.
    void closeEvent(QCloseEvent *event) override;

private slots:
    //! Opens the login configuration dialog (servers/connection parameters)
    void showLoginSettings();

    //! Opens the flight plan dialog
    void showFlightPlan();

    //! Opens the simulator configuration dialog
    void showSimulatorSettings();

    //! Opens the miscellaneous settings dialog
    void showMiscSettings();

    //! Network status has changed (for automatic tab switch after connect)
    void onNetworkStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                const swift::misc::network::CConnectionStatus &to);

    //! Loads/updates the Mini-GUI stylesheet (initially and when the .qss files change)
    void initStyleSheet();

    //! Current tab changed, used to collapse/restore the window for the empty "minimize" tab
    void onTabChanged(int index);

private:
    //! Initializes the tabs
    void initTabs();

    //! Initializes the menu bar
    void initMenus();

    QTabWidget *m_tabs = nullptr;
    int m_loginTabIndex = -1;
    int m_clientsTabIndex = -1;
    int m_cockpitTabIndex = -1;
    int m_minimizeTabIndex = -1; //!< empty tab used to collapse the window
    int m_expandedHeight = -1; //!< window height before collapsing

    CMiniLoginWidget *m_loginComponent = nullptr;
    swift::gui::components::CMiniClientsWidget *m_clientsWidget = nullptr;
    swift::gui::components::CMiniRadarWidget *m_radarWidget = nullptr;
    swift::gui::components::CMiniCockpitWidget *m_cockpitWidget = nullptr;

    CMiniFlightPlanDialog *m_flightPlanDialog = nullptr;
    CMiniSimulatorSettingsDialog *m_simSettingsDialog = nullptr;
    CMiniStatusBarWidget *m_statusBarWidget = nullptr;
    CMiniMiscSettingsDialog *m_miscSettingsDialog = nullptr;
    CMiniLoginSettingsDialog *m_loginSettingsDialog = nullptr;

    swift::misc::CSetting<swift::gui::settings::TMiniMinimizeOpacity> m_minimizeOpacitySetting { this };
};

#endif // SWIFTGUIMINI_MINIGUIWINDOW_H