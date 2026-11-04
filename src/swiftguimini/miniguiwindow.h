// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINIGUIWINDOW_H
#define SWIFTGUIMINI_MINIGUIWINDOW_H

#include <QMainWindow>

#include "misc/network/connectionstatus.h"

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

//! Vereinfachtes Hauptfenster der Mini-GUI mit Tabs (Login, Clients, Radar, Cockpit) und Menue
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
    //! Verhindert das Beenden, solange eine aktive FSD-Verbindung besteht, und laesst
    //! stattdessen den Disconnect-Button mehrfach rot blinken, um den Benutzer darauf hinzuweisen.
    void closeEvent(QCloseEvent *event) override;

private slots:
    //! Wechselt zum Login-Tab
    void showLoginSettings();

    //! Oeffnet den Flugplan-Dialog
    void showFlightPlan();

    //! Oeffnet den Simulator-Konfigurationsdialog
    void showSimulatorSettings();

    //! Oeffnet den Miscellaneous-Einstellungsdialog
    void showMiscSettings();

    //! Netzwerkstatus hat sich geaendert (fuer automatischen Tabwechsel nach Connect)
    void onNetworkStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                const swift::misc::network::CConnectionStatus &to);

    //! Laedt/aktualisiert das Mini-GUI-Stylesheet (initial und bei Aenderung der .qss-Dateien)
    void initStyleSheet();

private:
    //! Initialisiert die Tabs
    void initTabs();

    //! Initialisiert die Menueleiste
    void initMenus();

    QTabWidget *m_tabs = nullptr;
    int m_loginTabIndex = -1;
    int m_clientsTabIndex = -1;
    int m_cockpitTabIndex = -1;

    CMiniLoginWidget *m_loginComponent = nullptr;
    swift::gui::components::CMiniClientsWidget *m_clientsWidget = nullptr;
    swift::gui::components::CMiniRadarWidget *m_radarWidget = nullptr;
    swift::gui::components::CMiniCockpitWidget *m_cockpitWidget = nullptr;

    CMiniFlightPlanDialog *m_flightPlanDialog = nullptr;
    CMiniSimulatorSettingsDialog *m_simSettingsDialog = nullptr;
    CMiniStatusBarWidget *m_statusBarWidget = nullptr;
    CMiniMiscSettingsDialog *m_miscSettingsDialog = nullptr;
};

#endif // SWIFTGUIMINI_MINIGUIWINDOW_H