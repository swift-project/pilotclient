// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniguiwindow.h"

#include <QAction>
#include <QCloseEvent>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "miniMiscSettingsDialog.h"
#include "miniflightplandialog.h"
#include "miniloginwidget.h"
#include "minisimulatorsettingsdialog.h"
#include "ministatusbarwidget.h"

#include "core/context/contextnetwork.h"
#include "gui/components/miniclientswidget.h"
#include "gui/components/minicockpitwidget.h"
#include "gui/components/miniradarwidget.h"
#include "gui/guiapplication.h"
#include "gui/stylesheetutility.h"
#include "misc/icons.h"

using namespace swift::core::context;
using namespace swift::gui;
using namespace swift::gui::components;
using namespace swift::misc;
using namespace swift::misc::network;

CMiniGuiWindow::CMiniGuiWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle("swift Mini GUI");
    this->resize(1000, 700);

    this->initTabs();
    this->initMenus();
    this->initStyleSheet();

    if (sGui && sGui->getIContextNetwork())
    {
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CMiniGuiWindow::onNetworkStatusChanged, Qt::QueuedConnection);
    }

    if (sGui)
    {
        // Live-Reload: .qss-Dateien werden von CStyleSheetUtility ueberwacht (QFileSystemWatcher)
        // und bei Aenderung automatisch neu eingelesen -> Stylesheet hier erneut anwenden.
        connect(&sGui->getStyleSheetUtility(), &CStyleSheetUtility::styleSheetsChanged, this,
                &CMiniGuiWindow::initStyleSheet, Qt::QueuedConnection);
    }
}

CMiniGuiWindow::~CMiniGuiWindow() {}

void CMiniGuiWindow::initStyleSheet()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    const QString s = sGui->getStyleSheetUtility().styles({ CStyleSheetUtility::fileNameFonts(),
                                                            CStyleSheetUtility::fileNameStandardWidget(),
                                                            CStyleSheetUtility::fileNameSwiftMiniGui() });
    this->setStyleSheet(""); // siehe swiftguistdinit.cpp: Flicker/Crash-Workaround beim 2. Setzen vermeiden
    this->setStyleSheet(s);
}

void CMiniGuiWindow::initTabs()
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_statusBarWidget = new CMiniStatusBarWidget(central);
    mainLayout->addWidget(m_statusBarWidget);

    m_tabs = new QTabWidget(central);
    mainLayout->addWidget(m_tabs, 1);

    central->setLayout(mainLayout);
    this->setCentralWidget(central);

    m_loginComponent = new CMiniLoginWidget(m_tabs);
    m_loginTabIndex = m_tabs->addTab(m_loginComponent, CIcons::globe16(), tr("Login"));

    m_clientsWidget = new CMiniClientsWidget(m_tabs);
    m_clientsTabIndex = m_tabs->addTab(m_clientsWidget, CIcons::appAtc16(), tr("Clients"));

    m_radarWidget = new CMiniRadarWidget(m_tabs);
    m_tabs->addTab(m_radarWidget, CIcons::appRadar16(), tr("Radar"));

    m_cockpitWidget = new CMiniCockpitWidget(m_tabs);
    m_cockpitTabIndex = m_tabs->addTab(m_cockpitWidget, CIcons::appCockpit16(), tr("Cockpit"));
}

void CMiniGuiWindow::initMenus()
{
    QMenu *menuSettings = menuBar()->addMenu("Einstellungen");

    QAction *actionLoginSettings = menuSettings->addAction("Login-Konfiguration");
    connect(actionLoginSettings, &QAction::triggered, this, &CMiniGuiWindow::showLoginSettings);

    QAction *actionFlightPlan = menuSettings->addAction("Flugplanverwaltung");
    connect(actionFlightPlan, &QAction::triggered, this, &CMiniGuiWindow::showFlightPlan);

    QAction *actionSimSettings = menuSettings->addAction("Simulator-Konfiguration");
    connect(actionSimSettings, &QAction::triggered, this, &CMiniGuiWindow::showSimulatorSettings);

    QAction *actionMiscSettings = menuSettings->addAction("Miscellaneous");
    connect(actionMiscSettings, &QAction::triggered, this, &CMiniGuiWindow::showMiscSettings);

    menuSettings->addSeparator();

    QAction *actionQuit = menuSettings->addAction("Beenden");
    actionQuit->setShortcut(QKeySequence::Quit);
    connect(actionQuit, &QAction::triggered, this, &CMiniGuiWindow::close);
}

void CMiniGuiWindow::showLoginSettings()
{
    if (m_tabs && m_loginTabIndex >= 0) { m_tabs->setCurrentIndex(m_loginTabIndex); }
}

void CMiniGuiWindow::showFlightPlan()
{
    if (!m_flightPlanDialog) { m_flightPlanDialog = new CMiniFlightPlanDialog(this); }
    m_flightPlanDialog->show();
    m_flightPlanDialog->raise();
    m_flightPlanDialog->activateWindow();
}

void CMiniGuiWindow::showSimulatorSettings()
{
    if (!m_simSettingsDialog) { m_simSettingsDialog = new CMiniSimulatorSettingsDialog(this); }
    m_simSettingsDialog->show();
    m_simSettingsDialog->raise();
    m_simSettingsDialog->activateWindow();
}

void CMiniGuiWindow::showMiscSettings()
{
    if (!m_miscSettingsDialog) { m_miscSettingsDialog = new CMiniMiscSettingsDialog(this); }
    m_miscSettingsDialog->show();
    m_miscSettingsDialog->raise();
    m_miscSettingsDialog->activateWindow();
}

void CMiniGuiWindow::onNetworkStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
{
    Q_UNUSED(from)
    if (to.isConnected() && m_tabs && m_cockpitTabIndex >= 0) { m_tabs->setCurrentIndex(m_cockpitTabIndex); }
}

void CMiniGuiWindow::closeEvent(QCloseEvent *event)
{
    const bool connected = sGui && sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected();
    if (connected)
    {
        // Beenden verhindern, solange eine aktive FSD-Verbindung besteht.
        event->ignore();

        // Benutzer zum Login/Cockpit-Tab bringen, damit der blinkende Disconnect-Button sichtbar ist.
        if (m_tabs && m_loginTabIndex >= 0) { m_tabs->setCurrentIndex(m_loginTabIndex); }
        if (m_loginComponent) { m_loginComponent->blinkDisconnectButton(); }

        QMessageBox::warning(this, tr("Aktive Verbindung"),
                             tr("Es besteht noch eine aktive FSD-Verbindung.\n"
                                "Bitte zuerst trennen (Disconnect), bevor das Programm beendet wird."));
        return;
    }

    event->accept();
    QMainWindow::closeEvent(event);
}